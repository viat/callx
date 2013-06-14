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
 * Watchdog.cpp
 *
 *  Created on: Jan 16, 2013
 *      Author: mainka
 */

#include "Watchdog.hpp"
#include "container/CallMap.hpp"
#include "boost/thread.hpp"

namespace callx {

Watchdog::Watchdog()
        : m_callMap(CallMap::getInstance()) {
    L_t
    << "C'tor";
    classname = "Watchdog";
}

Watchdog::~Watchdog() {
    L_t
    << "D'tor";
}

void Watchdog::worker() {
    while (!m_stopRequested) {


        // This thread should sleep some time.
        // Not available in GCC 4.4.5 (Debian Squeeze package):
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // the boost variant
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));

        m_callMap->handleCallTimeouts();

    } // while (!m_stopRequested)

    L_t
    << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

} /* namespace callx */
