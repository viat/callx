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
 * SipProcessor.hpp
 *
 *  Created on: Oct 28, 2012
 *      Author: mainka
 */

#ifndef SIPPROCESSOR_HPP_
#define SIPPROCESSOR_HPP_

#include "sip.hpp"
#include "main/CallxThread.hpp"
#include "main/CallxTypes.hpp"

namespace callx {

class Transaction;
class CallMap;
class RtpSinkMap;
class Call;
class Dialog;
class SipPacket;
class SipPacketQueue;
class CallDecodeQueue;
class SbaIncidentMap;

class SipProcessor:
        public CallxThread {
public:
    SipProcessor();
    virtual ~SipProcessor();

    /**
     * Overrides method CallxThread::stop() because SipPacketQueue has
     * to be disabled before stooping.
     */
    bool stop();

    /**
     * Implementation of abstract method CallxThread::worker()
     */
    void worker();

protected:

    void handleInitialRequest();
    void handleRequestWithinTransaction();
    void handleResponse();

    void handleInvite();
    void handleBye();
    void handleOptions();
    void handleCancel();
    void handleAckOnSuccess();
    void handleAckOnNonSuccess();

    void handleProvisional();
    void handleProvisionalOnInvite();
    void handleProvisionalOnNonInvite();

    void handleSuccess();
    void handleNonSuccess();

    void handleSuccessOnInvite();
    void handleNonSuccessOnInvite();
    void handleResponseOnBye();
    void handleResponseOnOptions();
    void handleResponseOnCancel();

    /**
     * Creates SDP sinks and inserts them into local (Call) and global
     * RtpSinkMap.
     */
    void processSdp();

    /**
     * Deletes all RtpSink from local RtpSink map with status CONFIRMED.
     * Changes the status of RtpSinks with status INIT or CONFIRMED_INIT to
     * CONFIRMED.
     */
    void confirmRtpSinks();

    /**
     * Deletes RtpSink objects with status INIT.
     * Changes the status of RtpSinks with status CONFIRMED_INIT to CONFIRMED.
     */
    void annulRtpSinks();

    /**
     * Compares every TransactionState in the initializer list with the
     * current state.
     * @param states Vector of TransactionStates (pairs of client and server
     * states)
     * @return true if one of the TransactionStates matches the current
     * transaction state
     */
    bool matchState(std::initializer_list<TransactionState> states);

    /**
     * Deletes a Call reference from the CallMap. The Call object will be
     * destroyed if no other reference exists.
     */
    void deleteCallFromCallMap(const std::string& callId);

    /**
     * Sets the Call decode flag and pushes the reference into CallDecodeMap.
     */
    void prepareCallToDecode(const std::string& callId);

    /**
     * Test if there is an incident originated by this caller.
     * @param caller address, eg. sip:blah@blubb
     * @return true if there is at least one incident
     */
    bool callerHasIncident(const std::string& caller);


    SipPacketQueue *m_sipPacketQueue;
    CallMap *m_callMap;
    RtpSinkMap *m_rtpSinkMap;
    CallDecodeQueue *m_callDecodeQueue;
    SbaIncidentMap *m_sbaIncidentMap;

    std::unique_ptr<SipPacket> m_currPacket;
    std::shared_ptr<Call> m_currCall;
    std::unique_ptr<unique_lock> m_currCallLock;
    std::shared_ptr<Transaction> m_currTransaction;
    std::shared_ptr<Dialog> m_currDialog;
    std::shared_ptr<RtpSinkMapType> m_currCallRtpSinkMap;
};

} /* namespace callx */
#endif /* SIPPROCESSOR_HPP_ */
