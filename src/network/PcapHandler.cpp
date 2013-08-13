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
 * PcapNetworkHandlerUdp.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: mainka
 */

#include "PcapHandler.hpp"
#include <string>

using namespace std;

namespace callx {

PcapHandler::PcapHandler()
        : m_tlayerPacketQueue(TlayerPacketQueue::getInstance()),
          m_pcapPacketQueue(PcapPacketQueue::getInstance()) {
    L_t
    << "C'tor";
    classname = "PcapHandler";
}

PcapHandler::~PcapHandler() {
    L_t
    << "D'tor";
}

inline void PcapHandler::callback(const struct pcap_pkthdr *pcapHeader,
        const u_char *pcapPacket) {

    // Get TlyerPacket object from TlayerPacketQueue, fill it with pcap
    // header & data and push it into PcapPacketQueue object.
    if (m_tlayerPacketQueue->waitAndPop(m_tlayerPacket)) {
        m_tlayerPacket->fill(*pcapHeader, pcapPacket);
        m_pcapPacketQueue->push(move(m_tlayerPacket));
    } else {
        L_t
        << "No TlayerPacket object available!";
    }
}

bool PcapHandler::stop() {

    // Force pcap_dispatch or pcap_loop to return.
    m_pcapWrapper.breakLoop();

    // call parent
    return CallxThread::stop();
}

void PcapHandler::worker() {

    L_t
    << "Opening PCAP device: " << m_callxConfig->pcap_device;
    if (!m_pcapWrapper.openDevice(m_callxConfig->pcap_device)) {
        L_f
        << "Error opening PCAP device: " << m_pcapWrapper.getErrorMessage();
        return;
    }

    L_t
    << "Setting PCAP filter: " << m_callxConfig->pcap_filter;
    if (!m_pcapWrapper.setFilter(m_callxConfig->pcap_filter)) {
        L_f
        << "Error setting PCAP filter: " << m_pcapWrapper.getErrorMessage();
        return;
    }

    int received = 0;
    int oldReceived = 0;

    while (!m_stopRequested) {

        m_pcapWrapper.loop(this, 1000, staticCallback);

        m_pcapWrapper.readStats();
        received = m_pcapWrapper.getReceived();
        if (received - oldReceived >= 5000) {
            oldReceived = received;
            L_t
            << "PCAP packets received: " << received << "\t dropped: "
                    << m_pcapWrapper.getDropped();
        }
    } // while (!m_stopRequested)

    L_t
    << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

} /* namespace callx */
