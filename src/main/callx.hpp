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
 * Callx.hpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#define BOOST_ALL_DYN_LINK

#ifndef CALLX_HPP_
#define CALLX_HPP_

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes.hpp>

#include <string>
#include "CallxTypes.hpp"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

#define L_t BOOST_LOG_TRIVIAL(trace)   << "[t] [" << __FILE__ << ":" << __LINE__ << "] "
#define L_d BOOST_LOG_TRIVIAL(debug)   << "[d] [" << __FILE__ << ":" << __LINE__ << "] "
#define L_i BOOST_LOG_TRIVIAL(info)    << "[i] [" << __FILE__ << ":" << __LINE__ << "] "
#define L_w BOOST_LOG_TRIVIAL(warning) << "[w] [" << __FILE__ << ":" << __LINE__ << "] "
#define L_e BOOST_LOG_TRIVIAL(error)   << "[e] [" << __FILE__ << ":" << __LINE__ << "] "
#define L_f BOOST_LOG_TRIVIAL(fatal)   << "[f] [" << __FILE__ << ":" << __LINE__ << "] "

namespace callx {


inline hresClock::time_point now() {
    return hresClock::now();
}

inline microseconds intervalUs(const hresClock::time_point& t1,
        const hresClock::time_point& t0) {
    return boost::chrono::duration_cast<microseconds>(t1 - t0);
}

inline milliseconds intervalMs(const hresClock::time_point& t1,
        const hresClock::time_point& t0) {
    return boost::chrono::duration_cast<milliseconds>(t1 - t0);
}

class StopWatch {
    hresClock::time_point start_;
public:
    inline StopWatch()
            : start_(hresClock::now()) {
    }
    inline hresClock::time_point restart() {
        start_ = hresClock::now();
        return start_;
    }
    inline microseconds elapsedUs() {
        return intervalUs(now(), start_);
    }
    inline milliseconds elapsedMs() {
        return intervalMs(now(), start_);
    }
};

template<typename T>
T StringToNumber(const std::string &Text) {
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

template<typename T>
std::string NumberToString(T Number) {
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

template<typename Type, typename Deleter = std::default_delete<Type>,
        typename ... Args>
auto make_unique(Args&&... args) -> std::unique_ptr<Type, Deleter> {
    return std::unique_ptr<Type, Deleter> { new Type {
            std::forward<Args>(args)... } };
}

//template<typename Type, typename ... Args>
//auto make_unique(Args&&... args) -> std::unique_ptr<Type> {
//    return std::unique_ptr<Type> {
//        new Type {std::forward<Args>(args)...}
//    };
//}

} /* namespace callx */

#endif /* CALLX_HPP_ */
