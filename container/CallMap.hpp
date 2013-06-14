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
 * CallMap.hpp
 *
 *  Created on: Aug 26, 2012
 *      Author: mainka
 */

#ifndef CALLMAP_HPP_
#define CALLMAP_HPP_

#include "ThreadFriendlyMap.hpp"
#include "main/CallxSingleton.hpp"
#include "sip/Call.hpp"

namespace callx {

class CallDecodeQueue;
class CallxConfig;

class CallMap:
        public ThreadFriendlyMap<std::string, std::shared_ptr<Call> >,
        public CallxSingleton<CallMap> {

    friend class CallxSingleton<CallMap> ;

public:

    /**
     * Destructor
     */
    virtual ~CallMap();

    /**
     * String representation.
     * @return string
     */
    std::string toString() const;

    /**
    * Check and handle exceeding of the following timers (config values):
    *
    * max_call_recording_time:
    *   ALl RtpSinks of the call will be deactivated.
    *
    * max_call_age:
    *   The call will be taken out of the CallMap and put into the
    *   CallDecodeQueue.
    *
    * max_call_rtp_inactivity:
    *   The call will be taken out of the CallMap and put into the
    *   CallDecodeQueue.
    */
    void handleCallTimeouts();

private:

    /**
     * Hidden constructor
     */
    CallMap();

    CallxConfig *m_callxConfig;
    CallDecodeQueue *m_callDecodeQueue;
};

} /* namespace callx */

#endif /* CALLMAP_HPP_ */
