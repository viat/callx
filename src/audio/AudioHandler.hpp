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
 * AudioDecoder.hpp
 *
 *  Created on: Dec 9, 2012
 *      Author: mainka
 */

#ifndef AUDIOHandler_HPP_
#define AUDIOHandler_HPP_

#include "main/CallxThread.hpp"
#include "main/CallxTypes.hpp"

namespace callx {

enum rtpPayloadTypeEnum {
    pt_PCMU = 0,
    pt_GSM = 3,
    pt_PCMA = 8,
    pt_UNDEFINED = 127
};

class CallDecodeQueue;
class Call;
class RtpSink;
class AudioDecoderInterface;
class PcmAudio;
class PcmAudioQueue;
class TlayerPacketQueue;

typedef std::map<rtpPayloadTypeEnum, AudioDecoderInterface*> AudioDecoderMap;

/**
 * Audio Decoder Thread
 */
class AudioHandler:
        public callx::CallxThread {
public:
    AudioHandler();
    virtual ~AudioHandler();
    bool stop();
    void worker();

    static u_int rtpSeqNumError;

protected:
    void decodeRtpPacketDeque();

    CallDecodeQueue *m_callDecodeQueue;
    AudioDecoderMap m_audioDecoderMap;
    PcmAudioQueue* m_pcmAudioQueue;
    TlayerPacketQueue* m_tlayerPacketQueue;
    std::unique_ptr<TlayerDeque> m_rtpPacketDeque;

    std::shared_ptr<Call> m_currCall;
    std::shared_ptr<RtpSinkMapType> m_currRtpSinkMap;
    std::shared_ptr<RtpSink> m_currRtpSink;
};

} /* namespace callx */
#endif /* AUDIOHandler_HPP_ */
