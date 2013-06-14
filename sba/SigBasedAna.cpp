/**
 * This file is part of callx. The application callx performs the call
 * extraction as well as the signaling-based analysis in the VIAT system.
 *
 * http://viat.fh-koeln.de
 *
 * Copyright (C) 2013 Bernhard Mainka (mail@bmainka.de),
 * Cologne University of Applied Sciences
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * You along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * SigBasedAna.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: mainka
 */

#include "SigBasedAna.hpp"
#include "container/SbaEventMap.hpp"
#include "SbaIncident.hpp"
#include "container/SbaIncidentMap.hpp"
#include "config/CallxConfig.hpp"
#include "main/CallxTypes.hpp"
#include <unordered_set>
#include "boost/thread.hpp"

using namespace std;

namespace callx {

SigBasedAna::SigBasedAna()
        : m_callxConfig(CallxConfig::getInstance()),
          m_sbaEventMap(SbaEventMap::getInstance()),
          m_sbaIncidentMap(SbaIncidentMap::getInstance()) {
    L_t
            << "C'tor";
    classname = "SigBasedAna";
}

SigBasedAna::~SigBasedAna() {
    L_t
            << "D'tor";
}

bool SigBasedAna::stop() {
    L_t
            << "Stopping thread "
            << classname
            << ".";
    m_stopRequested = true;
    m_workerCond.notify_all();
    return CallxThread::stop();
}

void SigBasedAna::worker() {

    while (!m_stopRequested) {

        L_t
                << "Signaling-based Analyzer starting.";
        analyze();
        L_t
                << "Signaling-based Analyzer stopped.";

        // calculate wake up time point.
        auto wakeupTime = std::chrono::system_clock::now()
                + std::chrono::seconds(m_callxConfig->sba_pause);

        // Wait until the wake up time point is reached (timeout in wait_until())
        // or if the condition variable m_cond is notified (i.e. stop request).
        // Spurious wake ups are managed by while condition. Cannot use lambda
        // function in wait_until(...) using gcc 4.4.5 (n/a).
        unique_lock workerCondLock(m_workerCondMutex);
        while ((m_workerCond.wait_until(workerCondLock, wakeupTime) == true)
                && !m_stopRequested) {
        }
    }
    L_t
            << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

void SigBasedAna::analyze() {

    m_analyzeTs = systemClock::now();

    // for every caller in SbaEventMap...
    for (auto callerIter = m_sbaEventMap->begin();
            callerIter != m_sbaEventMap->end(); callerIter++) {

        m_currCaller = callerIter->first;
        m_currSbaEventDeque = callerIter->second;

        analyzeCallAttempts();
        analyzeCallsConcurrent();
        analyzeCallCompletion();
        analyzeCallDurationAverage();
        analyzeCallDurationCumulative();
        analyzeCallsClosedByCallee();
    }
}

void SigBasedAna::analyzeCallAttempts() {
    int period = m_callxConfig->sba_call_attempts_period;
    int call_attempts_max = m_callxConfig->sba_call_attempts_max;
    int call_attempts = 0;
    unordered_set<string> callIdSet;

    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {

        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_INVITE) {
            auto event = static_cast<InviteEvent*>((*iter).get());

            // no reinvite, with final response
            if (!event->reinviteFlag
                    && event->finalResponseCode != rc_UNDEFINED)
                callIdSet.insert(event->callId);
        }
    }

    call_attempts = callIdSet.size();

    // calculate / check result

    if (call_attempts > call_attempts_max) {
        stringstream reasonStream;
        reasonStream << call_attempts
                << " call attempts in period of "
                << period
                << " minutes";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALL_ATTEMPTS,
                        m_currCaller,
                        period,
                        call_attempts,
                        true,
                        reasonStream.str()));

        string type = incidentTypeToString(it_CALL_ATTEMPTS);

        L_t
                << incidentTypeToString(it_CALL_ATTEMPTS)
                << ": "
                << reasonStream.str();

    }
}

void SigBasedAna::analyzeCallsConcurrent() {
    int period = m_callxConfig->sba_calls_concurrent_period;
    int calls_concurrent_max = m_callxConfig->sba_calls_concurrent_max;
    int call_counter = 0;
    int calls_concurrent = 0;
    unordered_set<string> callIdSet;

    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {

        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_INVITE) {

            // Increasing call counter, if it is an ACK'ed initial INVITE event
            // with final response, so that 200 <= response < 300.
            auto event = static_cast<InviteEvent*>((*iter).get());
            if (!event->reinviteFlag && event->ackFlag
                    && (event->finalResponseCode >= rc_SUCCESS
                            && event->finalResponseCode < rc_REDIRECTION)) {
                callIdSet.insert(event->callId);
                call_counter++;
                if (call_counter > calls_concurrent)
                    calls_concurrent++;
            }
        } else if ((*iter)->eventType == et_BYE) {

            // Handle BYE event. We consider initial BYE events with no final
            // response.
            auto event = static_cast<ByeEvent*>((*iter).get());
            if (event->finalResponseCode == rc_UNDEFINED)
                continue;
            auto findIter = callIdSet.find(event->callId);
            if (findIter != callIdSet.end()) {

                // Hang up an ongoing call.
                callIdSet.erase(findIter);
                call_counter--;
            } else {

                // The related INVITE event does not lie in this analyze period.
                // We know that there is an ongoing call.
                calls_concurrent++;
            }
        }
    }

    // calculate / check result

    if (calls_concurrent > calls_concurrent_max) {

        stringstream reasonStream;
        reasonStream << calls_concurrent
                << " concurrent calls in period of "
                << period
                << " minutes";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALLS_CONCURRENT,
                        m_currCaller,
                        period,
                        calls_concurrent,
                        true,
                        reasonStream.str()));
        L_t
                << incidentTypeToString(it_CALLS_CONCURRENT)
                << ": "
                << reasonStream.str();
    }
}

void SigBasedAna::analyzeCallCompletion() {
    int period = m_callxConfig->sba_call_completion_period;
    int call_completion_min = m_callxConfig->sba_call_completion_min;
    int call_attemps_min = m_callxConfig->sba_call_completion_attempts_min;
    int call_attemps = 0;
    int calls_completed = 0;
    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {
        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_INVITE) {
            auto event = static_cast<InviteEvent*>((*iter).get());
            if (event->reinviteFlag)
                continue;

            if (event->finalResponseCode == rc_UNDEFINED)
                call_attemps++;

            if (event->ackFlag && event->finalResponseCode == rc_SUCCESS)
                calls_completed++;
        }
    }

    // calculate / check result

    // to few attempts?
    if (call_attemps < call_attemps_min)
        return;

    int completion_ratio = floor(((calls_completed * 1.0 / call_attemps) * 100)
            + 0.5);

    if (completion_ratio < call_completion_min) {
        stringstream reasonStream;
        reasonStream << completion_ratio
                << "% call completion in period of "
                << period
                << " minutes";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALL_COMPLETION,
                        m_currCaller,
                        period,
                        completion_ratio,
                        false,
                        reasonStream.str()));
        L_t
                << incidentTypeToString(it_CALL_COMPLETION)
                << ": "
                << reasonStream.str();
    }

}

void SigBasedAna::analyzeCallDurationAverage() {
    int period = m_callxConfig->sba_call_duration_average_period;
    int duration_average_min = m_callxConfig->sba_call_duration_average_min;
    int completed_min = m_callxConfig->sba_call_duration_average_min_completed;
    int call_counter = 0;
    int duration_cumulative = 0;
    int duration_average = 0;

    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {

        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_BYE) {
            auto event = static_cast<ByeEvent*>((*iter).get());

            // we use the initial BYE request only
            if (event->finalResponseCode == rc_UNDEFINED)
                continue;

            duration_cumulative += event->callDuration.count();
            call_counter++;
        }
    }

    if (call_counter < completed_min)
        return;

    // calculate / check result

    // duration in milliseconds
    duration_average = floor(((duration_cumulative * 1.0 / call_counter) / 1000)
            + 0.5);

    if (duration_average < duration_average_min) {

        stringstream reasonStream;
        reasonStream << duration_average
                << " seconds average call duration in period of "
                << period
                << " minutes";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALL_DURATION_AVERAGE,
                        m_currCaller,
                        period,
                        duration_average,
                        false,
                        reasonStream.str()));
        L_t
                << incidentTypeToString(it_CALL_DURATION_AVERAGE)
                << ": "
                << reasonStream.str();
    }
}

void SigBasedAna::analyzeCallDurationCumulative() {
    int period = m_callxConfig->sba_call_duration_cumulative_period;
    int duration_cumulative_max = m_callxConfig
            ->sba_call_duration_cumulative_max;
    int duration_cumulative = 0;
    int duration_cumulative_minutes = 0;

    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {

        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_BYE) {
            auto event = static_cast<ByeEvent*>((*iter).get());

            if (event->finalResponseCode != rc_UNDEFINED) {
                continue;
            }
            duration_cumulative += event->callDuration.count();
        }
    }

    // calculate / check result

    // milliseconds ---> minutes
    duration_cumulative_minutes = floor(duration_cumulative * 1.0 / 1000 / 60);

    if (duration_cumulative_minutes > duration_cumulative_max) {
        stringstream reasonStream;
        reasonStream << duration_cumulative_minutes
                << " minutes cumulative duration in period of "
                << period
                << " minutes";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALL_DURATION_CUMULATIVE,
                        m_currCaller,
                        period,
                        duration_cumulative_minutes,
                        true,
                        reasonStream.str()));
        L_t
                << incidentTypeToString(it_CALL_DURATION_CUMULATIVE)
                << ": "
                << reasonStream.str();
    }
}

void SigBasedAna::analyzeCallsClosedByCallee() {
    int period = m_callxConfig->sba_calls_closed_by_callee_period;
    int closed_by_callee_max = m_callxConfig->sba_calls_closed_by_callee_max;
    int min_completed = m_callxConfig->sba_calls_closed_by_callee_min_completed;
    int call_counter = 0;
    int closed_by_callee = 0;
    float callee_closed_ratio = 0;

    for (auto iter = m_currSbaEventDeque->begin();
            iter != m_currSbaEventDeque->end(); iter++) {

        if (!isInPeriod((*iter)->timestamp, period))
            continue;

        if ((*iter)->eventType == et_BYE) {
            auto event = static_cast<ByeEvent*>((*iter).get());

            // initial requests only
            if (event->finalResponseCode != rc_UNDEFINED)
                continue;

            // closed by callee
            if (event->caller.tag != event->transactionInitiator.tag) {
                closed_by_callee++;
            }
            call_counter++;
        }
    }

    // calculate / check result

    // To few calls? Explicit zero check because possibility of
    // dividing by zero in case of configuration error.
    if (call_counter == 0 || call_counter < min_completed)
        return;

    callee_closed_ratio = floor((closed_by_callee * 1.0 / call_counter * 100)
            + 0.5);

    if (call_counter >= min_completed
            && callee_closed_ratio > closed_by_callee_max) {
        stringstream reasonStream;
        reasonStream << callee_closed_ratio
                << "% "
                << "of the calls are closed by the callee";

        insertIncident(m_currCaller,
                make_shared<SbaIncident>(it_CALLS_CLOSED_BY_CALLEE,
                        m_currCaller,
                        period,
                        callee_closed_ratio,
                        true,
                        reasonStream.str()));
        L_t
                << incidentTypeToString(it_CALLS_CLOSED_BY_CALLEE)
                << ": "
                << reasonStream.str();
    }
}

bool SigBasedAna::isInPeriod(systemClock::time_point eventTs, int period) {
    return (boost::chrono::duration_cast<minutes>(m_analyzeTs - eventTs).count()
            < period
            && boost::chrono::duration_cast<seconds>(m_analyzeTs - eventTs)
                    .count() > 0);
}

void SigBasedAna::insertIncident(const std::string& caller,
        std::shared_ptr<SbaIncident> incident) {
    shared_ptr<SbaIncidentSet> incidentSet;
    if (!m_sbaIncidentMap->find(caller, incidentSet)) {
        incidentSet = make_shared<SbaIncidentSet>();
        m_sbaIncidentMap->insert(make_pair(caller, incidentSet));
    }
    incidentSet->insert(incident);
}

} /* namespace callx */

