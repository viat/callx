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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * CallMap.cpp
 *
 *  Created on: Jan 16, 2013
 *      Author: mainka
 */

#include "CallMap.hpp"
#include "CallDecodeQueue.hpp"
#include "config/CallxConfig.hpp"
#include <deque>

using namespace std;

namespace callx {

CallMap::CallMap()
        : m_callxConfig(CallxConfig::getInstance()),
          m_callDecodeQueue(CallDecodeQueue::getInstance()) {
    L_t
    << "C'tor";
}

CallMap::~CallMap() {
    L_t
    << "D'tor";
}

string CallMap::toString() const {
    stringstream strstream;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto iter = m_map.begin(); iter != m_map.end(); iter++) {
        strstream << "\r\n"
                  << "Call ID: "
                  << iter->second->getDialog()->callId
                  << "\r\n"
                  << "From: "
                  << iter->second->getDialog()->caller.address
                  << "\r\n"
                  << "To: "
                  << iter->second->getDialog()->callee.address
                  << "\r\n\r\n";
    }
    if (strstream.tellp() == 0) {
        strstream << "\r\n" << "CallMap is empty.\r\n\r\n";
    }
    return strstream.str();
}

void CallMap::handleCallTimeouts() {

// Temporary queue of calls that will be moved into CallDecodeQueue.
    std::deque<std::shared_ptr<Call> > tempCallQueue;

//    auto max_age = boost::chrono::seconds(m_callxConfig->max_call_age);
//    auto max_rtp_inactivity = boost::chrono::seconds(
//            m_callxConfig->max_call_rtp_inactivity);

// making a snapshot of the map
    std::unique_lock<std::mutex> callMapLock(m_mutex);
    map<string, shared_ptr<Call>> mapSnapShot(m_map);
    callMapLock.unlock();

    for (auto callIter = mapSnapShot.begin(); callIter != mapSnapShot.end();
            callIter++) {

        auto now = boost::chrono::steady_clock::now();
        auto currCall = callIter->second;
        auto callLock = currCall->getUniqueLock();

        //////////////////////////////////////////
        // START handle max_call_recording_time //
        //--------------------------------------//

        // Deactivate RtpSink objects, if max recording time is exceeded.
        currCall->checkRecordingTime(m_callxConfig->max_call_recording_time);

        //--------------------------------------//
        // START handle max_call_recording_time //
        //////////////////////////////////////////

        //////////////////////////////////////////
        // START handle max_call_rtp_inactivity //
        //--------------------------------------//
        // Push Call into CallDecodeQueue in case of RTP inactivity.
        int rtpInactivity = currCall->getRtpInactivityTime();
//        L_t
//        << "RTP inactivity: " << rtpInactivity;
        if (rtpInactivity > m_callxConfig->max_call_rtp_inactivity) {
            L_t
            << "RTP inactivity of " << rtpInactivity << " seconds exceeds the limit.";
            // max_call_rtp_inactivity exceeded!
            // This call will be deleted from global CallMap and pushed
            // to the CallDecodeQueue.
            currCall->unregisterRtpSinks();
            tempCallQueue.push_back(currCall);
        }
        //--------------------------------------//
        // END handle max_call_rtp_inactivity   //
        //////////////////////////////////////////

        //////////////////////////////////////////
        // START handle max_call_age            //
        //--------------------------------------//

        // Push Call into CallDecodeQueue if it exceeds max_call_age.
        int callAge = (boost::chrono::duration_cast<seconds>(
                now - currCall->getCreationTsSteadyClock()).count());

//        L_t
//        << "Call age: " << callAge;
        if (callAge > m_callxConfig->max_call_age) {
            L_t
            << "Call to old: " << callAge;

            // max_call_age exceeded!
            // This call will be deleted from global CallMap and pushed
            // to the CallDecodeQueue.
            currCall->unregisterRtpSinks();
            tempCallQueue.push_back(currCall);
        }
        //--------------------------------------//
        // END handle max_call_age              //
        //////////////////////////////////////////

        callLock->unlock();
    }

// All Call object references of the tempCallQueue will be deleted
// from global CallMap and pushed to the CallDecodeQueue now.

    callMapLock.lock(); // lock the lock again because we want to change m_map;
    for (auto callIter = tempCallQueue.begin(); callIter != tempCallQueue.end();
            callIter++) {
        if (m_map.erase((*callIter)->getDialog()->callId)) {
            L_t
            << "Erased one reference from CallMap.";
            m_callDecodeQueue->push(move(*callIter));
            L_t
            << "Pushed one reference to the CallDecodeQueue.";
        }
    }
}

} /* namespace callx */
