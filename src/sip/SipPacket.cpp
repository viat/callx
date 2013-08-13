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
 * SipPacket.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: mainka
 */

#include "SipPacket.hpp"
#include "network/TlayerPacket.hpp"
#include "main/callx.hpp"
#include <boost/regex.hpp>

using namespace std;

namespace callx {

SipPacket::SipPacket(unique_ptr<TlayerPacket, TlayerPacketRecycler> tlayerPacket)
        : m_tlayerPacket(move(tlayerPacket)),
          m_messageType(mt_MALFORMED),
          m_requestMethod(rm_UNDEFINED),
          m_responseStatusCode(0),
          m_cseqMethod(rm_UNDEFINED),
          m_seqNum(0),
          m_sentByProtocol(tp_UNDEFINED),
          m_hasSdpPayload(false) {

}

SipPacket::~SipPacket() {
}

bool SipPacket::parse() {

    m_sipRawString.assign(
            reinterpret_cast<char*>(m_tlayerPacket->m_udpPacket.payload),
            m_tlayerPacket->m_udpPacket.payloadLen);

    if (parseSip()) {

        // SIP parsing OK, SDP available?
        if (m_hasSdpPayload) {

            // returns false if SDP payload parsing fails
            return parseSdp();
        }

        // everything OK
        return true;
    }

    // SIP parsing failed
    return false;
}

bool SipPacket::parseSip() {

    string line; // one line of our SIP packet
    boost::cmatch matches; // regex_match() result

    // end of line is CRLF
    const boost::regex sipLineDelim("\r\n");

    // Method SP Request-URI SP SIP-Version
    const boost::regex sipRequest("(^\\w+?) (.*) SIP/2\\.0$",
            boost::regex_constants::icase);

    // SIP-Version SP Status-Code SP Reason-Phrase
    const boost::regex sipResponse("^SIP/2\\.0 (\\d{3}) (.+)",
            boost::regex_constants::icase);

    // field-name:field-value
    const boost::regex sipFieldAndValue("^(.+?)\\s*:\\s*(.+)");

    // field-name=field-value
    const boost::regex sdpFieldAndValue("^(.)\\s*=\\s*(.+)");

    // line iterator
    boost::sregex_token_iterator lineIter(m_sipRawString.begin(),
            m_sipRawString.end(), sipLineDelim, -1);
    boost::sregex_token_iterator nullIter;

    // no data
    if (lineIter == nullIter)
        return false;

    // handle startline
    m_startLine = *lineIter;

    L_i
    << m_startLine;

    // test for SIP request
    if (boost::regex_match(m_startLine.c_str(), matches, sipRequest)) {
        if (matches.size() == 3) {
            m_messageType = mt_REQUEST;
            string requestMethod(matches[1].first, matches[1].second);
            m_requestUri.assign(matches[2].first, matches[2].second);

            m_requestMethod = requestToEnum(requestMethod);

            L_t
            << "Request, method: " << requestMethodEnumToString(
                    m_requestMethod);
        }
    }

    // test for SIP response
    else if (boost::regex_match(m_startLine.c_str(), matches, sipResponse)) {
        if (matches.size() == 3) {
            m_messageType = mt_RESPONSE;
            string tempStr(matches[1].first, matches[1].second);

            // input is regex match \d{3}
            m_responseStatusCode = strtol(tempStr.c_str(), NULL, 10);
            m_responseReason.assign(matches[2].first, matches[2].second);

            L_t
            << "Response, status: " << m_responseStatusCode << " " << m_responseReason;
        }
    }

    // handle next lines

    // This regex changes later to &sdpFieldAndValue (if SDP existent)
    const boost::regex *fieldAndValue = &sipFieldAndValue;

    // later &sdpFieldMap if SDP existent
    fieldMapType *fieldMap = &m_sipFieldMap;

    while (++lineIter != nullIter) {
        line = *lineIter;

        // SIP->SDP context switch:
        // SIP body (SDP) follows after empty line (RFC 3261)
        if (line.empty()) {

            // switching context from SIP to SDP and continue loop
            fieldAndValue = &sdpFieldAndValue;
            fieldMap = &m_sdpFieldMap;
            continue;
        }

        // do the regex
        if (regex_match(line.c_str(), matches, *fieldAndValue)) {
            if (matches.size() == 3) {
                string matchFieldName(matches[1].first, matches[1].second);
                string matchFieldValue(matches[2].first, matches[2].second);

                // transforming to upper-case if we are in SIP *header*
                // not if we are in SIP body, SDP is case-significant
                if (fieldMap == &m_sipFieldMap) {
                    transform(matchFieldName.begin(), matchFieldName.end(),
                            matchFieldName.begin(), ptr_fun(::toupper));
                }

                // inserting in SIP or SDP fieldmap
                fieldMap->insert(
                        pair<string, string>(matchFieldName, matchFieldValue));
            }
        }
    }

    /*
     * Get the parts of fields "From" and "To": displayname, address, tag
     * Some parts may not be present.
     */

    // find iterator
    fieldMapType::const_iterator findIter;

    //*******************************
    // regex for e.g.: "PhonerLite" <sip:tel_20@192.168.11.107>;tag=as008e599f
    // (([ display-name ] LAQUOT SIP-URI RAQUOT) / SIP-URI )*( SEMI "tag" EQUAL token )
    // RFC3261 par. 25 for details
    boost::regex fromAndToRegex(
            "(\\\"?(.+?)\\\"?)?\\s*(<(.+?)(;.*)?>)\\s*(;tag=(.*))?" /*, boost::regex_constants::icase*/);
    //*******************************

    // SIP From
    findIter = m_sipFieldMap.find("FROM");
    if (findIter != m_sipFieldMap.end()) {
        m_hasFields.from = true;
        m_fromString = findIter->second;
        L_i
        << "From: " << m_fromString;

        // extract display name, from address, from tag
        boost::regex_match(findIter->second.c_str(), matches, fromAndToRegex);

        // match[1] is display name with possible double quotes
        m_from.displayname.assign(matches[2].first, matches[2].second);

        // match[3] is source address with left and right angle brackets
        m_from.address.assign(matches[4].first, matches[4].second);

        m_from.tag.assign(matches[7].first, matches[7].second);

    }

    // SIP To
    findIter = m_sipFieldMap.find("TO");
    if (findIter != m_sipFieldMap.end()) {
        m_hasFields.to = true;
        m_toString = findIter->second;
        L_i
        << "To: " << m_toString;
        // extract display name, to address, to tag
        boost::regex_match(findIter->second.c_str(), matches, fromAndToRegex);

        // match[1] is display name with possible double quotes
        m_to.displayname.assign(matches[2].first, matches[2].second);

        // match[3] is destination address with left and right angle brackets
        m_to.address.assign(matches[4].first, matches[4].second);

        m_to.tag.assign(matches[7].first, matches[7].second);
    }

    // SIP Call ID
    findIter = m_sipFieldMap.find("CALL-ID");
    if (findIter != m_sipFieldMap.end()) {
        m_hasFields.callId = true;
        m_callId = findIter->second;
        L_t
        << "Call ID: " << m_callId;
    }

    // SIP CSeq Number & Method
    findIter = m_sipFieldMap.find("CSEQ");
    if (findIter != m_sipFieldMap.end()) {
        m_hasFields.cSeq = true;

        // CSeq is: <32bit-number> <space> <method>
        boost::regex cSeqRegex("(\\d+)\\s+(\\w+)");
        if (regex_match(findIter->second.c_str(), matches, cSeqRegex)) {

            string number(matches[1].first, matches[1].second);
            m_seqNum = strtoul(number.c_str(), NULL, 10);

            string method(matches[2].first, matches[2].second);
            m_cseqMethod = requestToEnum(method);

            L_t
            << "CSeq fields: " << m_seqNum << " " << requestMethodEnumToString(
                    m_cseqMethod);
        }
    }

    // SIP branch, sent-by
    findIter = m_sipFieldMap.lower_bound("VIA");
    if (findIter != m_sipFieldMap.end()) {

        // This regex supports comma separated VIA lists
        boost::regex branchRegex(
                "^SIP/2\\.0/(UDP|TCP)\\s(.*?);branch=([^,]+?)((;|,).*)?|$");
        if (regex_match(findIter->second.c_str(), matches, branchRegex)) {

            // transport protocol
            string protocol(matches[1].first, matches[1].second);
            m_sentByProtocol = transportProtoStringToEnum(protocol);

            // sent-by
            m_sentBy.assign(matches[2].first, matches[2].second);

            // branch
            m_branch.assign(matches[3].first, matches[3].second);

            L_t
            << "Branch: " << m_branch;
        }
    }

    // SIP Max-Fowards
    findIter = m_sipFieldMap.find("MAX-FORWARDS");
    if (findIter != m_sipFieldMap.end()) {
        m_hasFields.maxForwards = true;
    }

    // Test if SDP payload available
    m_hasSdpPayload = false;
    findIter = m_sipFieldMap.find("CONTENT-TYPE");
    if (findIter != m_sipFieldMap.end()
            && findIter->second == "application/sdp") {
        m_hasSdpPayload = true;
    }

    return true;
}

bool SipPacket::parseSdp() {
    boost::cmatch matches;

    // regex that matches port and payloadtypes in e.g.:
    // audio 12692 RTP/AVP 8 3 0 112 5 10 7 110 111 101
    boost::regex portRegex("audio (\\d{4,5}) RTP/AVP( \\d{1,3})+");

    // parsing SDP
    for (fieldMapType::const_iterator sdpIter = m_sdpFieldMap.begin();
            sdpIter != m_sdpFieldMap.end(); sdpIter++) {

        // get IP
        if (sdpIter->first == "c" && sdpIter->second.find("IN IP") == 0) {
            m_sdpSocketAddress.setIpAddress(
                    boost::asio::ip::address::from_string(
                            sdpIter->second.substr(7).c_str()));
        }
        // get port
        if (sdpIter->first == "m") {
            if (boost::regex_match(sdpIter->second.c_str(), matches,
                    portRegex)) {

                // convert port
                string tempStr(matches[1].first, matches[1].second);
                unsigned short port = StringToNumber<unsigned short>(tempStr);

                // abort on bogus port number.
                if (port == 0 || port > 65535)
                    return false;
                m_sdpSocketAddress.setPort(port);
            }
        }
    }
    m_sdpSocketAddress.setTransportProto(tp_UDP);
    L_t
    << "SDP socket address: " << m_sdpSocketAddress;
    return true;
}

bool SipPacket::hasSdpPayload() {
    return m_hasSdpPayload;
}

requestMethodEnum SipPacket::requestToEnum(const string& request) const {
    string r = request;
    transform(r.begin(), r.end(), r.begin(), ptr_fun(::toupper)); // toupper
    if (r == "INVITE")
        return rm_INVITE;
    if (r == "ACK")
        return rm_ACK;
    if (r == "BYE")
        return rm_BYE;
    if (r == "CANCEL")
        return rm_CANCEL;
    if (r == "REGISTER")
        return rm_REGISTER;
    if (r == "OPTIONS")
        return rm_OPTIONS;
    if (r == "INFO")
        return rm_INFO;
    return rm_UNDEFINED;
}

const string& SipPacket::getCallId() const {
    return m_callId;
}

const string& SipPacket::getRequestUri() const {
    return m_requestUri;
}

const string& SipPacket::getToString() const {
    return m_toString;
}

const string& SipPacket::getFromString() const {
    return m_fromString;
}

const SipFromTo& SipPacket::getFrom() const {
    return m_from;
}

const SipFromTo& SipPacket::getTo() const {
    return m_to;
}

const string& SipPacket::getBranch() const {
    return m_branch;
}

const string& SipPacket::getSentBy() const {
    return m_sentBy;
}

const transportProtoEnum SipPacket::getSentByProtocol() const {
    return m_sentByProtocol;
}

requestMethodEnum SipPacket::getCseqMethod() const {
    return m_cseqMethod;
}

u_int SipPacket::getCseqNum() const {
    return m_seqNum;
}

messageTypeEnum SipPacket::getMessageType() const {
    return m_messageType;
}

responseCodeEnum SipPacket::getResponseCategory() const {
    if (m_responseStatusCode >= 100 && m_responseStatusCode <= 199)
        return rc_PROVISIONAL;
    if (m_responseStatusCode >= 200 && m_responseStatusCode <= 299)
        return rc_SUCCESS;
    if (m_responseStatusCode >= 300 && m_responseStatusCode <= 399)
        return rc_REDIRECTION;
    if (m_responseStatusCode >= 400 && m_responseStatusCode <= 499)
        return rc_CLIENT_ERROR;
    if (m_responseStatusCode >= 500 && m_responseStatusCode <= 599)
        return rc_SERVER_ERROR;
    if (m_responseStatusCode >= 600 && m_responseStatusCode <= 699)
        return rc_GLOBAL_FAILURE;
    return rc_NONVALID;
}

u_int SipPacket::getResponseCode() const {
    return m_responseStatusCode;
}

const string& SipPacket::getResponseReason() const {
    return m_responseReason;
}

const SocketAddress& SipPacket::getSdpMediaSocket() const {
    return m_sdpSocketAddress;
}

requestMethodEnum SipPacket::getRequestMethod() const {
    return m_requestMethod;
}

} /* namespace callx */
