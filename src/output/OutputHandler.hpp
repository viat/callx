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
 * OutputHandler.hpp
 *
 *  Created on: Dec 15, 2012
 *      Author: mainka
 */

#ifndef OUTPUTHANDLER_HPP_
#define OUTPUTHANDLER_HPP_

#include "main/CallxThread.hpp"
#include "output/db/ViatDb.hpp"
#include <boost/asio.hpp>

namespace callx {

class PcmAudioQueue;
class ViatDb;
class PcmAudio;

class OutputHandler:
        public CallxThread {
public:
    OutputHandler();
    virtual ~OutputHandler();

    /**
     * Overrides method CallxThread::stop() because PcmAudioQueue has
     * to be disabled before stooping.
     */
    bool stop();

    /**
     * Implementation of abstract method CallxThread::worker()
     */
    void worker();

private:
    void pushToFile();
    void pushToSocket();

    PcmAudioQueue *m_pcmAudioQueue;
    std::unique_ptr<PcmAudio> m_currPcmAudio;
    long m_dbCallId;
    std::string m_uniqueFilename;
    std::unique_ptr<ViatDb> m_viatDb;
    const static short m_startSep[4];
    const static short m_endSep[4];

    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::endpoint m_tcpServer;
    std::shared_ptr<boost::asio::ip::tcp::socket> m_tcpSocket;

};

} /* namespace callx */
#endif /* OUTPUTHANDLER_HPP_ */
