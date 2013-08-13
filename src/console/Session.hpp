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
 * Session.hpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <boost/asio.hpp>
#include "CommandServer.hpp"

using boost::asio::ip::tcp;

namespace callx {

class Session {

public:
    Session(boost::asio::io_service& io_service);
    virtual ~Session();

    tcp::socket& socket();
    void start();
    void handle_read(const boost::system::error_code& error,
            size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

private:
    tcp::socket m_socket;
    enum {
        max_length = 1024
    };
    char m_input[max_length];
    std::ostringstream m_output;
    std::string m_userData;
    CommandServer m_cmdSrv;

    bool handleUserData(size_t bytes_transferred);
    std::string prompt();

};

} /* namespace callx */

#endif /* SESSION_HPP_ */
