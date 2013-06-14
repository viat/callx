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
 * CallxThread.hpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#ifndef CALLXTHREAD_HPP_
#define CALLXTHREAD_HPP_

#include "callx.hpp"
#include "config/CallxConfig.hpp"
#include <thread>

namespace callx {

class CallxThread {
public:

    /**
     * Standard constructor
     */
    CallxThread();

    /**
     * Destructor
     */
    virtual ~CallxThread();

    /**
     * The abstract worker method that is started as thread.
     */
    virtual void worker() = 0;

    /**
     * Starts the worker as thread.
     */

    virtual void start();

    /**
     * Sets an internal stop flag.
     * The worker thread takes care of this flag and will (hopefully) stop.
     * @return stopped: true; timeout: false
     */
    virtual bool stop();

    /**
     * Returns status of worker thread.
     * @return true, if worker thread has stopped working
     */
    virtual bool stopRequested();

    /**
     * Returns information about stop request.
     * @return true, if there is a stop request.
     */
    virtual bool workerStopped();

    /**
     * Blocks the calling thread until this thread terminates.
     */
    virtual void join();


protected:
    CallxConfig *m_callxConfig;
    std::thread m_thread;
    bool m_stopRequested;
    bool m_stopped;
    std::string classname;
};

} /* namespace callx */

#endif /* CALLXTHREAD_HPP_ */
