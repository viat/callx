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
 * Watchdog.hpp
 *
 *  Created on: Jan 16, 2013
 *      Author: mainka
 */

#ifndef WATCHDOG_HPP_
#define WATCHDOG_HPP_

#include "main/CallxThread.hpp"

namespace callx {

class CallMap;

class Watchdog: public CallxThread {
public:
    Watchdog();
    virtual ~Watchdog();

    /**
     * Implementation of abstract method CallxThread::worker()
     */
    void worker();

private:

    void deleteOutdatedCalls();

    CallMap *m_callMap;

};

} /* namespace callx */
#endif /* WATCHDOG_HPP_ */
