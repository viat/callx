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
 * CallxThread.cpp
 *
 *  Created on: Aug 21, 2012
 *      Author: mainka
 */

#include "CallxThread.hpp"
#include "boost/thread.hpp"

namespace callx {

CallxThread::CallxThread()
        : m_callxConfig(0),
          m_stopRequested(false),
          m_stopped(true) {
    m_callxConfig = CallxConfig::getInstance();
}

CallxThread::~CallxThread() {
    /* The thread object must not be deleted while it is joinable */
    if (m_thread.joinable()) {
        m_thread.detach();
    }
}

void CallxThread::start() {
    m_stopped = false;
    m_thread = std::thread(&callx::CallxThread::worker, this);
}

bool CallxThread::stop() {
    L_t
            << "Stopping thread "
            << classname
            << ".";
    m_stopRequested = true;
    bool waiting = true;
    for (int i = 0; i < 10; i++) {
        if (m_stopped) {
            L_i
                    << classname
                    << " stopped working.";
            waiting = false;
            break;
        } else {
            L_t
                    << classname
                    << " is still working.";
        }
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    if (waiting) {
        L_f
                << classname
                << " does not stop working. Giving up!";
        return false;
    }
    return true;
}

bool CallxThread::workerStopped() {
    return m_stopped;
}

bool CallxThread::stopRequested() {
    return m_stopRequested;
}

void CallxThread::join() {
    m_thread.join();
}

} /* namespace callx */
