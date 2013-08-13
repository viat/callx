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
 * SbaEvent.hpp
 *
 *  Created on: Nov 29, 2012
 *      Author: mainka
 */

#ifndef SBAEVENT_HPP_
#define SBAEVENT_HPP_

#include "main/callx.hpp"
#include "sip/sip.hpp"

namespace callx {

/**
 * Event types as enum.
 */
typedef enum SbaEventTypeEnumType {
    et_GENERIC,
    et_INVITE,
    et_BYE,
    et_CANCEL,
    et_OPTIONS
} SbaEventTypeEnum;

static const std::string SbaEventType[] = { "GENERIC", "INVITE", "BYE",
        "CANCEL", "OPTIONS" };

inline static std::string sbaEventTypeEnumToString(SbaEventTypeEnum e) {
    return SbaEventType[e];
}

/**
 * Generic SBA Event.
 */
struct SbaEvent {
    SbaEvent(const std::string &callId,
            const SipFromTo &caller,
            const SipFromTo &transactionInitiator)
            : timestamp(systemClock::now()),
              callId(callId),
              caller(caller),
              transactionInitiator(transactionInitiator),
              finalResponseCode(0),
              eventType(et_GENERIC) {
    }
    systemClock::time_point timestamp;
    std::string callId;
    SipFromTo caller;
    SipFromTo transactionInitiator;
    u_int finalResponseCode;
    std::string finalResponseReason;
    SbaEventTypeEnum eventType;

    virtual ~SbaEvent() {
    }
};

/**
 * SBA event specific to INVITE transaction
 */
struct InviteEvent:
        public SbaEvent {
    InviteEvent(const std::string &callId,
            const SipFromTo &caller,
            const SipFromTo &transactionInitiator)
            : SbaEvent(callId, caller, transactionInitiator),
              reinviteFlag(false),
              cancelFlag(false),
              ackFlag(false) {
        L_t
        << "C'tor InviteEvent";
        eventType = et_INVITE;
    }
    bool reinviteFlag;
    bool cancelFlag;
    bool ackFlag;

    ~InviteEvent() {
        L_t
        << "D'tor InviteEvent";
    }
};

/**
 * SBA event specific to BYE transaction
 */
struct ByeEvent:
        public SbaEvent {
    ByeEvent(const std::string &callId,
            const SipFromTo &caller,
            const SipFromTo &transactionInitiator)
            : SbaEvent(callId, caller, transactionInitiator) {
        L_t
        << "C'tor ByeEvent";
        eventType = et_BYE;
    }

    virtual ~ByeEvent() {
        L_t
        << "D'tor ByeEvent";
    }

    milliseconds callDuration;
};

/**
 * SBA event specific to CANCEL transaction
 */
struct CancelEvent:
        public SbaEvent {
    CancelEvent(const std::string &callId,
            const SipFromTo &caller,
            const SipFromTo &transactionInitiator)
            : SbaEvent(callId, caller, transactionInitiator) {
        L_t
        << "C'tor CancelEvent";
        eventType = et_CANCEL;
    }

    virtual ~CancelEvent() {
        L_t
        << "D'tor CancelEvent";
    }
};

/**
 * SBA event specific to OPTIONS transaction
 */
struct OptionsEvent:
        public SbaEvent {
    OptionsEvent(const std::string &callId,
            const SipFromTo &caller,
            const SipFromTo &transactionInitiator)
            : SbaEvent(callId, caller, transactionInitiator),
              dialogEstablished(false) {
        L_t
        << "C'tor OptionsEvent";
        eventType = et_OPTIONS;
    }

    virtual ~OptionsEvent() {
        L_t
        << "D'tor OptionsEvent";
    }

    bool dialogEstablished;
};

} /* namespace callx */
#endif /* SBAEVENT_HPP_ */
