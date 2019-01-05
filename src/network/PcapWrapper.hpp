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
 * PcapWrapper.hpp
 *
 *  Created on: Jul 24, 2012
 *      Author: mainka
 */

#ifndef PCAPWRAPPER_HPP_
#define PCAPWRAPPER_HPP_

#include <iostream>
#include <string>
#include <pcap.h>

#define PCAP_MAX_CAPTURE_BYTES 1536

namespace callx {

class PcapWrapper {

public:
    /**
     * Standard constructor
     */
    PcapWrapper();

    /*
     * Destructor
     */
    ~PcapWrapper();

    /**
     * Open device in promiscuous mode.
     * @param device string such as eth0 or lo
     */
    bool openDevice(std::string device);

    /**
     * Open PCAP file.
     * @param path the PCAP file
     */
    bool openFile(std::string path);

    /**
     * Sets the PCAP filter (optional).
     * @param filter
     */
    bool setFilter(std::string filter);

    /**
     * Gets the pcap error message.
     * @return string object containing the pcap error message.
     */
    std::string getErrorMessage() const;

    /**
     * Gets the next packet and returns a success/failure indication.
     * The pcap method pcap_next_ex is used. Suitable: pcap_next_ex() returns 1.
     * @param header pointer to the pcap_pkthdr struct, see pcap manpage
     * @param data pointer to the data, see pcap manpage
     * @return see pcap manpage; in short: 1 ok, other not
     */
    int getNextPacket(struct pcap_pkthdr** header, u_char const** data);

    /**
     * Calls pcap_dispatch().
     * cnt is the maximum number number of packets to capture before returning.
     * It is not the minimum number, see loop(...). See 'man pcap_dispatch' for
     * details.
     * @param user
     * @param cnt
     * @param callback
     */
    int dispatch(void* user, int cnt, pcap_handler callback);

    /**
     * Calls pcap_loop, cnt is the minimum number of packets. See 'man
     * pcap_dispatch' for details.
     * @param user
     * @param cnt
     * @param callback
     */
    int loop(void* user, int cnt, pcap_handler callback);

    /**
     * Forces dispatch() or loop() to return.
     */
    void breakLoop();

    /**
     * Calls pcap_stats and stores the values of packets received / dropped
     * in member variables.
     */
    inline void readStats();

    /**
     * The packet count since first call of getNextPacket or getNextPacketRaw.
     * This is the PcapWrapper internal counter, not the pcap_stats() information.
     * @return packet count
     */
    unsigned long getCounter() const;

    /**
     * Number of received packets since start.
     * The number of received packets since call of openDevice or openFile as
     * returned by the pcap_stats().
     * @return number of received packets
     */
    unsigned long getReceived() const;

    /**
     * Number of dropped packets since start.
     * The number of dropped packets since call of openDevice or openFile as
     * returned by the pcap_stats().
     * @return number of dropped packets
     */
    unsigned long getDropped() const;

    static const u_int Max_Capture_Bytes = PCAP_MAX_CAPTURE_BYTES;

    int getLinkType();

private:
    char m_errBuf[PCAP_ERRBUF_SIZE];
    struct bpf_program m_fp; // compiled filter
    pcap_t *m_pcapHandle;
    int m_pcapLinkType;
    unsigned long m_received;
    unsigned long m_dropped;
    pcap_stat m_pcapStats;
};

// ************************************
// inline definitions -->
// ************************************

inline int PcapWrapper::getNextPacket(struct pcap_pkthdr** header,
        u_char const** data) {
    return pcap_next_ex(m_pcapHandle, header, data);
}

inline int PcapWrapper::dispatch(void* user, int cnt, pcap_handler callback) {
    return pcap_dispatch(m_pcapHandle, cnt, callback,
            reinterpret_cast<u_char*>(user));
}

inline int PcapWrapper::loop(void* user, int cnt, pcap_handler callback) {
    return pcap_loop(m_pcapHandle, cnt, callback,
            reinterpret_cast<u_char*>(user));
}

inline void PcapWrapper::readStats() {
    pcap_stats(m_pcapHandle, &m_pcapStats);
    m_received = m_pcapStats.ps_recv;
    m_dropped = m_pcapStats.ps_drop;
}

inline unsigned long PcapWrapper::getReceived() const {
    return m_received;
}

inline unsigned long PcapWrapper::getDropped() const {
    return m_dropped;
}

} /* namespace callx */

#endif /* PCAPWRAPPER_HPP_ */
