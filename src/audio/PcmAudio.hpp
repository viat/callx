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
 * PcmAudio.hpp
 *
 *  Created on: Dec 10, 2012
 *      Author: mainka
 */

#ifndef PCMAUDIO_HPP_
#define PCMAUDIO_HPP_

#include "main/callx.hpp"
#include "deque"

namespace callx {

class MemChunk;

typedef std::deque<std::shared_ptr<MemChunk>> MemChunkDeque;

/**
 * PcmAudio
 */
class PcmAudio {
public:
    PcmAudio(const std::string& callId,
            const SipFromTo& caller,
            const SipFromTo& callee,
            const systemClock::time_point& startTs,
            bool isCallerAudio);
    virtual ~PcmAudio();

    const std::string& getCallId() const;
    const SipFromTo& getCaller() const;
    const SipFromTo& getCallee() const;
    const systemClock::time_point& getStartTs() const;
    bool isCallerAudio() const;

    void addChunk(std::shared_ptr<MemChunk>& chunk);
    std::shared_ptr<MemChunkDeque> getChunkDeque();

protected:
    std::string m_callId;
    SipFromTo m_caller;
    SipFromTo m_callee;
    systemClock::time_point m_startTs;
    bool m_isCallerAudio;

    std::shared_ptr<MemChunkDeque> m_memChunkDeque;
};

} /* namespace callx */

#endif /* PCMAUDIO_HPP_ */
