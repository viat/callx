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
 * TlayerDispatcher.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: mainka
 */

#ifndef TLAYERDISPATCHER_HPP_
#define TLAYERDISPATCHER_HPP_

#include "main/CallxThread.hpp"
#include "container/PcapPacketQueue.hpp"
#include "container/UdpPacketQueue.hpp"

namespace callx {

class TlayerDispatcher: public CallxThread {
public:
    /**
     * C'Tor
     */
    TlayerDispatcher();

    /**
     * D'Tor
     */
    virtual ~TlayerDispatcher();

    /**
     * Overrides method CallxThread::stop() because PcapPacketQueue has
     * to be disabled before stooping.
     */
    bool stop();

    /**
     * Implementation of the abstract method CallxThread::worker().
     */
    void worker();

protected:
    PcapPacketQueue *m_pcapPacketQueue;
    UdpPacketQueue *m_udpPacketQueue;
};

} /* namespace callx */
#endif /* TLAYERDISPATCHER_HPP_ */
