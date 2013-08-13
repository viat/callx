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
 * WaveFileWriter.cpp
 *
 *  Created on: Aug 28, 2012
 *      Author: mainka
 */


#include "main/callx.hpp"
#include "WaveFileWriter.hpp"

using namespace std;

namespace callx {

WaveFileWriter::WaveFileWriter(const std::string& filename) {
    fwrite(&waveHeader, sizeof(WaveHeader), 1, outFile);
    setFilename(filename);
}

WaveFileWriter::~WaveFileWriter() {
    L_t << "WaveFileWriter destructor, writing WAVE header.";
    waveHeader.rwChunkID = whdr_RIFF;
    waveHeader.rwChunkSize = 36 + totalSize;
    waveHeader.rwRiffType = whdr_WAVE;
    waveHeader.fChunkId = whdr_fmt;
    waveHeader.fChunkSize = 16;
    waveHeader.fFormat = 1;
    waveHeader.fNumChannels = 1;
    waveHeader.fSampleRate = 8000;
    waveHeader.fByteRate = 16000;
    waveHeader.fBlockAlign = 2;
    waveHeader.fBitsPerSample = 16;
    waveHeader.dChunkId = whdr_data;
    waveHeader.dChunkSize = totalSize;
    rewind(outFile);
    fwrite(&waveHeader, sizeof(WaveHeader), 1, outFile);
}

} /* namespace callx */
