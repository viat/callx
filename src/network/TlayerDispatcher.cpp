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
 * TlayerDispatcher.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: mainka
 */

#include "TlayerDispatcher.hpp"
#include "network/TlayerPacket.hpp"
#include "network/TlayerPacketRecycler.hpp"

using namespace std;

namespace callx {

TlayerDispatcher::TlayerDispatcher() {
    L_t
    << "C'tor";
    classname = "TlayerDispatcher";
    m_pcapPacketQueue = PcapPacketQueue::getInstance();
    m_udpPacketQueue = UdpPacketQueue::getInstance();
}

TlayerDispatcher::~TlayerDispatcher() {
    L_t
    << "D'tor";
}

bool TlayerDispatcher::stop() {
    L_t
    << "deactivating PcapPacketQueue";
    m_pcapPacketQueue->deactivate();
    L_t
    << "calling parent stop()";

    // parent
    return CallxThread::stop();
}

void TlayerDispatcher::worker() {

    unique_ptr<TlayerPacket, TlayerPacketRecycler> tlayerPacket;

    while (!m_stopRequested) {

        //boost::this_thread::sleep(boost::posix_time::milliseconds(2));

        // get packet from PCAP packet queue
        if (!m_pcapPacketQueue->waitAndPop(tlayerPacket)) {

            // We are here, because the Queue has been deactivated.
            L_t
                    << "wait_and_pop() results false -> PcapPacketQueue has been deativated.";
            L_t
            << "m_stopRequested: " << m_stopRequested;
            continue;
        }

        if (!tlayerPacket->parseNetlayer()) {

            // parsing network layer failed
            continue;
        }

        // push packet into UdpQueue (if it's an UDP packet)
        if (tlayerPacket->m_dstSocketAddress.getTransportProtocol() == tp_UDP) {
            m_udpPacketQueue->push(move(tlayerPacket));
        }

        // TCP is not implemented
        //
        //  if (tlayerPacket->m_dstSocketAddress.m_transportProtocol == tp_TCP) {
        //      tcpPacketQueue->push(tlayerPacket);
        //  }

    }
    L_t
    << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

} /* namespace callx */
