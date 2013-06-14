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
 * RtpSink.cpp
 *
 *  Created on: Oct 16, 2012
 *      Author: mainka
 */

#include "RtpSink.hpp"
#include "network/TlayerPacket.hpp"
#include "network/TlayerPacketRecycler.hpp"

using namespace std;

namespace callx {

RtpSink::RtpSink(bool isCallerSink)
        : m_isCallerSink(isCallerSink),
          m_active(true),
          m_activityTs(steadyClock::now()),
          m_rtpSinkStatus(rs_INIT),
          m_rtpPacketDeque(unique_ptr<TlayerDeque>(new TlayerDeque())),
          m_packetCount(0),
          m_packetCountOld(0),
          m_tlayerPacketQueue(TlayerPacketQueue::getInstance()) {
    L_t
    << "C'tor";
}

RtpSink::~RtpSink() {
    L_t
    << "D'tor";
}

void RtpSink::rollIn(std::unique_ptr<TlayerPacket, TlayerPacketRecycler>&& tlayerPacket) {
    if(m_active) {
        // Save the  RTP packet.
        m_rtpPacketDeque->push_back(forward<std::unique_ptr<TlayerPacket,
                TlayerPacketRecycler>>(tlayerPacket));
    } else {
        // RtpSink is not activated. The recording time may be exceeded.
        // Push TlayerPacket back to the TlayerPacketQueue. Is this a bit
        // faster than relying on the TlayerPacketRecycler?
        m_tlayerPacketQueue->push(forward<std::unique_ptr<TlayerPacket,
                TlayerPacketRecycler>>(tlayerPacket));
    }
    m_packetCount++;
}

size_t RtpSink::inactiveFor() {
    if (m_packetCount > m_packetCountOld) {
        m_packetCount = m_packetCountOld;
        m_activityTs = steadyClock::now();
        return 0;
    } else {
        return (boost::chrono::duration_cast<seconds>(
                steadyClock::now() - m_activityTs)).count();
    }

}

void RtpSink::deactivate() {
    m_active = false;
}

unique_ptr<TlayerDeque> RtpSink::getTlayerDeque() {
    return move(m_rtpPacketDeque);
}

rtpSinkStatusEnum RtpSink::getStatus() const {
    return m_rtpSinkStatus;
}

void RtpSink::setStatus(rtpSinkStatusEnum status) {
    m_rtpSinkStatus = status;
}

bool RtpSink::isCallerSink() const {
    return m_isCallerSink;
}

} /* namespace callx */
