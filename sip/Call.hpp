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
 * Call.hpp
 *
 *  Created on: Oct 29, 2012
 *      Author: mainka
 */

#ifndef CALL_HPP_
#define CALL_HPP_

#include <map>
#include "main/CallxTypes.hpp"
#include "sip.hpp"

namespace callx {

class Transaction;
class RtpSink;
class RtpSinkMap;

typedef std::map<TransactionId, std::shared_ptr<Transaction>> TransactionMap;

struct Dialog {

    Dialog(std::string callId)
            : state(ds_UNDEFINED),
              callId(callId) {
    }

    dialogStateEnum state;
    std::string callId;

    SipFromTo caller;
    SipFromTo callee;
};

class Call {

public:
    Call(std::string callId);
    virtual ~Call();

    /**
     * Locks the Call object. The lock will be released if the returned
     * unique pointer gets destroyed.
     * @return Unique pointer to the unique lock.
     */
    std::unique_ptr<unique_lock> getUniqueLock() const;

    const systemClock::time_point& getCreationTs() const;
    const steadyClock::time_point& getCreationTsSteadyClock() const;
    const steadyClock::time_point& getActivityTs() const;
    void updateActivityTs();

    std::shared_ptr<Dialog> getDialog();

    bool findTransaction(const TransactionId&, std::shared_ptr<Transaction>&);
    bool findInviteTransaction(std::shared_ptr<Transaction>&);
    void insertTransaction(std::shared_ptr<Transaction>);
    size_t delTransaction(const TransactionId&);

    /**
     * Deactivates all RtpSink objects, if the call is older than <seconds>.
     * @param seconds
     */
    void checkRecordingTime(int seconds);

    /**
     * Returns the RTP inactivity time in seconds.
     * @return Number of seconds of RTP inactivity. This is the number of
     * seconds since call creation, if no RtpSink is available.
     */
    int getRtpInactivityTime();

    /**
     * Returns the local RtpSinkMap as a shared pointer.
     * @return
     */
    std::shared_ptr<RtpSinkMapType> getLocalRtpSinkMap();

    bool getReinviteFlag() const;
    void setReinviteFlag();
    void unsetReinviteFlag();

    void unregisterRtpSinks() const;
    bool rtpSinksAreActivated() const;
    void setFlagNoRtp();

    std::shared_ptr<RtpSinkMapType> getRtpSinkMap();

protected:
    // creation Timestamp (system clock)
    systemClock::time_point m_creationTs;

    // creation Timestamp (steady clock)
    steadyClock::time_point m_creationTsSteady;

    // activity Timestamp (steady clock)
    steadyClock::time_point m_activityTs;

    TransactionMap m_transactionMap;
    std::shared_ptr<RtpSinkMapType> m_localRtpSinkMap;
    RtpSinkMap *m_rtpSinkMap;
    std::shared_ptr<Dialog> m_dialog;
    bool m_reinviteFlag;
    bool m_rtpSinksAreActivated;
    mutable mutex m_mutex;

};

} /* namespace callx */
#endif /* CALL_HPP_ */
