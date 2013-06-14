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
 * Call.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: mainka
 */


#include "Call.hpp"
#include "Transaction.hpp"
#include "container/RtpSinkMap.hpp"
#include "sip.hpp"
#include <utility>

using namespace std;

namespace callx {

Call::Call(string callId)
        : m_creationTs(systemClock::now()),
          m_creationTsSteady(steadyClock::now()),
          m_activityTs(steadyClock::now()),
          m_localRtpSinkMap(make_shared<RtpSinkMapType>()),
          m_rtpSinkMap(RtpSinkMap::getInstance()),
          m_dialog(make_shared<Dialog>(callId)),
          m_reinviteFlag(false),
          m_rtpSinksAreActivated(true) {
    L_t
    << "C'tor";
}

Call::~Call() {
    L_t
    << "D'tor";
    unregisterRtpSinks();
}

unique_ptr<unique_lock> Call::getUniqueLock() const {
    return unique_ptr<unique_lock>(new unique_lock(m_mutex));
}

const systemClock::time_point& Call::getCreationTs() const {
    return m_creationTs;
}

const steadyClock::time_point& Call::getCreationTsSteadyClock() const {
    return m_creationTsSteady;
}

const steadyClock::time_point& Call::getActivityTs() const {
    return m_activityTs;
}

void Call::updateActivityTs() {
    m_activityTs = steadyClock::now();
}

shared_ptr<Dialog> Call::getDialog() {
    return m_dialog;
}

bool Call::findTransaction(const TransactionId& transactionId,
        std::shared_ptr<Transaction>& transaction) {

    auto iter = m_transactionMap.find(transactionId);
    if (iter == m_transactionMap.end())
        return false;
    transaction = iter->second;
    return true;
}

bool Call::findInviteTransaction(std::shared_ptr<Transaction>& transaction) {
    for (auto iter = m_transactionMap.begin(); iter != m_transactionMap.end();
            iter++) {
        if (iter->first.second == rm_INVITE) {
            transaction = iter->second;
            return true;
        }
    }
    return false;
}

void Call::insertTransaction(shared_ptr<Transaction> transaction) {
    m_transactionMap.insert( { transaction->getId(), transaction });
}

size_t Call::delTransaction(const TransactionId& transactionId) {
    return m_transactionMap.erase(transactionId);
}

void Call::checkRecordingTime(int maxSeconds) {
    auto now = steadyClock::now();
    auto diff = boost::chrono::duration_cast<seconds>(now - m_creationTsSteady);

    if (m_rtpSinksAreActivated && diff.count() > maxSeconds) {
        for (auto sinkIter = m_localRtpSinkMap->begin();
                sinkIter != m_localRtpSinkMap->end(); sinkIter++) {
            sinkIter->second->deactivate();
            L_t
            << "RtpSink deactivated after " << diff.count() << " seconds, socket: "
                    << sinkIter->first;
        }
        m_rtpSinksAreActivated = false;
    }
}

int Call::getRtpInactivityTime() {

    // max duration of inactivity is <now() - time_of_call_creation>
    int inactiveSeconds = (boost::chrono::duration_cast<seconds>(
            steadyClock::now() - m_creationTsSteady)).count();

    // iterate over RtpSink objects to compare inactivity times
    for (auto sinkIter = m_localRtpSinkMap->begin();
            sinkIter != m_localRtpSinkMap->end(); sinkIter++) {
        int sinkInactivity = sinkIter->second->inactiveFor();
        if (sinkInactivity < inactiveSeconds)
            inactiveSeconds = sinkInactivity;
    }
    return inactiveSeconds;
}

shared_ptr<RtpSinkMapType> Call::getLocalRtpSinkMap() {
    return m_localRtpSinkMap;
}

bool Call::getReinviteFlag() const {
    return m_reinviteFlag;
}

void Call::setReinviteFlag() {
    m_reinviteFlag = true;
}

void Call::unsetReinviteFlag() {
    m_reinviteFlag = false;
}

void Call::unregisterRtpSinks() const {
    int count = 0;
    L_t
    << "Deleting RtpSink references from global RtpSinkMap.";

    // iterate over all RtpSink references in the local map
    for (auto iter = m_localRtpSinkMap->begin();
            iter != m_localRtpSinkMap->end(); iter++) {

        // delete RtpSink from global RtpSinkMap
        count += m_rtpSinkMap->erase(iter->first);
    }
    L_t
    << "Deleted " << count << " RtpSink references.";
}

bool Call::rtpSinksAreActivated() const {
    return m_rtpSinksAreActivated;
}

void Call::setFlagNoRtp() {
    m_rtpSinksAreActivated = false;
}

std::shared_ptr<RtpSinkMapType> Call::getRtpSinkMap() {
    return m_localRtpSinkMap;
}

} /* namespace callx */
