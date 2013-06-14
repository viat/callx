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
 * RtpSinkMap.hpp
 *
 *  Created on: Oct 16, 2012
 *      Author: root
 */

#ifndef RTPSINKMAP_HPP_
#define RTPSINKMAP_HPP_

#include "ThreadFriendlyMap.hpp"
#include "network/SocketAddress.hpp"
#include "main/CallxSingleton.hpp"
#include "audio/RtpSink.hpp"

namespace callx {

class RtpSinkMap:
        public CallxSingleton<RtpSinkMap>,
        public ThreadFriendlyMap<SocketAddress, std::shared_ptr<RtpSink> > {

    friend class CallxSingleton<RtpSinkMap>;

public:

    /**
     * Destructor
     */
    ~RtpSinkMap() {
    }

private:

    /**
     * Hidden constructor
     */
    RtpSinkMap() {
    }

};

} /* namespace callx */
#endif /* RTPSINKMAP_HPP_ */
