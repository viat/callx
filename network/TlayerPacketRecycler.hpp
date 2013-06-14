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
 * TlayerPacketRecycler.hpp
 *
 *  Created on: Feb 9, 2013
 *      Author: mainka
 */

#ifndef TLAYERPACKETRECYCLER_HPP_
#define TLAYERPACKETRECYCLER_HPP_

#include "container/TlayerPacketQueue.hpp"
#include "main/callx.hpp"

namespace callx {

class TlayerPacketRecycler {
public:

    TlayerPacketRecycler()
            : m_tlayerPacketQueue(TlayerPacketQueue::getInstance()) {
    }

    void operator()(TlayerPacket* tlayerPacket) {

        // Recycling is only possible if there is a TlayerPacket object.
        if (tlayerPacket) {
            // L_t << "Recycling TlayerPacket object!";
            m_tlayerPacketQueue->push(
                    std::unique_ptr<TlayerPacket, TlayerPacketRecycler>(
                            tlayerPacket));
        }
    }

private:
    TlayerPacketQueue *m_tlayerPacketQueue;
};

} /* namespace callx */
#endif /* TLAYERPACKETRECYCLER_HPP_ */
