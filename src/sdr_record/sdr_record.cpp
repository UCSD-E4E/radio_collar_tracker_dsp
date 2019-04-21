/*
 * @file sdr_test.cpp
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * This file provides a software interface to allow testing from existing
 * datasets.  This should be able to provide a drop-in replacement for the
 * actual sdr_record app.
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

#include "sdr_record.hpp"
#ifdef TEST_SDR
	#include "sdr_test.hpp"
	#ifndef SDR_TEST_DATA
		#define SDR_TEST_DATA "/home/ntlhui/workspace/tmp/testData"
	#endif
#else
	#include "sdr.hpp"
#endif
#include "dsp.hpp"
// #include "dspv1.hpp"
#include "dspv3.hpp"
#include "localization.hpp"
#include <string>
#include <mutex>
#include <queue>
#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <sys/time.h>
#include <vector>
#include <condition_variable>

namespace RTT{
	const std::string META_PREFIX = "META_";

	SDR_RECORD* SDR_RECORD::m_pInstance = NULL;

	void SDR_RECORD::print_help(){
		printf("sdr_record - Radio Collar Tracker drone application to pull IQ samples from USRP and dump to disk\n\n"
				"Options:\n"
				"    -r (run_number)\n"
				"    -f (collar frequency in Hz)\n"
				"    -s (sample rate in Hz)\n"
				"    -g (gain)\n"
				"    -o (output directory)\n"
				"    -v [1-7]            Verbosity Level\n"
				"    -h (print this help message)\n");

		exit(0);
	}

	SDR_RECORD::SDR_RECORD(){
		syslog(LOG_INFO, "Setting signal handler");
		signal(SIGINT, &RTT::SDR_RECORD::sig_handler);
	}

	SDR_RECORD* SDR_RECORD::instance(){
		if(!m_pInstance){
			m_pInstance = new SDR_RECORD;
		}
		return m_pInstance;
	}

	void SDR_RECORD::process_args(int argc, char* const* argv){
		int option = 0;
		while((option = getopt(argc, argv, "hg:s:c:r:o:v:f:")) != -1){
			switch(option)
			{
				case 'h':
					syslog(LOG_INFO, "Got help flag");
					print_help();
					break;
				case 'g':
					args.gain = std::stod(optarg);
					syslog(LOG_INFO, "Got gain setting of %.2f", args.gain);
					break;
				case 's':
					args.rate = std::stol(optarg);
					syslog(LOG_INFO, "Got sampling rate setting of %ld", args.rate);
					break;
				case 'f':
					args.rx_freq = std::stod(optarg);
					syslog(LOG_INFO, "Got collar/center frequency target of %ld", args.rx_freq);
					break;
				case 'r':
					args.run_num = std::stoi(optarg);
					syslog(LOG_INFO, "Got run number of %ld", args.run_num);
					break;
				case 'o':
					args.data_dir = std::string(optarg);
					syslog(LOG_INFO, "Got an output directory of %s", args.data_dir.c_str());
					break;
				case 'v':
					syslog(LOG_INFO, "Setting log output to %d", std::stoi(optarg));
					setlogmask(LOG_UPTO(std::stoi(optarg)));
					break;
			}
		}

		syslog(LOG_INFO, "Sanity checking args");

		if (!args.run_num) {
			syslog(LOG_ERR, "Must set run number\n");
			print_help();
		}

		if (args.gain < 0){
			syslog(LOG_ERR, "Must set gain\n");
			print_help();
		}

		if (args.data_dir.empty()){
			syslog(LOG_ERR, "Must set directory\n");
			print_help();
		}

		if (!args.rx_freq){
			syslog(LOG_ERR, "Must set freq\n");
			print_help();
		}

		if (!args.rate){
			syslog(LOG_ERR, "Must set rate\n");
			print_help();
		}
	}

	void SDR_RECORD::init(int argc, char * const*argv){
		this->process_args(argc, argv);		

		try{
			syslog(LOG_INFO, "Initializing Radio");
			#ifdef TEST_SDR
			sdr = new RTT::SDR_TEST(std::string(SDR_TEST_DATA));
			#else
			sdr = new RTT::SDR(args.gain, args.rate, args.rx_freq);
			#endif
		}catch(std::runtime_error e){
			syslog(LOG_CRIT, "No devices found!");
			exit(1);
		}

		dsp = new RTT::DSP_V3{args.rate};
		localizer = new RTT::PingLocalizer();
	}

	void SDR_RECORD::sig_handler(int sig){
		std::unique_lock<std::mutex> run_lock(SDR_RECORD::instance()->run_mutex);
		SDR_RECORD::instance()->program_on = 0;
		run_lock.unlock();
		SDR_RECORD::instance()->run_var.notify_all();
		syslog(LOG_WARNING, "Caught termination signal");
	}

	void SDR_RECORD::print_meta_data(){

		struct timespec start_time;
		std::ofstream timing_stream;

		clock_gettime(CLOCK_REALTIME, &start_time);

		std::ostringstream buffer;
		buffer << args.data_dir << "/" << META_PREFIX;
		buffer << std::setw(6) << std::setfill('0');
		buffer << args.run_num;

		timing_stream.open(buffer.str());		

		timing_stream << "start_time: " << start_time.tv_sec + (float)start_time.tv_nsec / 1.e9 << std::endl;
		timing_stream << "center_freq: " << args.rx_freq << std::endl;
		timing_stream << "sampling_freq: " << args.rate << std::endl;
		timing_stream << "gain: " << args.gain << std::endl;

		timing_stream.close();		
	}

	void SDR_RECORD::receiver(){
		syslog(LOG_INFO, "rx: Starting USRP stream");

	}

	void SDR_RECORD::run(){
		syslog(LOG_DEBUG, "Printing metadata to file");
		this->print_meta_data();

		syslog(LOG_INFO, "Starting threads");
		dsp->startProcessing(sdr_queue, sdr_queue_mutex, sdr_var, ping_queue, 
			ping_queue_mutex, ping_var);
		sdr->startStreaming(sdr_queue, sdr_queue_mutex, sdr_var, &program_on);
		while(true){
			std::unique_lock<std::mutex> run_lock(run_mutex);
			if(program_on){
				run_var.wait_for(run_lock, std::chrono::milliseconds{100});
			}else{
				break;
			}
		}
		sdr->stopStreaming();
		dsp->stopProcessing();
	}

	SDR_RECORD::~SDR_RECORD(){
		delete sdr;
		delete dsp;
		delete localizer;
	}
}

int main(int argc, char **argv){
	openlog("sdr_record", LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(4));

	syslog(LOG_INFO, "Getting command line options");
	RTT::SDR_RECORD* program = RTT::SDR_RECORD::instance();
	program->init(argc, argv);
	program->run();
	
	return 0;
}