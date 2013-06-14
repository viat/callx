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
 * UdpHandler.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: mainka
 */

#include "UdpHandler.hpp"
#include "TlayerPacketRecycler.hpp"
#include "TlayerPacket.hpp"
#include "container/UdpPacketQueue.hpp"
#include "container/SipPacketQueue.hpp"
#include "container/RtpSinkMap.hpp"

using namespace std;

namespace callx {

UdpHandler::UdpHandler() {
    L_t
            << "C'tor";
    classname = "UdpHandler";
    m_udpPacketQueue = UdpPacketQueue::getInstance();
    m_sipPacketQueue = SipPacketQueue::getInstance();
    m_rtpSinkMap = RtpSinkMap::getInstance();
}

UdpHandler::~UdpHandler() {
    L_t
            << "D'tor";
}

bool UdpHandler::stop() {
    L_t
            << "Overridden stop() has been called, deactivating UdpPacketQueue.";
    m_udpPacketQueue->deactivate();
    return CallxThread::stop();
}

void UdpHandler::worker() {

    unique_ptr<TlayerPacket, TlayerPacketRecycler> tlayerPacket;
    shared_ptr<RtpSink> rtpSink;
    unique_ptr<SipPacket> sipPacket;

    size_t minSipSize = m_callxConfig->min_sip_size;
    const char sipIdentStr[] = "SIP/2.0";
    size_t sipIdentStrLen = strlen(sipIdentStr);
    bool isSip;

    while (!m_stopRequested) {

        // get packet from UDP packet queue
        if (!m_udpPacketQueue->waitAndPop(tlayerPacket)) {
            // We are here, because the UdpPacketQueue has been deactivated.
            L_t
                    << "wait_and_pop() results false -> UdpPacketQueue"
                    << "is deativated.";
            L_t
                    << "m_stopRequested: "
                    << m_stopRequested;
            continue;
        }

        if (!tlayerPacket->parseTransportlayer()) {

            // parsing transport layer failed
            L_t
                    << "parsing transport layer failed";
            continue;
        }

        // Test if RTP version is 2 (pre-selection of potential RTP packets)
        // and if destination socket does exist in RtpSinkMap.
        if ((*(tlayerPacket->m_udpPacket.payload) >> 6 == 2)) {

            if (m_rtpSinkMap->find(tlayerPacket->m_dstSocketAddress, rtpSink)) {

                // L_t << "found socket " << tlayerPacket->m_dstSocketAddress;

                // push tlayerPacket into RtpSink
                rtpSink->rollIn(tlayerPacket);
                // next packet
                continue;
            }
        }

        // SIP test if not RTP
        isSip = false;

        // perform SIP test if size of UDP payload is at least minSipSize
        if (tlayerPacket->m_udpPacket.payloadLen < minSipSize) {



            // You should uncomment the following log output, if you lose
            // UDP packets.
            L_t << "Ignore UDP packet, payload < min_sip_size.";
            continue;
        }

        // Search the SIP ident string. A fast search in
        // undefined data, possibly no trailing binary zero at the end.
        for (u_int i = 0; i < minSipSize - sipIdentStrLen; i++) {

            // compare the first character of both strings
            if (tlayerPacket->m_udpPacket.payload[i] == sipIdentStr[0]) {

                // think positive!
                isSip = true;

                // compare remaining characters
                for (u_int n = 1; n < sipIdentStrLen; n++) {
                    if (tlayerPacket->m_udpPacket.payload[i + n]
                            != sipIdentStr[n]) {
                        isSip = false;
                        break;
                    }
                    if (isSip) {
                        break;
                    }
                }
            }
            if (isSip) {
                // create new SipPacket object
                sipPacket.reset(new SipPacket(move(tlayerPacket)));

                // Push SipPacket into SipPacketQueue
                m_sipPacketQueue->push(sipPacket);
                break;
            }
        }
    }

    L_t
            << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

} /* namespace callx */
