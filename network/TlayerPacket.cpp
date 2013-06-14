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
 * TlayerPacket.cpp
 *
 *  Created on: Oct 10, 2012
 *      Author: minka
 */

#include "TlayerPacket.hpp"
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

namespace callx {

TlayerPacket::TlayerPacket()
        : m_parsedNetworklayer(false),
          m_parsedTransportlayer(false) {
}

void TlayerPacket::fill(pcap_pkthdr pcapHeader, const u_char *pcapPacket) {

    m_parsedNetworklayer = false;
    m_parsedTransportlayer = false;

    // PCAP header
    m_pcapPacket.header = pcapHeader;

    // PCAP payload length
    m_pcapPacket.payloadLen = m_pcapPacket.header.caplen;

    // copy PCAP data
    memcpy(m_pcapPacket.payload, pcapPacket, m_pcapPacket.payloadLen);
}

TlayerPacket::~TlayerPacket() {
}

bool TlayerPacket::parseNetlayer() {

    // size of Ethernet header
    m_ethernetPacket.headerLen = sizeof(ether_header);

    // Ethernet header
    m_ethernetPacket.header = (ether_header*) m_pcapPacket.payload;

    // IPv4 or IPv6
    uint16_t etherPayloadType = ntohs(m_ethernetPacket.header->ether_type);
    if (etherPayloadType == ETHERTYPE_IP) {
        m_ipPacket.version = 4;
    } else if (etherPayloadType == ETHERTYPE_IPV6) {
        m_ipPacket.version = 6;
    } else {
        L_t
        << "Ignoring packet: Neither IPv4 nor IPv6 payload.";
        return false;
    }

    // ethernet payload size
    m_ipPacket.payloadLen = m_pcapPacket.header.caplen
            - m_ethernetPacket.headerLen;

    // set etherPayloadStart pointer to the beginning of ethernet payload
    m_ethernetPacket.payload = (u_char *) m_ethernetPacket.header
            + m_ethernetPacket.headerLen;

    // start of IP header is start of Ethernet payload
    m_ipPacket.header = (ip*) m_ethernetPacket.payload;

    if (m_ipPacket.version == 4) {

        // size of ethernet payload
        m_ethernetPacket.payloadLen = m_pcapPacket.payloadLen
                - m_ethernetPacket.headerLen;

        // size of IP header
        m_ipPacket.headerLen = m_ipPacket.header->ip_hl * 4;

        // size of IP payload
        m_ipPacket.payloadLen = m_ipPacket.payloadLen - m_ipPacket.headerLen;

        // start of IP payload
        m_ipPacket.payload = m_ethernetPacket.payload + m_ipPacket.headerLen;

        // network layer sanity check
        if (m_ipPacket.headerLen + m_ipPacket.payloadLen
                > m_ethernetPacket.payloadLen) {

            L_t
            << "Network layer sanity check: "
                    << "IP packet larger than ethernet payload. Discarding packet.";
            return false;
        }

        // convert IP addresses to string
        inet_ntop(AF_INET, &m_ipPacket.header->ip_src.s_addr, srcIPv4TmpStr,
                INET_ADDRSTRLEN);

        inet_ntop(AF_INET, &m_ipPacket.header->ip_dst.s_addr, dstIPv4TmpStr,
                INET_ADDRSTRLEN);

        // init SocketAddress objects
        m_srcSocketAddress.setIpAddress(
                boost::asio::ip::address::from_string(srcIPv4TmpStr));
        m_srcSocketAddress.setTransportProto(
                (transportProtoEnum) m_ipPacket.header->ip_p);

        m_dstSocketAddress.setIpAddress(
                boost::asio::ip::address::from_string(dstIPv4TmpStr));
        m_dstSocketAddress.setTransportProto(
                (transportProtoEnum) m_ipPacket.header->ip_p);

    } else if (m_ipPacket.version == 6) {

//        L_t
//        << "IPv6 not implemented.";
        return false;

    } else {

        L_t
        << "No IP data available";
        return false;
    }

    // We successful parsed the network layer.
    m_parsedNetworklayer = true;
    return true;
}

bool TlayerPacket::parseTransportlayer() {

    // UDP header
    m_udpPacket.header = (udphdr*) m_ipPacket.payload;

    // UDP header size
    m_udpPacket.headerLen = sizeof(udphdr);

    // start of UDP payload
    m_udpPacket.payload = m_ipPacket.payload + m_udpPacket.headerLen;

    // UDP payload size
    m_udpPacket.payloadLen = m_ipPacket.payloadLen - m_udpPacket.headerLen;

    // packet sanity check
    if (m_udpPacket.headerLen + m_udpPacket.payloadLen
            > m_ipPacket.payloadLen) {

        L_t
        << "Transport layer sanity check: "
                << "UDP packet larger than IP payload. Discarding packet.";
        return false;
    }

    // update SocketAddress objects with port and protocol
    m_srcSocketAddress.setPort(ntohs(m_udpPacket.header->source));
    m_srcSocketAddress.setTransportProto(tp_UDP);

    m_dstSocketAddress.setPort(ntohs(m_udpPacket.header->dest));
    m_dstSocketAddress.setTransportProto(tp_UDP);

    // We successful parsed the transport layer
    m_parsedTransportlayer = true;
    return true;
}

}
/* namespace callx */
