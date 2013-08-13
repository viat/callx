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
 * PcapNetHandler.hpp
 *
 *  Created on: Jul 24, 2012
 *      Author: mainka
 */

#ifndef PCAPHANDLER_HPP_
#define PCAPHANDLER_HPP_

#include <boost/thread.hpp>

#include "PcapWrapper.hpp"
#include "main/CallxThread.hpp"
#include "container/PcapPacketQueue.hpp"

#include "container/TlayerPacketQueue.hpp"
#include "network/TlayerPacket.hpp"
#include "network/TlayerPacketRecycler.hpp"

namespace callx {

class PcapHandler:
        public CallxThread {

public:

    /**
     * Standard constructor
     */
    PcapHandler();

    /**
     * Destructor
     */
    ~PcapHandler();

    /**
     * The static callback method that is used by libpcap's pcap_dispatch()
     * and pcap_loop()
     */
    inline static void staticCallback(u_char *instance,
            const struct pcap_pkthdr *header,
            const u_char *data) {
        ((PcapHandler*) instance)->callback(header, data);
    }

    /**
     * Stop LIBPCAP's loop or dispatch and call CallxThread::stop() afterwards.
     */
    bool stop();

    /**
     * The worker method, that is started as thread.
     */
    void worker();

private:

    /**
     * The dispatch method, that is called from staticCallback().
     */
    void callback(const struct pcap_pkthdr *header, const u_char *data);

    TlayerPacketQueue *m_tlayerPacketQueue;
    PcapPacketQueue *m_pcapPacketQueue;
    std::unique_ptr<TlayerPacket, TlayerPacketRecycler> m_tlayerPacket;
    PcapWrapper m_pcapWrapper;
};

} /* namespace callx */

#endif /* PCAPHANDLER_HPP_ */
