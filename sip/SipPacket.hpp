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
 * SipPacket.hpp
 *
 *  Created on: Aug 13, 2012
 *      Author: mainka
 */

#ifndef SIPPACKET_HPP_
#define SIPPACKET_HPP_

#include <string>
#include <iostream>
#include "sip.hpp"
#include "network/SocketAddress.hpp"
#include "network/TlayerPacketRecycler.hpp"
#include "network/TlayerPacket.hpp"

namespace callx {

// pair of <field> and <value>
typedef std::multimap<std::string, std::string> fieldMapType;

class SipPacket {
public:

    /**
     * Standard constructor
     */
    SipPacket(std::unique_ptr<TlayerPacket, TlayerPacketRecycler>);

    /**
     * Destructor
     */
    virtual ~SipPacket();

    bool parse();
    bool hasSdpPayload();

    const std::string& getCallId() const;
    const std::string& getRequestUri() const;
    const std::string& getFromString() const;
    const std::string& getToString() const;
    const SipFromTo& getFrom() const;
    const SipFromTo& getTo() const;
    const std::string& getBranch() const;
    const std::string& getSentBy() const;
    const transportProtoEnum getSentByProtocol() const;
    requestMethodEnum getCseqMethod() const;
    u_int getCseqNum() const;
    messageTypeEnum getMessageType() const;
    requestMethodEnum getRequestMethod() const;
    responseCodeEnum getResponseCategory() const;
    u_int getResponseCode() const;
    const std::string& getResponseReason() const;
    const SocketAddress& getSdpMediaSocket() const;

private:

    bool parseSip();
    bool parseSdp();

    requestMethodEnum requestToEnum(const std::string& request) const;

    // Layer 4 packet
    std::unique_ptr<TlayerPacket, TlayerPacketRecycler> m_tlayerPacket;

    // This string takes the raw SIP data.
    std::string m_sipRawString;

    // SIP start line
    std::string m_startLine;

    // SIP attribute value pairs
    fieldMapType m_sipFieldMap;

    // SIP message type (request / response)
    messageTypeEnum m_messageType;

    // SIP request method
    requestMethodEnum m_requestMethod;

    // SIP request URI
    std::string m_requestUri;

    // SIP response status code
    u_int m_responseStatusCode;

    // SIP response reason phrase
    std::string m_responseReason;

    struct HasFields {
        bool to;
        bool from;
        bool cSeq;
        bool callId;
        bool maxForwards;
        bool via;
        HasFields()
                : to(false),
                  from(false),
                  cSeq(false),
                  callId(false),
                  maxForwards(false),
                  via(false) {
        }
    } m_hasFields;

    std::string m_callId;
    std::string m_fromString;
    std::string m_toString;
    SipFromTo m_from;
    SipFromTo m_to;
    requestMethodEnum m_cseqMethod;
    u_int m_seqNum;
    std::string m_branch;
    std::string m_sentBy;
    transportProtoEnum m_sentByProtocol;

    // SDP payload available?
    bool m_hasSdpPayload;

    // SDP attribute value pairs
    fieldMapType m_sdpFieldMap;

    // SDP Socket Address
    SocketAddress m_sdpSocketAddress;
};

} /* namespace callx */

#endif /* SIPPACKET_HPP_ */
