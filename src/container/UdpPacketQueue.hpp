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
 * UdpPacketQueue.hpp
 *
 *  Created on: Aug 20, 2012
 *      Author: mainka
 */

#ifndef UDPPACKETQUEUE_HPP_
#define UDPPACKETQUEUE_HPP_

//#include "network/TlayerPacket.hpp"
//#include "network/TlayerPacketRecycler.hpp"
#include "ThreadFriendlyQueue.hpp"
#include "main/CallxSingleton.hpp"

namespace callx {

class TlayerPacket;
class TlayerPacketRecycler;

class UdpPacketQueue:
        public CallxSingleton<UdpPacketQueue>,
        public ThreadFriendlyQueue<
                std::unique_ptr<TlayerPacket, TlayerPacketRecycler>> {

    friend class CallxSingleton<UdpPacketQueue>;

public:
    /*
     * Copying not allowed.
     */
    UdpPacketQueue(const UdpPacketQueue&) = delete;

    /*
     * Assigning not allowed.
     */
    UdpPacketQueue& operator=(const UdpPacketQueue&) = delete;

    /*
     * Destructor.
     */
    ~UdpPacketQueue() {
        L_t << "D'tor";
    }

protected:

    /*
     * Hide constructor.
     */
    UdpPacketQueue() {
        L_t << "C'tor";
    }

};

} /* namespace callx */

#endif /* UDPPACKETQUEUE_HPP_ */
