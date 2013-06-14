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
 * RtpSink.hpp
 *
 *  Created on: Oct 16, 2012
 *      Author: mainka
 */

#ifndef RTPSINK_HPP_
#define RTPSINK_HPP_

#include <queue>
#include <string>
#include <memory>
#include "main/CallxTypes.hpp"

namespace callx {

class TlayerPacket;
class TlayerPacketRecycler;
class TlayerPacketQueue;

enum rtpSinkStatusEnum {
    rs_INIT = 0,
    rs_CONFIRMED,
    rs_CONFIRMED_INIT,
    rs_TERMINATED
};

static const std::string rtpSinkStatus[] = { "INIT", "CONFIRMED",
        "CONFIRMED_INIT", "TERMINATED" };

inline std::string rtpSinkStatusEnumToString(rtpSinkStatusEnum status) {
    return rtpSinkStatus[status];
}

class RtpSink {
public:
    RtpSink(bool isCallerSink);
    virtual ~RtpSink();

    /**
     * Stores a TlayerPacket with RTP data in an internal queue.
     * @param tlayerPacket
     */
    void rollIn(std::unique_ptr<TlayerPacket, TlayerPacketRecycler>&& tlayerPacket);

    /**
     * Returns RtpSink inactivity time.
     * @return Number of seconds of inactivity.
     */
    size_t inactiveFor();

    /**
     * Deactivates the storing of TlaerPacket objects.
     */
    void deactivate();

    /**
     * Transafers the internal queue of TlayerPacket objects (unique_ptr).
     * After that, the RtpSink is no longer the owner of the queue.
     * @return unique_ptr of the internal TlayerDeque.
     */
    std::unique_ptr<TlayerDeque> getTlayerDeque();
    void setStatus(rtpSinkStatusEnum status);
    rtpSinkStatusEnum getStatus() const;

    /**
     * Caller or Callee sink.
     * @return True, if this is a sink of the caller.
     */
    bool isCallerSink() const;

protected:

    // caller or callee sink
    bool m_isCallerSink;

    // If RTP data should be stored or discarded.
    bool m_active;

    // activity timestamp (steady clock)
    steadyClock::time_point m_activityTs;

    // status of the RTP sink
    rtpSinkStatusEnum m_rtpSinkStatus;

    // stored RTP data
    std::unique_ptr<TlayerDeque> m_rtpPacketDeque;

    // packet count (for activity check)
    size_t m_packetCount;
    size_t m_packetCountOld;

    // For the manual (faster?) recycling of TlayerPackets.
    TlayerPacketQueue* m_tlayerPacketQueue;
};

} /* namespace callx */
#endif /* RTPSINK_HPP_ */
