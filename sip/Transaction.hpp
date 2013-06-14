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
 * Transaction.hpp
 *
 *  Created on: Dec 3, 2012
 *      Author: mainka
 */

#ifndef TRANSACTION_HPP_
#define TRANSACTION_HPP_

#include "sip/SipPacket.hpp"
#include "Call.hpp"
#include "container/SbaEventMap.hpp"

namespace callx {

class Transaction {
public:

    Transaction(std::weak_ptr<Call> call,
            const std::unique_ptr<SipPacket>& sipPacket)
            : m_creationTs(systemClock::now()),
              m_activityTs(steadyClock::now()),
              m_call(call),
              m_initiator(sipPacket->getFrom()),
              m_sentBy(sipPacket->getSentBy()),
              m_cseqNum(sipPacket->getCseqNum()),
              m_id( { sipPacket->getBranch(), sipPacket->getCseqMethod() }),
              m_state( { ts_TERMINATED, ts_TERMINATED }),
              m_finalResponseCode(0),
              m_cancelFlag(false),
              m_ackFlag(false) {

        L_t
        << "C'tor";
        m_sbaEventMap = SbaEventMap::getInstance();
    }

    virtual ~Transaction() {
        L_t
        << "D'tor";
    }

    const systemClock::time_point& getCreationClockstamp() const {
        return m_creationTs;
    }

    const steadyClock::time_point& getActivityClockstamp() const {
        return m_activityTs;
    }

    const SipFromTo& getInitiator() {
        return m_initiator;
    }

    const std::string& getSentBy() const {
        return m_sentBy;
    }

    u_int getCseqNum() const {
        return m_cseqNum;
    }

    const TransactionId& getId() const {
        return m_id;
    }

    const TransactionState& getState() const {
        return m_state;
    }

    requestMethodEnum getCseqMethod() const {
        return m_id.second;
    }

    void updateActivityClockstamp() {
        m_activityTs = steadyClock::now();
    }

    void setCseqNum(u_int cseqNum) {
        m_cseqNum = cseqNum;
    }

    void setState(TransactionState states) {
        L_t
        << "Changing state to (client / server): " << transactionState[states.first] << " / " << transactionState[states
                .second];
        m_state = states;
    }

    void setFinalResponseCode(u_int responseCode) {
        m_finalResponseCode = responseCode;
    }

    void setFinalResponseReason(const std::string& responseReason) {
        m_finalResponseReason = responseReason;
    }

    void setCancelFlag() {
        m_cancelFlag = true;
    }

    void setAckFlag() {
        m_ackFlag = true;
    }

    void createAndPushEvent() {

        // Making a temp. shared_ptr out of the weak_ptr pointing the
        // Call object.
        if (std::shared_ptr<Call> currCall = m_call.lock()) {
            std::shared_ptr<Dialog> currDialog = currCall->getDialog();

            switch (m_id.second) {
            case rm_INVITE: {
                InviteEvent* event = new InviteEvent(currDialog->callId,
                        currDialog->caller, m_initiator);
                event->finalResponseCode = m_finalResponseCode;
                event->finalResponseReason = m_finalResponseReason;
                event->reinviteFlag = currCall->getReinviteFlag();
                event->ackFlag = m_ackFlag;
                event->cancelFlag = m_cancelFlag;
                pushEvent(currDialog->caller.address, event);
            }
            break;
            case rm_BYE: {
                ByeEvent* event = new ByeEvent(currDialog->callId,
                        currDialog->caller, m_initiator);
                event->finalResponseCode = m_finalResponseCode;
                event->finalResponseReason = m_finalResponseReason;
                event->callDuration =
                        boost::chrono::duration_cast<milliseconds>(
                                systemClock::now() - currCall->getCreationTs());
                pushEvent(currDialog->caller.address, event);
            }
            break;
            case rm_CANCEL: {
                CancelEvent* event = new CancelEvent(currDialog->callId,
                        currDialog->caller, m_initiator);
                event->finalResponseCode = m_finalResponseCode;
                event->finalResponseReason = m_finalResponseReason;
                pushEvent(currDialog->caller.address, event);
            }
            break;
            case rm_OPTIONS: {
                OptionsEvent* event = new OptionsEvent(currDialog->callId,
                        currDialog->caller, m_initiator);
                event->finalResponseCode = m_finalResponseCode;
                event->finalResponseReason = m_finalResponseReason;
                pushEvent(currDialog->caller.address, event);
            }
            break;
            default:
                // We should never be here...
                L_e
                << "Transaction::pushEvent(): Unhandled type of transaction";
                return;
            break;
            }
        }
    }

protected:

    /**
     * Adds an SBA event to the event list of the specific caller. It
     * creates the map entry and the list, if not existent.
     * @param caller
     * @param event
     */
    void pushEvent(const std::string &caller, SbaEvent *event) {
        std::shared_ptr<SbaEventDeque> eventDeque;
        if (!m_sbaEventMap->find(caller, eventDeque)) {
            eventDeque = std::make_shared<SbaEventDeque>();
            m_sbaEventMap->insert(std::make_pair(caller, eventDeque));
        }
        eventDeque->push_back(std::shared_ptr<SbaEvent>(event));
    }

    SbaEventMap *m_sbaEventMap;

    systemClock::time_point m_creationTs; // creation Timestamp
    steadyClock::time_point m_activityTs; // activity Timestamp

    std::weak_ptr<Call> m_call;
    SipFromTo m_initiator;
    std::string m_sentBy;
    u_int m_cseqNum;
    TransactionId m_id;
    TransactionState m_state;
    u_int m_finalResponseCode;
    std::string m_finalResponseReason;
    bool m_cancelFlag;  // relevant for INVITE transaction
    bool m_ackFlag;     // relevant for INVITE transaction
};

} /* namespace callx */
#endif /* TRANSACTION_HPP_ */
