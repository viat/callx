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
 * TcpServer.hpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include <boost/asio.hpp>
#include "Session.hpp"

using boost::asio::ip::tcp;

namespace callx {

class TcpServer {
public:
    TcpServer(boost::asio::io_service& io_service, short port);
    void handle_accept(Session* new_session,
            const boost::system::error_code& error);
    virtual ~TcpServer();

private:
    boost::asio::io_service& m_ioService;
    tcp::acceptor m_acceptor;
};

} /* namespace callx */
#endif /* TCPSERVER_HPP_ */
