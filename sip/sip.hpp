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
 * sip.hpp
 *
 *  Created on: Aug 15, 2012
 *      Author: mainka
 */

#ifndef SIP_HPP_
#define SIP_HPP_

#include "sba/SbaEvent.hpp"
#include <sys/types.h>
#include <string>

namespace callx {

// Message Type (mt_)
enum messageTypeEnum {
    mt_REQUEST = 0,
    mt_RESPONSE,
    mt_MALFORMED
};
static const std::string messageType[] = { "REQUEST", "RESPONSE", "MALFORMED" };

// Request Method (rm_)
enum requestMethodEnum {
    rm_INVITE = 0,
    rm_ACK,
    rm_BYE,
    rm_CANCEL,
    rm_REGISTER,
    rm_OPTIONS,
    rm_INFO,
    rm_UNDEFINED
};
static const std::string requestMethod[] = { "INVITE", "ACK", "BYE", "CANCEL",
        "REGISTER", "OPTIONS", "INFO", "UNDEFINED" };

inline std::string requestMethodEnumToString(requestMethodEnum m) {
    return requestMethod[m];
}

// Response Code (rc_)
enum responseCodeEnum {
    rc_UNDEFINED = 0,
    rc_PROVISIONAL = 100,
    rc_SUCCESS = 200,
    rc_REDIRECTION = 300,
    rc_CLIENT_ERROR = 400,
    rc_SERVER_ERROR = 500,
    rc_GLOBAL_FAILURE = 600,
    rc_NONVALID = 900
};
static const std::string responseCode[] = { "PROVISIONAL", "SUCCESS",
        "REDIRECTION", "CLIENT ERROR", "SERVER ERROR", "GLOBAL FAILURE",
        "NONVALID" };

inline std::string responseCodeEnumToString(responseCodeEnum r) {
    return responseCode[r / 100 - 1];
}

// Transaction State (ts_)
enum transactionStateEnum {
    ts_CALLING = 0,
    ts_PROCEEDING,
    ts_TRYING,
    ts_COMPLETED,
    ts_CONFIRMED,
    ts_TERMINATED
};
static const std::string transactionState[] = { "CALLING", "PROCEEDING",
        "TRYING", "COMPLETED", "CONFIRMED", "TERMINATED" };

// client state and server state as a pair
typedef std::pair<transactionStateEnum, transactionStateEnum> TransactionState;

// The Transaction ID consists of the branch string and the cseq method as enum.
typedef std::pair<std::string, requestMethodEnum> TransactionId;

// Dialog State (ds_)
enum dialogStateEnum {
    ds_UNDEFINED = 0,
    ds_INIT,
    ds_EARLY,
    ds_CONFIRMED,
    ds_TERMINATING
};
static const std::string dialogState[] = { "UNDEFINED", "INIT", "EARLY",
        "CONFIRMED", "TERMINATED" };

// Transport Protocol (tp_)
enum transportProtoEnum {
    tp_TCP = 0x6, tp_UDP = 0x11, tp_UNDEFINED = 0xFF
};
//static const std::string transportProto[] = { "UNDEFINED", "UDP", "TCP" };

inline transportProtoEnum transportProtoStringToEnum(std::string proto) {
    if (proto == "UDP") return tp_UDP;
    if (proto == "TCP") return tp_TCP;
    return tp_UNDEFINED;
}

} /* namespace callx */

#endif /* SIP_HPP_ */
