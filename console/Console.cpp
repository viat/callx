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
 * Console.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: mainka
 */

#include "Console.hpp"
#include "console/TcpServer.hpp"

using namespace std;

namespace callx {

Console::Console() {
    L_t
            << "C'tor";
    classname = "Console";
}

Console::~Console() {
    L_t
            << "D'tor";
}

bool Console::stop() {
    L_t
            << "Overridden stop() has been called, stopping TcpServer first.";
    m_ioService.stop();
    return CallxThread::stop(); // parent
}

void Console::worker() {

    TcpServer tcpServer(m_ioService, m_callxConfig->console_port);

    m_ioService.run();

    m_stopped = true;
    L_t
            << "The worker stopped.";
}

} /* namespace callx */
