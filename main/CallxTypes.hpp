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
 * CallxTypes.hpp
 *
 *  Created on: Oct 29, 2012
 *      Author: mainka
 */

#ifndef CALLXTYPES_HPP_
#define CALLXTYPES_HPP_

#include <boost/chrono.hpp>
#include <queue>
#include <list>
#include <set>
#include <thread>
#include <map>
#include "boost/thread.hpp"

namespace callx {

class RtpSink;
class SocketAddress;
class TlayerPacket;
class TlayerPacketRecycler;
class SbaEvent;
class SbaIncident;

// we use std::*
//typedef boost::mutex mutex;
//typedef boost::lock_guard<mutex> lock_guard;
//typedef boost::unique_lock<mutex> unique_lock;
//typedef boost::condition_variable condition_variable;

typedef std::mutex mutex;
typedef std::lock_guard<mutex> lock_guard;
typedef std::unique_lock<mutex> unique_lock;
typedef std::condition_variable condition_variable;

typedef boost::chrono::system_clock systemClock;
typedef boost::chrono::steady_clock steadyClock;
typedef boost::chrono::high_resolution_clock hresClock;

typedef boost::chrono::microseconds microseconds;
typedef boost::chrono::milliseconds milliseconds;
typedef boost::chrono::seconds seconds;
typedef boost::chrono::minutes minutes;

typedef std::map<SocketAddress, std::shared_ptr<RtpSink> > RtpSinkMapType;
typedef std::deque<std::unique_ptr<TlayerPacket, TlayerPacketRecycler>> TlayerDeque;
typedef std::deque<std::shared_ptr<SbaEvent>> SbaEventDeque;

typedef std::set<std::shared_ptr<SbaIncident>> SbaIncidentSet;

struct SipFromTo {
    std::string tag;
    std::string address;
    std::string displayname;
};



} /* namespace callx */

#endif /* CALLXTYPES_HPP_ */
