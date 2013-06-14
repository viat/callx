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
 * SocketAddress.hpp
 *
 *  Created on: Aug 7, 2012
 *      Author: mainka
 */

#ifndef SOCKETADDRESS_HPP_
#define SOCKETADDRESS_HPP_

#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include "sip/sip.hpp"

namespace callx {

class SocketAddress {

public:

    /**
     * Standard constructor
     */
    SocketAddress();

    /**
     * Constructor
     * @param IP Address
     * @param Port
     */
    SocketAddress(boost::asio::ip::address ipAddress,
            unsigned short port,
            transportProtoEnum tp);

    /**
     * Destructor
     */
    virtual ~SocketAddress();

    /**
     * sets the IP address of the socket.
     * @param IP address
     */
    void setIpAddress(boost::asio::ip::address ipAddress);

    /*
     * Sets the port of the socket.
     * @param Port
     */
    void setPort(unsigned short port);

    /**
     * Sets the transport protocol.
     */
    void setTransportProto(transportProtoEnum tp);

    /**
     * Sets IP, port and transport protocol.
     */
    void setData(boost::asio::ip::address ipAddress,
            unsigned short port,
            transportProtoEnum tp);

    /**
     * IP address of the socket.
     * @return IP address
     */
    inline const boost::asio::ip::address& getIpAddress() const {
        return m_ipAddress;
    }

    /**
     * Port of the socket.
     * @return Port
     */
    inline unsigned short getPort() const {
        return m_port;
    }

    /**
     * Transport protocol of the socket.
     * @return Transport protocol as enum.
     */
    inline transportProtoEnum getTransportProtocol() const {
        return m_transportProtocol;
    }

    /**
     * Transport protocol of the socket.
     * @return Transport protocol as enum.
     */
    inline std::string getTransportProtocolStr() const {
        switch (m_transportProtocol) {
        case tp_TCP:
            return "TCP";
            break;
        case tp_UDP:
            return "UDP";
            break;
        default:
            return "unknown";
        }
    }

    /**
     *
     */
    inline bool isValid() const {
        return !m_ipAddress.is_unspecified() && m_port > 0
                && m_transportProtocol > 0;
    }

    /**
     * This socket address is smaller if the other IP is greater
     * or if both IPs are equal and the other port is greater
     * or if both IPs and ports are equal and the other transport protocol
     * is greater.
     * @return True if smaller.
     */
    bool operator<(const SocketAddress& other) const;

    /**
     * Opposite of operator<()
     * @return True if greater.
     */
    bool operator>(const SocketAddress& other) const;

    /**
     * Equality
     * @return True if equal.
     */
    bool operator==(const SocketAddress& other) const;

    /**
     * Inequality
     * @return True if not equal.
     */
    bool operator!=(const SocketAddress& other) const;

    /**
     * This streaming method may access private data.
     * @return Stream
     */
    friend std::ostream &operator<<(std::ostream &stream,
            SocketAddress *socketAddress);

    /**
     * This streaming method may access private data.
     * @return Stream
     */
    friend std::ostream &operator<<(std::ostream &stream,
            SocketAddress socketAddress);

private:

    /**
     * The IP address of the socket.
     */
    boost::asio::ip::address m_ipAddress;

    /**
     * The port of the socket.
     */
    unsigned short m_port;

    /**
     * The transport protocol (UDP, TCP, ...)
     */
    transportProtoEnum m_transportProtocol;
}
;

/**
 * An overloaded output stream method.
 * It adds the socket address to the stream, format <IP>:<port>
 * @param stream The stream reference.
 * @param socketAddress A pointer to a SocketAddress object.
 * @return Stream reference
 */
inline std::ostream &operator<<(std::ostream &stream,
        SocketAddress *socketAddress) {
    return stream << socketAddress->m_ipAddress << ":" << socketAddress->m_port
            << " " << socketAddress->getTransportProtocolStr();
}

/**
 * An overloaded output stream method.
 * It adds the socket address to the stream, format <IP>:<port>
 * @param stream The stream reference.
 * @param socketAddress A SocketAddress object.
 * @return Stream reference
 */
inline std::ostream &operator<<(std::ostream &stream,
        SocketAddress socketAddress) {
    return stream << socketAddress.m_ipAddress << ":" << socketAddress.m_port
            << " " << socketAddress.getTransportProtocolStr();
}

} /* namespace callx */

#endif /* SOCKETADDRESS_HPP_ */
