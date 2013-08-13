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
 * CommandServer.cpp
 *
 *  Created on: Jan 2, 2013
 *      Author: mainka
 */

#include <sstream>
#include "CommandServer.hpp"
#include "container/TlayerPacketQueue.hpp"
#include "container/PcapPacketQueue.hpp"
#include "container/UdpPacketQueue.hpp"
#include "container/SipPacketQueue.hpp"
#include "container/CallMap.hpp"
#include "container/RtpSinkMap.hpp"
#include "container/SbaEventMap.hpp"
#include "container/CallDecodeQueue.hpp"
#include "container/PcmAudioQueue.hpp"
#include "container/SbaIncidentMap.hpp"
#include "audio/AudioHandler.hpp"

using namespace std;

namespace callx {

const string CommandServer::welcomeStr = //
        "********************\r\n"//
                "* callx v2 console *\r\n"//
                "********************\r\n\r\n";
const string CommandServer::promptStr = "callx> ";

CommandServer::CommandServer() {
    L_t
    << "C'tor";
    m_tlayerPacketQueue = TlayerPacketQueue::getInstance();
    m_pcapPacketQueue = PcapPacketQueue::getInstance();
    m_udpPacketQueue = UdpPacketQueue::getInstance();
    m_sipPacketQueue = SipPacketQueue::getInstance();
    m_callMap = CallMap::getInstance();
    m_rtpSinkMap = RtpSinkMap::getInstance();
    m_sbaEventMap = SbaEventMap::getInstance();
    m_callDecodeQueue = CallDecodeQueue::getInstance();
    m_pcmAudioQueue = PcmAudioQueue::getInstance();
    m_sbaIncidentMap = SbaIncidentMap::getInstance();
}

CommandServer::~CommandServer() {
    L_t
    << "D'tor";
}

string CommandServer::welcome() const {
    return welcomeStr;
}

string CommandServer::prompt() const {
    return promptStr;
}

string CommandServer::listContainer() const {
    stringstream sstream;
    sstream << "\r\n"
            << "TlayerPacketQueue\t(cur / max): "
            << m_tlayerPacketQueue->size()
            << " / "
            << m_tlayerPacketQueue->sizeMax()
            << "\r\n"

            << "PcapPacketQueue\t\t(cur / max): "
            << m_pcapPacketQueue->size()
            << " / "
            << m_pcapPacketQueue->sizeMax()
            << "\r\n"

            << "UdpPacketQueue\t\t(cur / max): "
            << m_udpPacketQueue->size()
            << " / "
            << m_udpPacketQueue->sizeMax()
            << "\r\n"

            << "SipPacketQueue\t\t(cur / max): "
            << m_sipPacketQueue->size()
            << " / "
            << m_sipPacketQueue->sizeMax()
            << "\r\n"

            << "CallMap\t\t\t(cur / max): "
            << m_callMap->size()
            << " / "
            << m_callMap->sizeMax()
            << "\r\n"

            << "CallDecodeQueue\t\t(cur / max): "
            << m_callDecodeQueue->size()
            << " / "
            << m_callDecodeQueue->sizeMax()
            << "\r\n"

            << "RtpSinkMap\t\t(cur / max): "
            << m_rtpSinkMap->size()
            << " / "
            << m_rtpSinkMap->sizeMax()
            << "\r\n"

            << "PcmAudioQueue\t\t(cur / max): "
            << m_pcmAudioQueue->size()
            << " / "
            << m_pcmAudioQueue->sizeMax()
            << "\r\n"

            << "SbaEventMap\t\t(cur / max): "
            << m_sbaEventMap->size()
            << " / "
            << m_sbaEventMap->sizeMax()
            << "\r\n"

            << "SbaIncidentMap\t\t(cur / max): "
            << m_sbaIncidentMap->size()
            << " / "
            << m_sbaIncidentMap->sizeMax()
            << "\r\n"

            << "RtpSeqNumError: "
            << AudioHandler::rtpSeqNumError
            << "\r\n"

            << "\r\n";
    return sstream.str();
}

std::string CommandServer::listCallMap() const {
    return m_callMap->toString();
}

std::string CommandServer::listSbaEventMap() const {
    return m_sbaEventMap->toString();
}

std::string CommandServer::listSbaIncidentMap() const {
    return m_sbaIncidentMap->toString();
}

std::string CommandServer::clearSba() const {
    m_sbaEventMap->eraseAll();
    m_sbaIncidentMap->eraseAll();
    return "\r\nSbaEventMap and SbaIncidentMap are now empty.\r\n\r\n";
}

} /* namespace callx */
