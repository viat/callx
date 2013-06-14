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
 * Console.hpp
 *
 *  Created on: Aug 26, 2012
 *      Author: mainka
 */

#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include "main/CallxThread.hpp"
#include <boost/asio.hpp>

namespace callx {

class Console:
        public callx::CallxThread {
public:
    Console();
    virtual ~Console();

    /**
     * Overrides CallxThread::stop(). We need this because the TcpServer
     * has to be stopped first.
     */
    bool stop();

    /**
     * Implementation of abstract method CallxThread::worker()
     */
    void worker();

private:
    boost::asio::io_service m_ioService;
};

} /* namespace callx */

#endif /* CONSOLE_HPP_ */
