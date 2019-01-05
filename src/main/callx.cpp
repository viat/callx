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

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <boost/date_time.hpp>
#include "callx.hpp"
#include "config/CallxConfig.hpp"
#include "network/PcapHandler.hpp"
#include "network/TlayerDispatcher.hpp"
#include "network/UdpHandler.hpp"
#include "sip/SipProcessor.hpp"
#include "audio/AudioHandler.hpp"
#include "console/Console.hpp"
#include "container/CallMap.hpp"
#include "container/CallDecodeQueue.hpp"
#include "container/SbaEventMap.hpp"
#include "container/PcmAudioQueue.hpp"
#include "container/TlayerPacketQueue.hpp"
#include "container/SipPacketQueue.hpp"
#include "output/OutputHandler.hpp"
#include "output/filesystem/WaveFileWriter.hpp"
#include "sba/SigBasedAna.hpp"
#include "watchdog/Watchdog.hpp"

using namespace std;
using namespace callx;

/* The flag stopRequested may be set to true in signal handler sigHandler()
 * and is consumed in run() as terminating condition.
 */
bool stopRequested = false;
ofstream pidFile;

/* Command line options.
 * One option so far:
 * -c <config file>
 */
struct CmdOptions {
	std::string configFileStr;
};

/*
 * Initialize loging (boost::log)
 */
void initLogging(std::string logfile, int level) {

	logging::add_file_log(keywords::file_name = logfile, // file name pattern
	keywords::format = "[%TimeStamp%] [%ThreadID%] %Message%", // message format

	//keywords::rotation_size = 10 * 1024 * 1024, // rotate files every 10 MiB...
	//keywords::time_based_rotation =
	//sinks::file::rotation_at_time_point(0, 0, 0), // ...or at midnight

			keywords::auto_flush = true // flush
					);

	logging::core::get()->set_filter(logging::trivial::severity < level);
	logging::add_common_attributes();
}

/*
 * Show command line options.
 */
void printHelp() {
	std::cout	<< endl
				<< "usage: callx [-c <config file>]"
				<< endl
				<< endl;
}

/*
 * Parse command line options.
 */
bool parseOpt(int argc, char **argv, CmdOptions &cmdOptions) {
	int c;
	char* configValue = NULL;
	while ((c = getopt(argc, argv, "hc:")) != -1) {
		switch (c) {
		case 'h':
			printHelp();
			return 0;
			break;

		case 'c':
			configValue = optarg;
			break;

		case '?':
			if (optopt == 'c') {
				cerr	<< "Option c requires the config filename as an argument."
						<< endl;
			}
			printHelp();
			return false;

		default:
			abort();
		}
	}
	if (configValue)
		cmdOptions.configFileStr.assign(configValue);

	return true;
}

void createPIDFile(std::string filename) {
	pidFile.open(filename, ios_base::in);
	if (pidFile.is_open()) {
		pidFile.close();
		throw("Abort. PID file exists. Application still running?");
	}

	pidFile.open(filename, ios_base::out);
	if (!pidFile.is_open()) {
		throw("Abort. Error creating PID file.");
	}
	ostringstream pidStringStream;
	pidStringStream << getpid();

	pidFile << pidStringStream.str()
			<< flush;
}

/**
 * Sets global stop request.
 */
void setStopRequest() {
	L_t<< "Setting global stop request.";
	stopRequested = true;
}

/**
 * Handles an incoming signal.
 * @param sig
 */
void sigHandler(int sig) {
	L_t<< "Signal "
	<< sig
	<< " received.";
	switch (sig) {
		case 2: // SIGINT
		setStopRequest();
		break;

		case 3:// SIGQUIT
		setStopRequest();
		break;

		case 15:// SIGTERM
		setStopRequest();
		break;

		default:
		break;
	}
}

void logConfig() {

	CallxConfig* callxConfig = CallxConfig::getInstance();

	L_f<< "Logging started: '"
	<< callxConfig->log_file
	<< "'";
	L_f<< "Loglevel is "
	<< callxConfig->log_level;

#ifdef _DEBUG
	L_i
	<< "Running DEBUG version.";
#else
	L_i<< "Running RELEASE version.";
#endif

	/* log config items (info) */
	L_i<< "--- CONFIGURATION START ---";

	L_i<< "log_file: "
	<< callxConfig->log_file;
	L_i<< "log_level: "
	<< callxConfig->log_level;
	L_i<< "pid_file: "
	<< callxConfig->pid_file;
	L_i<< "daemonize: "
	<< callxConfig->daemonize;
	L_i<< "pcap_device: "
	<< callxConfig->pcap_device;
	L_i<< "pcap_filter: "
	<< callxConfig->pcap_filter;
	L_i<< "min_sip_size: "
	<< callxConfig->min_sip_size;
	L_i<< "tp_repository_size: "
	<< callxConfig->tp_repository_size;
	L_i<< "mem_chunk_size: "
	<< callxConfig->mem_chunk_size;
	L_i<< "sba_pause: "
	<< callxConfig->sba_pause;
	L_i<< "record_if_incident_only: "
	<< callxConfig->record_if_incident_only;
	L_i<< "record_caller: "
	<< callxConfig->record_caller;
	L_i<< "record_callee: "
	<< callxConfig->record_callee;
	L_i<< "use_wavefile_output_interface: "
	<< callxConfig->use_wavefile_output_interface;
	L_i<< "wave_output_path: "
	<< callxConfig->wave_output_path;
	L_i<< "use_socket_output_interface: "
	<< callxConfig->use_socket_output_interface;
	L_i<< "socket_output_remote_ip: "
	<< callxConfig->socket_output_remote_ip;
	L_i<< "socket_output_remote_port: "
	<< callxConfig->socket_output_remote_port;
	L_i<< "socket_output_send_seconds: "
	<< callxConfig->socket_output_send_seconds;
	L_i<< "use_viat_db: "
	<< callxConfig->use_viat_db;
	L_i<< "viat_db_connect_str: "
	<< callxConfig->viat_db_connect_str;
	L_i<< "start_console "
	<< callxConfig->start_console;
	L_i<< "console_port: "
	<< callxConfig->console_port;
	L_i<< "max_call_recording_time: "
	<< callxConfig->max_call_recording_time;
	L_i<< "max_call_age: "
	<< callxConfig->max_call_age;
	L_i<< "max_call_age_if_error: "
	<< callxConfig->max_call_age_if_error;

	/* SBA config -> */
	L_i<< "sba_call_attempts_period: "
	<< callxConfig->sba_call_attempts_period;
	L_i<< "sba_call_attempts_max: "
	<< callxConfig->sba_call_attempts_max;
	L_i<< "sba_calls_concurrent_period: "
	<< callxConfig->sba_calls_concurrent_period;
	L_i<< "sba_calls_concurrent_max: "
	<< callxConfig->sba_calls_concurrent_max;
	L_i<< "sba_call_completion_period: "
	<< callxConfig->sba_call_completion_period;
	L_i<< "sba_call_completion_attemps_min: "
	<< callxConfig->sba_call_completion_attempts_min;
	L_i<< "sba_call_completion_min: "
	<< callxConfig->sba_call_completion_min;
	L_i<< "sba_call_duration_average_period: "
	<< callxConfig->sba_call_duration_average_period;
	L_i<< "sba_call_duration_average_min_completed: "
	<< callxConfig->sba_call_duration_average_min_completed;
	L_i<< "sba_call_duration_average_min: "
	<< callxConfig->sba_call_duration_average_min;
	L_i<< "sba_call_duration_cumulative_period: "
	<< callxConfig->sba_call_duration_cumulative_period;
	L_i<< "sba_call_duration_cumulative_max: "
	<< callxConfig->sba_call_duration_cumulative_max;
	L_i<< "sba_closed_by_callee_period: "
	<< callxConfig->sba_calls_closed_by_callee_period;
	L_i<< "sba_closed_by_callee_min_completed: "
	<< callxConfig->sba_calls_closed_by_callee_min_completed;
	L_i<< "sba_closed_by_callee_max: "
	<< callxConfig->sba_calls_closed_by_callee_max;
	L_i<< "--- CONFIGURATION  END ---";
}

void run(CallxConfig *callxConfig) {

	/* Create TlayerPacket repository and objects. */
	TlayerPacketQueue *tlayerPacketQueue = TlayerPacketQueue::getInstance();
	for (int i = 1; i <= callxConfig->tp_repository_size; i++) {

		tlayerPacketQueue->push(
				move(make_unique<TlayerPacket, TlayerPacketRecycler>()));
	}
	L_t<< "The TlayerPacket repository has got "
	<< tlayerPacketQueue->size()
	<< " elements.";

	/* Create thread objects. */
	unique_ptr<Console> console(new Console());
	unique_ptr<PcapHandler> pcapHandler(new PcapHandler());
	unique_ptr<TlayerDispatcher> tlayerDispatcher(new TlayerDispatcher());
	unique_ptr<UdpHandler> udpHandler(new UdpHandler());
	unique_ptr<SipProcessor> sipProcessor(new SipProcessor());
	unique_ptr<AudioHandler> audioHandler(new AudioHandler());
	unique_ptr<OutputHandler> outputHandler(new OutputHandler());
	unique_ptr<SigBasedAna> sigBasedAna(new SigBasedAna());
	unique_ptr<Watchdog> watchdog(new Watchdog());

	/* Start threads. */
	if (callxConfig->start_console) {
		L_i<< "Console has been created but will not be started, because start_console is false.";
		console->start();
	}
	pcapHandler->start();
	tlayerDispatcher->start();
	udpHandler->start();
	sipProcessor->start();
	audioHandler->start();
	outputHandler->start();
	sigBasedAna->start();
	watchdog->start();

	/* Register signal handles. */
	signal(SIGTERM, sigHandler);
	signal(SIGINT, sigHandler);
	signal(SIGUSR1, sigHandler);

	do {
		sleep(1);
	} while (!stopRequested);

	L_i<< "Stop requested. Stopping threads now.";
	if (!callxConfig->daemonize)
		cout	<< endl
				<< "Stop requested. Cleaning up. Be patient..."
				<< endl
				<< flush;

	/* stopping threads */
	bool console_stopped = console->stop();
	bool pcapHandler_stopped = pcapHandler->stop();
	bool tlayerDispatcher_stopped = tlayerDispatcher->stop();
	bool udphandler_stopped = udpHandler->stop();
	bool sipProcessor_stopped = sipProcessor->stop();
	bool audioHandler_stopped = audioHandler->stop();
	bool outputHandler_stopped = outputHandler->stop();
	bool sigBasedAna_stopped = sigBasedAna->stop();
	bool watchdog_stopped = watchdog->stop();

	if (console_stopped && pcapHandler_stopped && tlayerDispatcher_stopped
			&& udphandler_stopped && sipProcessor_stopped
			&& audioHandler_stopped && outputHandler_stopped
			&& sigBasedAna_stopped && watchdog_stopped) {

		/* Calling join() on threads to enable a clean shutdown */
		L_t<< "All threads are stopped. Calling join().";
		console->join();
		pcapHandler->join();
		tlayerDispatcher->join();
		udpHandler->join();
		sipProcessor->join();
		audioHandler->join();
		outputHandler->join();
		sigBasedAna->join();
		watchdog->join();
	} else {
		L_f
		<< "One or more threads did not stop.";
	}

	/* Deleting threads before deleting shared resources.. */
	console.reset();
	pcapHandler.reset();
	tlayerDispatcher.reset();
	udpHandler.reset();
	sipProcessor.reset();
	audioHandler.reset();
	outputHandler.reset();
	sigBasedAna.reset();
	watchdog.reset();

	/* Clean up singleton resources. */
	L_i<< "Cleaning up and deleting shared resources.";
	delete (PcapPacketQueue::getInstance());
	delete (UdpPacketQueue::getInstance());
	delete (SipPacketQueue::getInstance());
	delete (CallMap::getInstance());
	delete (SbaEventMap::getInstance());
	delete (CallDecodeQueue::getInstance());
	delete (PcmAudioQueue::getInstance());
	delete (CallxConfig::getInstance());

	L_i<< "Deleting "
	<< tlayerPacketQueue->size()
	<< " TlayerPacket objects and TlayerPacketQueue.";
	unique_ptr<TlayerPacket, TlayerPacketRecycler> tlayerPacket;
	while (!tlayerPacketQueue->empty()) {
		if (tlayerPacketQueue->waitAndPop(tlayerPacket)) {
			delete tlayerPacket.release();
		}
	}
	delete (TlayerPacketQueue::getInstance());
}

void daemonize() {

	/* Process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process. */
	pid = fork();
	if (pid < 0) {
		throw("Forking failed!");
	}

	/* Exit, if this is the parent process. */
	if (pid > 0) {
		throw("Forking successful, shutting down the parent.");
	}

	/* Change file mode mask */
	umask(0);

	/* Create new SID for child process. */
	sid = setsid();
	if (sid < 0) {
		throw("Creating a new SID for the child failed.");
	}

	/* Change the working directory. */
	string workDir = "/";
	if ((chdir(workDir.c_str())) < 0) {
		throw("Changing the working directory to " + workDir + " failed.");
	}

	/* Close file descriptors. */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char **argv) {
	/*
	 cout << endl
	 << flush;*/

	/* Parse options. */
	CmdOptions cmdOptions;
	if (!parseOpt(argc, argv, cmdOptions)) {
		exit(EXIT_FAILURE);
	}

	/* Initialize config values. */
	CallxConfig* callxConfig = CallxConfig::getInstance();
	if (cmdOptions.configFileStr.size() == 0) {
		cmdOptions.configFileStr = "/etc/viat/callx.cfg";
	}
	try {
		callxConfig->init(cmdOptions.configFileStr);
	} catch (const char* e) {
		cerr	<< endl
				<< "Error reading config file '"
				<< cmdOptions.configFileStr
				<< "'."
				<< endl
				<< "Use the option -c to specify the config file."
				<< endl;
		cerr	<< e
				<< endl;
		exit(EXIT_FAILURE);
	}
	cout	<< "Using configuration file '"
			<< cmdOptions.configFileStr
			<< "'."
			<< endl;
	cout	<< "Writing log into '"
			<< callxConfig->log_file
			<< "'."
			<< endl;
	cout	<< "Wave output directory '"
			<< callxConfig->wave_output_path
			<< "'."
			<< endl;
	if (callxConfig->start_console) {
		cout	<< "TCP Console at localhost port "
				<< callxConfig->console_port
				<< "."
				<< endl;
	}

	/* Configure logging. */
	initLogging(callxConfig->log_file, callxConfig->log_level);
	logConfig();

	/* Check output directory if using wave output interface */
	if (callxConfig->use_wavefile_output_interface) {
		WaveFileWriter wfw(
				callxConfig->wave_output_path + "callx_temp_file.tmp");
	}

	/* PID file */
	if (callxConfig->pid_file != "") {
		try {
			createPIDFile(callxConfig->pid_file);
		} catch (const char* e) {
			L_t<< e;
			cerr << e
			<< endl
			<< endl;
			exit(EXIT_FAILURE);
		}
	}

	/* Daemonize */
	if (callxConfig->daemonize) {
		cout	<< "Starting daemon."
				<< endl
				<< flush;
		try {
			daemonize();
		} catch (const char* e) {
			L_t<< "Exception in daemonize(): "
			<< e;
			exit(EXIT_FAILURE);
		} catch (...) {
			L_t
			<< "Caught an unknown exception in daemonize().";
			exit(EXIT_FAILURE);
		}
	}

	/* Start running. */
#ifndef DEBUG
	try {
#endif
		run(callxConfig);
#ifndef DEBUG
	} catch (...) {
		L_t<< "Caught an unknown exception in run().";
	}
#endif

	/* Delete possible PID file. */
	if (pidFile.is_open()) {
		pidFile.close();
		if (remove(callxConfig->pid_file.c_str()) == 0) {
			L_i<< "PID file deleted.";
		} else {
			L_e
			<< "Error deleting PID file.";
		}
	}

	/* Application termination. */
	L_i<< "Bye-bye!";
	if (!callxConfig->daemonize)
		cout	<< "Done."
				<< endl
				<< flush;

	exit(EXIT_SUCCESS);
}
