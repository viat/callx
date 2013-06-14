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
 * OutputHandler.cpp
 *
 *  Created on: Dec 15, 2012
 *      Author: mainka
 */

#include "OutputHandler.hpp"
#include "container/PcmAudioQueue.hpp"
#include "output/filesystem/WaveFileWriter.hpp"
#include "audio/PcmAudio.hpp"
#include "audio/MemChunk.hpp"
#include "boost/thread.hpp"

using namespace std;

namespace callx {

const short OutputHandler::m_startSep[] = { 1, 10, 100, 1000 };
const short OutputHandler::m_endSep[] = { 1000, 100, 10, 1 };

OutputHandler::OutputHandler()
        : m_pcmAudioQueue(PcmAudioQueue::getInstance()),
          m_dbCallId(-1) {
    L_t
            << "C'tor";
    classname = "Output";

    // Socket output: create ViatDb object and TCP socket (if requested in config)
    if (m_callxConfig->use_viat_db) {

        m_viatDb.reset(new ViatDb(m_callxConfig->viat_db_connect_str));

        if (m_callxConfig->use_socket_output_interface) {

            m_tcpServer.address(boost::asio::ip::address::from_string(m_callxConfig
                    ->socket_output_remote_ip));
            m_tcpServer.port(m_callxConfig->socket_output_remote_port);
            m_tcpSocket =
                    std::make_shared<boost::asio::ip::tcp::socket>(m_ioService);
        }
    }
}

OutputHandler::~OutputHandler() {
    L_t
            << "D'tor";
}

bool OutputHandler::stop() {
    L_t
            << "Overridden stop() has been called, deactivating PcmAudioQueue.";
    m_pcmAudioQueue->deactivate();
    return CallxThread::stop();
}

void OutputHandler::worker() {

    while (!m_stopRequested) {

        // get next PcmAudio object
        if (!m_pcmAudioQueue->waitAndPop(m_currPcmAudio)) {

            // The PcmAudioQueue has been deactivated.
            L_t
                    << "wait_and_pop() results false -> PcmAudioQueue has been "
                    << "deactivated.";
            L_t
                    << "m_stopRequested: "
                    << m_stopRequested;

            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            continue;
        }

        // Skip if the PcmAudio object is empty.
        if (m_currPcmAudio->getChunkDeque()->size() == 0) {
            L_t
                    << "PcmAudio object is empty.";
            continue;
        }

        // If use_viat_db is true, create caller and call entries in the DB.
        if (m_callxConfig->use_viat_db) {
            m_dbCallId = m_viatDb->insertCall(m_currPcmAudio->getCaller()
                    .displayname,
                    m_currPcmAudio->getCaller().address);

            // Skip in case of database error.
            if (m_dbCallId == -1) {
                L_e
                        << "Error creating database entry.";
                continue;
            }

            L_i
                    << "DB entry created, DB Call ID: "
                    << m_dbCallId;
        }

        // wave file output
        if (m_callxConfig->use_wavefile_output_interface) {
            pushToFile();
        }

        // output to feature extractor (if use_viat_db is true)
        if (m_callxConfig->use_socket_output_interface
                && m_callxConfig->use_viat_db) {
            pushToSocket();
        }
        m_currPcmAudio.reset();
    } // main loop

    if (m_tcpSocket && m_tcpSocket->is_open())
        m_tcpSocket->close();
    L_t
            << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

void OutputHandler::pushToFile() {

    // generate filename and create WaveFileWriter object
    string fileName = m_callxConfig->wave_output_path;
    if (m_callxConfig->use_viat_db) {
        fileName += NumberToString(m_dbCallId);
    } else {
        fileName += m_currPcmAudio->getCallId();
    }
    if (m_currPcmAudio->isCallerAudio()) {
        fileName += "_caller";
    } else {
        fileName += "_callee";
    }
    fileName += ".wav";
    WaveFileWriter waveFileWriter(fileName);

    // get the chunk queue that contains PCM data
    auto currMemChunkDeque = m_currPcmAudio->getChunkDeque();

    // write data
    for (auto iter = currMemChunkDeque->begin();
            iter != currMemChunkDeque->end(); iter++) {
        waveFileWriter.write((*iter)->data(), (*iter)->fillingLevel());
    }
}

void OutputHandler::pushToSocket() {

    boost::system::error_code ec;

    // max bytes = seconds * sample rate * 2 bytes
    u_int maxBytes = m_callxConfig->socket_output_send_seconds * 8000 * 2;
    u_int sentBytes = 0;
    u_int bytesToSend = 0;

    L_t
            << "maxBytes: "
            << maxBytes;

    // get the chunk queue that contains PCM data
    auto currMemChunkDeque = m_currPcmAudio->getChunkDeque();

    // send data (Call ID)
    sentBytes = 0;
    u_long dbCallIdFeaturex = m_dbCallId;

    int connectCount = 0;

    // label for reconnect in case of a broken connection
    connect:

    connectCount++;
    try {
        if (!m_tcpSocket->is_open()) {
            m_tcpSocket->connect(m_tcpServer);
            L_i
                    << "TCP client, local / remote endpoint: "
                    << m_tcpSocket->local_endpoint()
                    << " / "
                    << m_tcpSocket->remote_endpoint();
        }

    } catch (std::exception& e) {
        if (m_tcpSocket->is_open())
            m_tcpSocket->close();
        L_f
                << "Connection to Feature Extractor failed: "
                << e.what();

        if (connectCount > 2)
            return;

        goto connect;
    }

    try {

        sentBytes += m_tcpSocket->write_some(boost::asio::buffer(m_startSep, 8),
                ec);
        if (ec) {
            L_f
                    << "TCP client: error writing start separator to socket: "
                    << ec;

            if (m_tcpSocket->is_open())
                m_tcpSocket->close();

            if (connectCount > 2)
                return;

            goto connect;

        } else {
            L_t
                    << "TCP client: sent start separator to socket, "
                    << sentBytes
                    << " bytes.";
        }

        sentBytes =
                m_tcpSocket->write_some(boost::asio::buffer(&dbCallIdFeaturex,
                        sizeof(long)),
                        ec);

        if (ec) {
            L_t
                    << "TCP client: error writing Call ID to socket: "
                    << ec;

            if (m_tcpSocket->is_open())
                m_tcpSocket->close();

            if (connectCount > 2)
                return;

            goto connect;

        } else {
            L_t
                    << "TCP client: "
                    << sentBytes
                    << " bytes sent (Call ID)";
        }

        // send PCM data
        sentBytes = 0;
        bytesToSend = 0;
        bool last = false;
        for (auto iter = currMemChunkDeque->begin();
                iter != currMemChunkDeque->end(); iter++) {
            auto memChunk = *iter;

            // number of bytes to send
            if (memChunk->fillingLevel() <= (maxBytes - sentBytes)) {
                bytesToSend = memChunk->fillingLevel();
            } else {
                bytesToSend = maxBytes - sentBytes;
                last = true;
            }

            sentBytes += m_tcpSocket->write_some(boost::asio::buffer(memChunk
                    ->data(),
                    bytesToSend),
                    ec);

            if (ec) {
                L_f
                        << "TCP client: error writing PCM data to socket: "
                        << ec;

                if (m_tcpSocket->is_open())
                    m_tcpSocket->close();
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                break;
            } else {
                L_t
                        << "TCP client: "
                        << sentBytes
                        << " bytes sent (cumulative)";
            }

            if (last || maxBytes - sentBytes == 0)
                break;
        }

        L_i
                << "TCP client: data bytes (samples) sent: "
                << sentBytes
                << " ("
                << sentBytes / 2
                << ")";

        sentBytes = m_tcpSocket->write_some(boost::asio::buffer(m_endSep, 8),
                ec);

        if (ec) {
            L_f
                    << "TCP client: error writing end separator to socket: "
                    << ec;

            if (m_tcpSocket->is_open())
                m_tcpSocket->close();
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            return;
        } else {
            L_t
                    << "TCP client: sent end separator to socket, "
                    << sentBytes
                    << " bytes.";
        }

    } catch (std::exception& e) {
        L_f
                << "Error sending data to Feature Extractor ("
                << e.what()
                << ").";

        if (m_tcpSocket->is_open())
            m_tcpSocket->close();
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        return;
    }
}

} /* namespace callx */
