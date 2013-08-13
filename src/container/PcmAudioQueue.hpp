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
 * PcmAudioQueue.hpp
 *
 *  Created on: Dec 12, 2012
 *      Author: mainka
 */

#ifndef PCMAUDIOQUEUE_HPP_
#define PCMAUDIOQUEUE_HPP_

#include "main/CallxSingleton.hpp"
#include "ThreadFriendlyQueue.hpp"
#include "audio/PcmAudio.hpp"
#include "network/TlayerPacketRecycler.hpp"

namespace callx {

class PcmAudioQueue:
        public CallxSingleton<PcmAudioQueue>,
        public ThreadFriendlyQueue<std::unique_ptr<PcmAudio>> {

    friend class CallxSingleton<PcmAudioQueue>;

public:

    /**
     * Destructor
     */
    virtual ~PcmAudioQueue() {
        L_t << "D'tor";
    }

private:

    /**
     * Hidden constructor
     */
    PcmAudioQueue() {
        L_t << "C'tor";
    }

};

} /* namespace callx */
#endif /* PCMAUDIOQUEUE_HPP_ */
