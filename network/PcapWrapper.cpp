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
 * PcapWrapper.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: mainka
 */

#include "main/callx.hpp"
#include "PcapWrapper.hpp"

using namespace std;

namespace callx {

PcapWrapper::PcapWrapper()
        : m_pcapHandle(0),
          m_received(0),
          m_dropped(0) {
}

PcapWrapper::~PcapWrapper() {
    if (m_pcapHandle)
        pcap_close(m_pcapHandle);
}

bool PcapWrapper::openDevice(string device) {
    m_pcapHandle = pcap_open_live(device.c_str(), Max_Capture_Bytes, 1, 1000,
            m_errBuf);
    if (m_pcapHandle == NULL) {
        L_f
        << "Couldn't open device " << device.c_str() << ": " << m_errBuf
                << endl;
        return false;
    }

    if (pcap_setnonblock(m_pcapHandle, 0, m_errBuf) == -1) {
        return false;
    }

    return true;
}

bool PcapWrapper::openFile(string path) {
    m_pcapHandle = pcap_open_offline(path.c_str(), m_errBuf);
    if (m_pcapHandle == NULL) {
        L_f
        << "Couldn't open file \"" << path.c_str() << "\": " << m_errBuf
                << endl;
        return false;
    }
    return true;
}

bool PcapWrapper::setFilter(string filter) {

// compile and apply filter
    if (pcap_compile(m_pcapHandle, &m_fp, filter.c_str(), 0, 0) == -1) {
        L_f
        << "Couldn't parse filter " << filter << ": "
                << pcap_geterr(m_pcapHandle) << endl;
        return false;
    }
    if (pcap_setfilter(m_pcapHandle, &m_fp) == -1) {
        L_f
        << "Couldn't install filter" << filter << ": "
                << pcap_geterr(m_pcapHandle) << endl;
        return false;
    }

    return true;
}

void PcapWrapper::breakLoop() {
    pcap_breakloop(m_pcapHandle);
}

string PcapWrapper::getErrorMessage() const {
    return m_errBuf;
}

} /* namespace callx */

