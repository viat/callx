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
 * CallxConfig.hpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#ifndef CALLXCONFIG_HPP_
#define CALLXCONFIG_HPP_

#include "Config.hpp"
#include "main/CallxSingleton.hpp"
#include "main/callx.hpp"
#include <string>

namespace callx {

class CallxConfig:
        public CallxSingleton<CallxConfig> {

    friend class CallxSingleton<CallxConfig> ;

public:

    void init(std::string configFile);

    // path where the logfile will be stored
    std::string log_file;

    // log level [1..6] (fatal=1, error, warning, info, debug, trace)
    int log_level;

    // PID file
    std::string pid_file;

    // Daemonize yes/no
    bool daemonize;

    // network device for pcap sniffing
    std::string pcap_device;

    // filter for pcap, e.g. "udp"
    std::string pcap_filter;

    // filter small packets in sip test
    int min_sip_size;

    // TlayerPacketQueue repository size (how many TlayerPacket obejects are
    // created at startup. 1.000.000 need 1,8 GB of memory).
    int tp_repository_size;

    // memory chunk size used by the decoder for PCM data
    int mem_chunk_size;

    // SBA pause between runs in seconds
    int sba_pause;

    // only record the audiostream if there is an incident for this caller
    bool record_if_incident_only;

    // Record audio streams of the caller.
    bool record_caller;

    // Record audio streams of the callee.
    bool record_callee;

    // wavefile output interface
    bool use_wavefile_output_interface;

    // path for output files
    std::string wave_output_path;

    // socket output interface
    bool use_socket_output_interface;

    // IP of feature extractor
    std::string socket_output_remote_ip;

    // port of feature extractor
    int socket_output_remote_port;

    // number of seconds of audio material (16 bit samples, 8kHz sampling rate)
    // to be sent to the feature extractor
    int socket_output_send_seconds;

    //
    bool use_viat_db;

    // DB connect string
    // "dbname=<db name> user=<user> password=<pass> host=<ip>"
    std::string viat_db_connect_str;


    bool start_console;

    // listening port for console connections
    int console_port;

    // recording time in seconds
    int max_call_recording_time;

    // The maximum number of seconds a Call object can be active. If it exceeds
    // the deadline, it will be moved to the CallDecodeQueue and later be
    // deleted.
    int max_call_age;

    // The maximum number of seconds a Call object can be active if an error has
    // occurred before. If it exceeds the deadline, it will be moved to the
    // CallDecodeQueue and later be deleted.
    int max_call_age_if_error;

    // The maximum number of seconds a call object is allowed to have no rtp
    // activity. If it exceeds the deadline, it will be moved to the
    // CallDecodeQueue and later be deleted.
    int max_call_rtp_inactivity;

    // ***************************************
    // --- Call attemps --------------------->

    // sba call attemps analyze period
    int sba_call_attempts_period;

    // sba call attemps max count
    int sba_call_attempts_max;

    // ***************************************
    // --- Concurrent Calls ----------------->

    // analyze period in minutes
    int sba_calls_concurrent_period;

    // max concurrent calls
    int sba_calls_concurrent_max;

    // ***************************************
    // --- Call completion ----------------->

    // analyze period in minutes
    int sba_call_completion_period;

    // min call attemps
    int sba_call_completion_attempts_min;

    // min completion percentage
    int sba_call_completion_min;

    // ***************************************
    // --- Call duration average ------------>

    // analyze period in minutes
    int sba_call_duration_average_period;

    // min completed calls
    int sba_call_duration_average_min_completed;

    // min call duration in seconds;
    int sba_call_duration_average_min;

    // ***************************************
    // --- Call cumulative duration --------->

    // analyze period in minutes
    int sba_call_duration_cumulative_period;

    // max cumulative duration
    int sba_call_duration_cumulative_max;

    // ***************************************
    // --- Calls closed by Callee ----------->

    // analyze period in minutes
    int sba_calls_closed_by_callee_period;

    // min completed calls
    int sba_calls_closed_by_callee_min_completed;

    // max percentage
    int sba_calls_closed_by_callee_max;

    /*
     * Destructor.
     */
    virtual ~CallxConfig();

private:

    /*
     * Hide constructor.
     */
    CallxConfig();

    /*
     * Copying not allowed.
     */
    CallxConfig(const Config&);

    /*
     * Assigning not allowed.
     */
    CallxConfig& operator=(const Config&);

    /*
     * Append a slash at the end of the string, if not there
     */
    void ensureSlash(std::string& str);

    // the 'raw' config object
    Config m_config;

};

} /* namespace callx */

#endif /* CALLXCONFIG_HPP_ */
