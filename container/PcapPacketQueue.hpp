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
 * PcapPacketQueue.hpp
 *
 *  Created on: Aug 20, 2012
 *      Author: mainka
 */

#ifndef PCAPPACKETQUEUE_HPP_
#define PCAPPACKETQUEUE_HPP_

#include "ThreadFriendlyQueue.hpp"
#include "main/CallxSingleton.hpp"

namespace callx {

class TlayerPacket;
class TlayerPacketRecycler;

class PcapPacketQueue:
        public CallxSingleton<PcapPacketQueue>,
        public ThreadFriendlyQueue<
                std::unique_ptr<TlayerPacket, TlayerPacketRecycler>> {

    friend class CallxSingleton<PcapPacketQueue>;

public:

    /*
     * Destructor
     */
    ~PcapPacketQueue() {
        L_t
        << "D'tor";
    }

protected:

    /*
     * Hidden constructor
     */
    PcapPacketQueue() {
        L_t
        << "C'tor";
    }

};

} /* namespace callx */

#endif /* PCAPPACKETQUEUE_HPP_ */
