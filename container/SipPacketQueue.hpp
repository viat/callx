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
 * SipPacketQueue.hpp
 *
 *  Created on: Aug 26, 2012
 *      Author: mainka
 */

#ifndef SIPPACKETQUEUE_HPP_
#define SIPPACKETQUEUE_HPP_

#include "sip/SipPacket.hpp"
#include "main/CallxSingleton.hpp"
#include "ThreadFriendlyQueue.hpp"

namespace callx {

class SipPacketQueue:
        public CallxSingleton<SipPacketQueue>,
        public ThreadFriendlyQueue<std::unique_ptr<SipPacket> > {

    friend class CallxSingleton<SipPacketQueue> ;

public:

    /**
     * Destructor
     */
    virtual ~SipPacketQueue() {
        L_t
        << "D'tor";
    }

protected:
    /**
     * Hidden constructor
     */
    SipPacketQueue() {
        L_t
        << "C'tor";
    }

};

} /* namespace callx */

#endif /* SIPPACKETQUEUE_HPP_ */

