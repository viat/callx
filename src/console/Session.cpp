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
 * Session.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#include "Session.hpp"
#include "main/callx.hpp"
#include <boost/bind.hpp>

namespace callx {

using boost::asio::ip::tcp;

using namespace std;

Session::Session(boost::asio::io_service& io_service)
        : m_socket(io_service) {
    L_t
            << "C'tor";
}

Session::~Session() {
    L_t
            << "D'tor";
}

tcp::socket& Session::socket() {
    return m_socket;
}

void Session::start() {
    m_socket.write_some(boost::asio::buffer(m_cmdSrv.welcome().c_str(),
            m_cmdSrv.welcome().size()));
    m_socket.write_some(boost::asio::buffer(m_cmdSrv.prompt().c_str(),
            m_cmdSrv.prompt().size()));
    m_socket.async_read_some(boost::asio::buffer(m_input, max_length),
            boost::bind(&Session::handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void Session::handle_read(const boost::system::error_code& error,
        size_t bytes_transferred) {

    bool quitRequest = false;
    bool terminateRequest = false;

    if (!error) {

        m_output.str(string());
        m_output.clear();

        if (handleUserData(bytes_transferred)) {
            L_t
                    << "m_userData: "
                    << m_userData;
            if (m_userData == "status" || m_userData == "s") {
                m_output << m_cmdSrv.listContainer();
                m_userData = "";
            } else if (m_userData == "CallMap" || m_userData == "c") {
                m_output << m_cmdSrv.listCallMap();
                m_userData = "";
            } else if (m_userData == "SbaEventMap" || m_userData == "e") {
                m_output << m_cmdSrv.listSbaEventMap();
                m_userData = "";
            } else if (m_userData == "SbaIncidentMap" || m_userData == "i") {
                m_output << m_cmdSrv.listSbaIncidentMap();
                m_userData = "";
            } else if (m_userData == "ClearSba" || m_userData == "l") {
                m_output << m_cmdSrv.clearSba();
                m_userData = "";
            } else if (m_userData == "quit" || m_userData == "exit"
                    || m_userData == "q") {
                m_output << "Bye, bye...\r\n";
                quitRequest = true;
            } else if (m_userData == "terminate" || m_userData == "t") {
                terminateRequest = true;
                m_output << "Terminating application...\r\n";
            } else if (m_userData == "help" || m_userData == "h") {
                m_output << "\r\nList of commands. Shortcuts in brackets. "
                        << "Commands are case sensitive.\r\n"
                        << "(s) status\r\n"
                        << "(c) CallMap\r\n"
                        << "(e) SbaEventMap\r\n"
                        << "(i) SbaIncidentMap\r\n"
                        << "(l) ClearSba\r\n"
                        << "(q) quit (console)\r\n"
                        << "(t) terminate (program)\r\n"
                        << "\r\n";
                m_userData = "";

            } else if (m_userData != "") {
                m_output << "command not available\r\n";
                m_userData = "";
            }

            m_output << prompt();
            boost::asio::async_write(m_socket,
                    boost::asio::buffer(m_output.str(), m_output.tellp()),
                    boost::bind(&Session::handle_write,
                            this,
                            boost::asio::placeholders::error));
        } else {
            boost::asio::async_write(m_socket,
                    boost::asio::buffer("", 0),
                    boost::bind(&Session::handle_write,
                            this,
                            boost::asio::placeholders::error));
        }

    } else {
        delete this;
    }

    if (quitRequest) {
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_socket.close();
    } else if (terminateRequest) {
        L_i
                << "Terminating (console request)...";
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_socket.close();
        raise(SIGINT);
    }

}

void Session::handle_write(const boost::system::error_code& error) {
    if (!error) {
        m_socket.async_read_some(boost::asio::buffer(m_input, max_length),
                boost::bind(&Session::handle_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}

bool Session::handleUserData(size_t bytes_transferred) {
    if (m_userData.size() + bytes_transferred > 200) {
        m_userData = "";
        m_output << "\r\nIGNORED\r\n";
        return false;
    } else if (m_input[bytes_transferred - 2] == 0x0D
            && m_input[bytes_transferred - 1] == 0x0A) {
        string tmp;
        tmp.assign(m_input, bytes_transferred - 2);
        m_userData += tmp;
        return true;
    } else {
        string tmp;
        tmp.assign(m_input, bytes_transferred);
        m_userData += tmp;
        return false;
    }
}

std::string Session::prompt() {
    return "callx> ";
}

} /* namespace callx */
