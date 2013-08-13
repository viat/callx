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
 * UdpHandler.hpp
 *
 *  Created on: Oct 16, 2012
 *      Author: mainka
 */

#ifndef UDPHANDLER_HPP_
#define UDPHANDLER_HPP_

#include "main/CallxThread.hpp"

namespace callx {

class UdpPacketQueue;
class SipPacketQueue;
class RtpSinkMap;

class UdpHandler: public CallxThread {
public:

    /**
     * C'tor
     */
    UdpHandler();

    /**
     * D'tor
     */
    virtual ~UdpHandler();

    /**
     * Overrides CallxThread::stop(). The UdpPacketQueue has to be disabled
     * before stopping.
     */
    bool stop();

    /**
     * Implementation of abstract method CallxThread::worker()
     */
    void worker();

protected:
    UdpPacketQueue *m_udpPacketQueue;
    SipPacketQueue *m_sipPacketQueue;
    RtpSinkMap *m_rtpSinkMap;
};

} /* namespace callx */
#endif /* UDPHANDLER_HPP_ */
