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
 * PcmAudio.cpp
 *
 *  Created on: Dec 10, 2012
 *      Author: mainka
 */

#include "PcmAudio.hpp"

using namespace std;

namespace callx {

PcmAudio::PcmAudio(const string& callId,
        const SipFromTo& caller,
        const SipFromTo& callee,
        const systemClock::time_point& startTs,
        bool isCallerAudio)
        : m_callId(callId),
          m_caller(caller),
          m_callee(callee),
          m_startTs(startTs),
          m_isCallerAudio(isCallerAudio),
          m_memChunkDeque(make_shared<MemChunkDeque>()) {
    L_t
            << "C'tor";
}

PcmAudio::~PcmAudio() {
    L_t
            << "D'tor";
}

const std::string& PcmAudio::getCallId() const {
    return m_callId;
}

const SipFromTo& PcmAudio::getCaller() const {
    return m_caller;
}

const SipFromTo& PcmAudio::getCallee() const {
    return m_callee;
}

const systemClock::time_point& PcmAudio::getStartTs() const {
    return m_startTs;
}

void PcmAudio::addChunk(std::shared_ptr<MemChunk>& chunk) {
    m_memChunkDeque->push_back(move(chunk));
}

bool PcmAudio::isCallerAudio() const {
    return m_isCallerAudio;
}

shared_ptr<MemChunkDeque> PcmAudio::getChunkDeque() {
    return m_memChunkDeque;
}

} /* namespace callx */
