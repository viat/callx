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
 * CommandServer.hpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#ifndef COMMANDSERVER_HPP_
#define COMMANDSERVER_HPP_

namespace callx {

class PcapPacketQueue;
class UdpPacketQueue;
class SipPacketQueue;
class CallMap;
class RtpSinkMap;
class SbaEventMap;
class CallDecodeQueue;
class PcmAudioQueue;
class SbaIncidentMap;
class TlayerPacketQueue;

class CommandServer {
public:
    CommandServer();
    virtual ~CommandServer();

    std::string welcome() const;
    std::string prompt() const;
    std::string listContainer() const;
    std::string listCallMap() const;
    std::string listSbaEventMap() const;
    std::string listSbaIncidentMap() const;
    std::string clearSba() const;

    static const std::string welcomeStr;
    static const std::string promptStr;

private:
    PcapPacketQueue *m_pcapPacketQueue;
    UdpPacketQueue *m_udpPacketQueue;
    SipPacketQueue *m_sipPacketQueue;
    CallMap *m_callMap;
    RtpSinkMap *m_rtpSinkMap;
    SbaEventMap *m_sbaEventMap;
    CallDecodeQueue *m_callDecodeQueue;
    PcmAudioQueue *m_pcmAudioQueue;
    SbaIncidentMap *m_sbaIncidentMap;
    TlayerPacketQueue *m_tlayerPacketQueue;
};


} /* namespace callx */
#endif /* COMMANDSERVER_HPP_ */
