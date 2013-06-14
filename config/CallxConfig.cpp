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
 * CallxConfig.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: mainka
 */

#include "CallxConfig.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;

namespace callx {

CallxConfig::CallxConfig()
        : log_file("/var/log/callx.log"),
          log_level(1),
          pid_file(""),
          daemonize(false),
          pcap_device("eth0"),
          pcap_filter("udp"),
          min_sip_size(300),
          tp_repository_size(1000000),
          mem_chunk_size(1024),
          sba_pause(60),
          record_if_incident_only(false),
          record_caller(true),
          record_callee(false),
          use_wavefile_output_interface(true),
          wave_output_path("/var/spool/callx/output/"),
          use_socket_output_interface(false),
          socket_output_remote_ip(""),
          socket_output_remote_port(0),
          socket_output_send_seconds(6),
          use_viat_db(false),
          viat_db_connect_str(""),
          start_console(true),
          console_port(5000),

          max_call_recording_time(12),
          max_call_age(3600),
          max_call_age_if_error(30),
          max_call_rtp_inactivity(90),

          sba_call_attempts_period(100),
          sba_call_attempts_max(60),

          sba_calls_concurrent_period(60),
          sba_calls_concurrent_max(10),

          sba_call_completion_period(60),
          sba_call_completion_attempts_min(20),
          sba_call_completion_min(70),

          sba_call_duration_average_period(20),
          sba_call_duration_average_min_completed(20),
          sba_call_duration_average_min(5),

          sba_call_duration_cumulative_period(100),
          sba_call_duration_cumulative_max(100),

          sba_calls_closed_by_callee_period(60),
          sba_calls_closed_by_callee_min_completed(60),
          sba_calls_closed_by_callee_max(80) {

}

CallxConfig::~CallxConfig() {

}

void CallxConfig::init(string configFile) {
    if (!m_config.parseConfig(configFile)) {
        throw("Cannot open/read config file.");
    }

    // get config values
    log_file = m_config.getStr("log_file", log_file);
    log_level = m_config.getInt("log_level", log_level);
    pid_file = m_config.getStr("pid_file", pid_file);
    daemonize = m_config.getBool("daemonize", daemonize);
    pcap_device = m_config.getStr("pcap_device", pcap_device);
    pcap_filter = m_config.getStr("pcap_filter", pcap_filter);
    min_sip_size = m_config.getInt("min_sip_size", min_sip_size);
    tp_repository_size = m_config.getInt("tp_repository_size",
            tp_repository_size);
    mem_chunk_size = m_config.getInt("mem_chunk_size", mem_chunk_size);
    sba_pause = m_config.getInt("sba_pause", sba_pause);
    record_if_incident_only = m_config.getBool("record_if_incident_only");
    record_caller = m_config.getBool("record_caller");
    record_callee = m_config.getBool("record_callee");
    use_wavefile_output_interface =
            m_config.getBool("use_wavefile_output_interface",
                    use_wavefile_output_interface);
    wave_output_path = m_config.getStr("wave_output_path", wave_output_path);
    use_socket_output_interface =
            m_config.getBool("use_socket_output_interface",
                    use_socket_output_interface);
    socket_output_remote_ip = m_config.getStr("socket_output_remote_ip");
    socket_output_remote_port = m_config.getInt("socket_output_remote_port");
    socket_output_send_seconds = m_config.getInt("socket_output_send_seconds");
    use_viat_db = m_config.getBool("use_viat_db");
    viat_db_connect_str = m_config.getStr("viat_db_connect_str");
    start_console = m_config.getInt("start_console");
    console_port = m_config.getInt("console_port");

    max_call_recording_time = m_config.getInt("max_call_recording_time");
    max_call_age = m_config.getInt("max_call_age");
    max_call_age_if_error = m_config.getInt("max_call_age_if_error");
    max_call_rtp_inactivity = m_config.getInt("max_call_rtp_inactivity");

    sba_call_attempts_period = m_config.getInt("sba_call_attempts_period",
            sba_call_attempts_period);
    sba_call_attempts_max = m_config.getInt("sba_call_attempts_max",
            sba_call_attempts_max);

    sba_calls_concurrent_period = m_config.getInt("sba_calls_concurrent_period",
            sba_calls_concurrent_period);
    sba_calls_concurrent_max = m_config.getInt("sba_calls_concurrent_max",
            sba_calls_concurrent_max);

    sba_call_completion_period = m_config.getInt("sba_call_completion_period",
            sba_call_completion_period);
    sba_call_completion_attempts_min =
            m_config.getInt("sba_call_completion_attempts_min",
                    sba_call_completion_attempts_min);
    sba_call_completion_min = m_config.getInt("sba_call_completion_min",
            sba_call_completion_min);

    sba_call_duration_average_period =
            m_config.getInt("sba_call_duration_average_period",
                    sba_call_duration_average_period);
    sba_call_duration_average_min_completed =
            m_config.getInt("sba_call_duration_average_min_completed",
                    sba_call_duration_average_min_completed);
    sba_call_duration_average_min =
            m_config.getInt("sba_call_duration_average_min",
                    sba_call_duration_average_min);

    sba_call_duration_cumulative_period =
            m_config.getInt("sba_call_duration_cumulative_period",
                    sba_call_duration_cumulative_period);
    sba_call_duration_cumulative_max =
            m_config.getInt("sba_calls_duration_cumulative_max",
                    sba_call_duration_cumulative_max);

    sba_calls_closed_by_callee_period =
            m_config.getInt("sba_closed_by_callee_period",
                    sba_calls_closed_by_callee_period);
    sba_calls_closed_by_callee_min_completed =
            m_config.getInt("sba_closed_by_callee_min_completed",
                    sba_calls_closed_by_callee_min_completed);
    sba_calls_closed_by_callee_max =
            m_config.getInt("sba_calls_closed_by_callee_max",
                    sba_calls_closed_by_callee_max);

    // ***********************************************************
    // check config value constraints -->

    if (use_socket_output_interface && socket_output_remote_ip.empty()) {
        throw("Config error: use_socket_output_interface is true but socket_output_remote_ip is not set.");
    }

    if (use_socket_output_interface && !socket_output_remote_port) {
        throw("Config error: use_socket_output_interface is true but socket_output_remote_port is not set.");
    }

    if (use_wavefile_output_interface && wave_output_path.empty()) {
        throw("Config error: use_wavefile_output_interface is true but wave_output_path is not set, e.g. wave_output_path=/var/spool/callx/output/");
    }
    ensureSlash(wave_output_path);

    if (use_socket_output_interface && !use_viat_db) {
        throw("Config error: use_socket_output_interface is true but use_viat_db is false.");
    }

    if (use_socket_output_interface && !use_viat_db) {
        throw("Config error: use_socket_output_interface is true but viat_db_connect_str is not set.");
    }

    if (sba_pause < 0) {
        throw("Config error: sba_pause has to be greater zero.");
    }

    // <-- check config value constraints
    // ***********************************************************
}

void CallxConfig::ensureSlash(std::string& str) {
    if (str[str.size() - 1] != '/')
        str.append("/");
}

} /* namespace callx */
