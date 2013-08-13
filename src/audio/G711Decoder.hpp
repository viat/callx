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
 * G711Decoder.hpp
 *
 *  Created on: Aug 28, 2012
 *      Author: mainka
 */

#ifndef G711DECODER_HPP_
#define G711DECODER_HPP_

#include "AudioDecoderInterface.hpp"

namespace callx {

class G711Decoder: public AudioDecoderInterface {
public:
    G711Decoder(bool alaw);
    ~G711Decoder();
    void decode(void* inBuf, void* outBuf);

private:
    const short* decodeTab;
    static const short alawDecodeTab[256];
    static const short ulawDecodeTab[256];

};

} /* namespace callx */

#endif /* G711DECODER_HPP_ */
