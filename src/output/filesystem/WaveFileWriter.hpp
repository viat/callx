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
 * WaveFileWriter.hpp
 *
 *  Created on: Aug 28, 2012
 *      Author: mainka
 */

#ifndef WAVEFILEWRITER_HPP_
#define WAVEFILEWRITER_HPP_

#include "RawFileWriter.hpp"

namespace callx {

class WaveFileWriter: public RawFileWriter {
public:
  WaveFileWriter(const std::string& filename);
  ~WaveFileWriter();

private:

  struct WaveHeader {
    // RIFF-WAVE (rw) Chunk
    u_int32_t rwChunkID; // "RIFF"
    u_int32_t rwChunkSize; // this chunk size - 8 [rest of this chunk] + fChunkSize + 8 + dChunkSize + 8
    u_int32_t rwRiffType; // "WAVE"

    // Format (f) Chunk
    u_int32_t fChunkId; // "fmt"
    u_int32_t fChunkSize; // this chunk size - 8 [rest of this chunk]
    u_int16_t fFormat; // PCM = 1
    u_int16_t fNumChannels; // 1 mono, 2 stereo
    u_int32_t fSampleRate; // samples per second
    u_int32_t fByteRate; // bytes per second
    u_int16_t fBlockAlign; // size per sample: 1 or 2 bytes
    u_int16_t fBitsPerSample; // 8, 16, ...

    // Data (d) Chunk
    u_int32_t dChunkId; // "data"
    u_int32_t dChunkSize; // NumSamples * NumChannels * BitsPerSample/8
  };

  WaveHeader waveHeader;

  // defines the strings "RIFF", "WAVE", "fmt" and "data" as 4 byte values
  // used in Wave Header
  static const u_int32_t whdr_RIFF = 0x46464952;  // RIFF
  static const u_int32_t whdr_WAVE = 0x45564157;  // WAVE
  static const u_int32_t whdr_fmt  = 0x20746D66;  // fmt
  static const u_int32_t whdr_data = 0x61746164;  // data
};

} /* namespace callx */

#endif /* WAVEFILEWRITER_HPP_ */
