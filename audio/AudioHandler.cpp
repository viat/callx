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
 * AudioDecoder.cpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mainka
 */

#include "AudioHandler.hpp"

#include "main/CallxTypes.hpp"

#include "sip/Call.hpp"

#include "container/CallDecodeQueue.hpp"
#include "container/PcmAudioQueue.hpp"
#include "container/TlayerPacketQueue.hpp"

#include "network/SocketAddress.hpp"
#include "network/TlayerPacket.hpp"
#include "network/TlayerPacketRecycler.hpp"

#include "RtpSink.hpp"
#include "AudioDecoderInterface.hpp"
#include "G711Decoder.hpp"
#include "GsmDecoder.hpp"
#include "PcmAudio.hpp"
#include "MemChunk.hpp"

using namespace std;

namespace callx {

u_int AudioHandler::rtpSeqNumError = 0;

AudioHandler::AudioHandler()
        : m_callDecodeQueue(CallDecodeQueue::getInstance()),
          m_pcmAudioQueue(PcmAudioQueue::getInstance()),
          m_tlayerPacketQueue(TlayerPacketQueue::getInstance()) {

    L_t
    << "C'tor";
    classname = "AudioHandler";

    // instantiating audio decoder objects
    m_audioDecoderMap[pt_PCMU] = new G711Decoder(false);
    m_audioDecoderMap[pt_PCMA] = new G711Decoder(true);

    // Currently not supported (bug...) (20130613)
    //m_audioDecoderMap[pt_GSM] = new GsmDecoder();
}

AudioHandler::~AudioHandler() {
    L_t
    << "D'tor";
}

bool AudioHandler::stop() {
    L_t
    << "Overridden stop() has been called, deactivating CallDecodeQueue.";
    m_callDecodeQueue->deactivate();

    return CallxThread::stop(); // calling parent stop()
}

void AudioHandler::worker() {

    while (!m_stopRequested) {

        // get next call
        if (!m_callDecodeQueue->waitAndPop(m_currCall)) {

            // We are here, because the CallDecodeQueue has been deactivated.
            L_t
            << "wait_and_pop() results false -> CallDecodeQueue" << "is deativated.";
            L_t
            << "m_stopRequested: " << m_stopRequested;
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            continue;
        }

        // get the local RtpSinkMap object
        m_currRtpSinkMap = m_currCall->getRtpSinkMap();
        L_t
        << "Local RtpSinkMap size: " << m_currRtpSinkMap->size();

        // iterate over the RtpSinkMap and process every RtpSink object
        for (auto sinkMapIter = m_currRtpSinkMap->begin();
                sinkMapIter != m_currRtpSinkMap->end(); sinkMapIter++) {

            m_currRtpSink = sinkMapIter->second;

            // This queue contains the TlayerPackets (RTP) waiting to be
            // decoded.
            m_rtpPacketDeque = m_currRtpSink->getTlayerDeque();

            decodeRtpPacketDeque();
        }
    }
    L_t
    << "Stopped the worker because stop request is true.";
    m_stopped = true;
}

void AudioHandler::decodeRtpPacketDeque() {
    unique_ptr<TlayerPacket, TlayerPacketRecycler> tlayerPacket;
    rtpPayloadTypeEnum rtpPayloadType;
    u_char* rtpPayloadData;
    size_t rtpPayloadSize;
    u_short rtpSeqNum = 0;
    u_short rtpSeqNumPrev = 0;
    auto memChunk = make_shared<MemChunk>(m_callxConfig->mem_chunk_size);
    void* buffer = NULL;

    unique_ptr<PcmAudio> pcmAudio(
            new PcmAudio(m_currCall->getDialog()->callId,
                    m_currCall->getDialog()->caller,
                    m_currCall->getDialog()->callee,
                    m_currCall->getCreationTs(),
                    m_currRtpSink->isCallerSink()));
    AudioDecoderInterface *audioDecoder;

    // decode packet by packet...
    for (auto iter = m_rtpPacketDeque->begin(); iter != m_rtpPacketDeque->end();
            iter++) {

        // getting ownership of TlayerPacket object (unique_ptr)
        tlayerPacket = move(*iter);

        // RTP header & payload
        rtpPayloadType = static_cast<rtpPayloadTypeEnum>(tlayerPacket
                ->m_udpPacket.payload[1] & 0x7F); // without marker-bit


        // Currently not supported (bug...)! (20130614)
        if(rtpPayloadType == pt_GSM) {
            L_t << "GSM is currently not supported.";
            continue;
        }

        rtpSeqNum = (tlayerPacket->m_udpPacket.payload[2] << 8)
                | tlayerPacket->m_udpPacket.payload[3];
        rtpPayloadData = tlayerPacket->m_udpPacket.payload + 12;
        rtpPayloadSize = tlayerPacket->m_udpPacket.payloadLen - 12;

        // DEBUG
        if (rtpSeqNumPrev) {
            if (rtpSeqNum != rtpSeqNumPrev + 1) {
                AudioHandler::rtpSeqNumError++;
                L_t << "seq num error, rtpSeqNumPrev="
                     << rtpSeqNumPrev
                     << " rtpSeqNum="
                     << rtpSeqNum
                     << endl;
            }
        }

        if (rtpPayloadType != pt_PCMU && rtpPayloadType != pt_PCMA
                && rtpPayloadType != pt_GSM) {
            L_t
            << "RTP payloadtype not supported: " << rtpPayloadType;
            continue;
        }

        // get audio decoder
        audioDecoder = m_audioDecoderMap[rtpPayloadType];

        // RTP payload has to be greater than or equal to the decoder input
        // buffer size.
        if (audioDecoder->inBufSize > rtpPayloadSize) {
            L_e
            << "RTP payload smaller than decoder input buffer.";
            continue;
        }

        // get pointer to store decoder output
        if (!memChunk->declare(audioDecoder->outBufSize, &buffer)) {

            // memory chunk seems to be full, push current memory chunk
            pcmAudio->addChunk(memChunk);

            // create new memory chunk
            memChunk = make_shared<MemChunk>(m_callxConfig->mem_chunk_size);
            if (!memChunk->declare(audioDecoder->outBufSize, &buffer)) {
                L_f
                << "Not able to store decoded audio in memory chunk. " << "Memory chunk too small?";
                exit(-1);
            }
        }

        // decode
        audioDecoder->decode(rtpPayloadData, buffer);

        // Push TlayerPacket back to the TlayerPacketQueue. Is this faster
        // than relying on the TlayerPacketRecycler?
        m_tlayerPacketQueue->push(tlayerPacket);
    }

    // push last memory chunk if it is not empty
    if (memChunk->fillingLevel() > 0)
        pcmAudio->addChunk(memChunk);

    // push PcmAudio object into PcmAudioQueue if not empty
    if (pcmAudio->getChunkDeque()->size() > 0)
        m_pcmAudioQueue->push(pcmAudio);
}

} /* namespace callx */
