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
 * TcpServer.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#include "main/callx.hpp"
#include "TcpServer.hpp"
#include "Session.hpp"
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>

namespace callx {

TcpServer::TcpServer(boost::asio::io_service& io_service, short port)
        : m_ioService(io_service),
          m_acceptor(io_service,
                  tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"),
                          port)) {
    L_t
            << "C'tor";
    Session* new_session = new Session(m_ioService);
    m_acceptor.async_accept(new_session->socket(),
            boost::bind(&TcpServer::handle_accept,
                    this,
                    new_session,
                    boost::asio::placeholders::error));
}

TcpServer::~TcpServer() {
    L_t
            << "D'tor";
}

void TcpServer::handle_accept(Session* new_session,
        const boost::system::error_code& error) {
    if (!error) {
        new_session->start();
        new_session = new Session(m_ioService);
        m_acceptor.async_accept(new_session->socket(),
                boost::bind(&TcpServer::handle_accept,
                        this,
                        new_session,
                        boost::asio::placeholders::error));
    } else {
        delete new_session;
    }
}

} /* namespace callx */
