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
 * SocketAddress.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: mainka
 */

#include "SocketAddress.hpp"

namespace callx {

SocketAddress::SocketAddress()
        : m_port(0), m_transportProtocol(tp_UNDEFINED) {
}

SocketAddress::SocketAddress(
        boost::asio::ip::address ipAddress,
        unsigned short port,
        transportProtoEnum tp)
        : m_ipAddress(ipAddress), m_port(port), m_transportProtocol(tp) {
}

SocketAddress::~SocketAddress() {
}

void SocketAddress::setIpAddress(boost::asio::ip::address ipAddress) {
    m_ipAddress = ipAddress;
}

void SocketAddress::setPort(unsigned short port) {
    m_port = port;
}

void SocketAddress::setTransportProto(transportProtoEnum tp) {
    m_transportProtocol = tp;
}

void SocketAddress::setData(
        boost::asio::ip::address ipAddress,
        unsigned short port,
        transportProtoEnum tp) {
    m_ipAddress = ipAddress;
    m_port = port;
    m_transportProtocol = tp;
}

bool SocketAddress::operator<(const SocketAddress& other) const {
    return m_ipAddress < other.getIpAddress()
            || (m_ipAddress == other.getIpAddress() && m_port < other.getPort())
            || (m_ipAddress == other.getIpAddress() && m_port == other.getPort()
                    && m_transportProtocol < other.getTransportProtocol());
}

bool SocketAddress::operator>(const SocketAddress& other) const {
    return !(other < *this);
}

bool SocketAddress::operator==(const SocketAddress& other) const {
    return m_ipAddress == other.getIpAddress() && m_port == other.getPort()
            && m_transportProtocol == other.getTransportProtocol();
}

bool SocketAddress::operator!=(const SocketAddress& other) const {
    return !(*this == other);
}

} /* namespace callx */
