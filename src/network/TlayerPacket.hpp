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
 * TlayerPacket.hpp
 *
 *  Created on: Oct 10, 2012
 *      Author: mainka
 */

#ifndef TLAYERPACKET_HPP_
#define TLAYERPACKET_HPP_

#include "SocketAddress.hpp"
#include "config/CallxConfig.hpp"

#include <sys/types.h>
#include <pcap.h>

#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#include "network/PcapWrapper.hpp"

namespace callx {

struct PcapPacket {
	pcap_pkthdr header;
	u_char payload[PcapWrapper::Max_Capture_Bytes];
	size_t payloadLen;
};

struct GenericPacket {
	void* header;
	size_t headerLen;
	u_char* payload;
	size_t payloadLen;
};

struct EthernetPacket: GenericPacket {
	ether_header* header;
};

struct IpPacket: GenericPacket {
	ip* header;
	u_short version;
};

struct UdpPacket: GenericPacket {
	udphdr* header;
};

struct TcpPacket: GenericPacket {
	tcphdr* header;
};

class TlayerPacket {
public:

	TlayerPacket();

	virtual ~TlayerPacket();

	/**
	 * Fill in the pcap data
	 * @param pcapHeader
	 * @param pcapPacket
	 */
	void fill(pcap_pkthdr pcapHeader, const u_char *pcapPacket);

	/**
	 * Parse Network Layer (IP).
	 */
	bool parseNetlayer();

	/**
	 * Parse Transport Layer.
	 */
	bool parseTransportlayer();

	/**
	 * PCAP packet
	 */
	PcapPacket m_pcapPacket;

	/**
	 * PCAP data, link layer
	 */
	EthernetPacket m_ethernetPacket;

	/**
	 * Network Layer packet (IP)
	 */
	IpPacket m_ipPacket;

	/**
	 * Transport Layer packet (UDP)
	 */
	UdpPacket m_udpPacket;

	/**
	 * Transport Layer packet (TCP)
	 */
	TcpPacket m_tcpPacket;

	/**
	 * IP and port, protocol of the source
	 */
	SocketAddress m_srcSocketAddress;

	/**
	 * IP and port, protocol of the destination
	 */
	SocketAddress m_dstSocketAddress;

	/**
	 * Information if Networklayer has beeen successful parsed.
	 */
	bool m_parsedNetworklayer;

	/**
	 * Information if Transportlayer has beeen successful parsed.
	 */
	bool m_parsedTransportlayer;

private:

	char srcIPv4TmpStr[INET_ADDRSTRLEN];
	char dstIPv4TmpStr[INET_ADDRSTRLEN];

};

} /* namespace callx */
#endif /* TLAYERPACKET_HPP_ */
