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
#include "sdr_test.hpp"
#define SDR_TEST_DATA "/home/ntlhui/workspace/tmp/testData"
#include "sdr.hpp"
#include "dsp.hpp"
// #include "dspv1.hpp"
#include "dspv3.hpp"
// #include "localization.hpp"
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
#include <boost/program_options.hpp>
#include <sys/types.h>
#include <sys/syscall.h>

namespace po = boost::program_options;

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
		std::uint8_t verbosity = 0;

		po::options_description desc("sdr_record - Radio "
			"Collar Tracker drone application\n\nOptions");
		desc.add_options()
			("help,h", "Prints help message")
			("gain,g", po::value(&args.gain), "Gain")
			("sampling_freq,s", po::value(&args.rate), "Sampling Frequency")
			("center_freq,c", po::value(&args.rx_freq), "Center Frequency")
			("run,r", po::value(&args.run_num), "Run Number")
			("output,o", po::value(&args.data_dir), "Output Directory")
			("verbose,v", po::value(&verbosity), "Verbosity")
			("test_config", "Test Configuration")
			("test_data", po::value(&args.test_data), "Test Data")
			("gps_target", po::value(&args.gps_target), "GPS Target")
		;
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if(vm.count("help")){
			std::cout << desc << std::endl;
			exit(0);
		}

		if(vm.count("test_config")){
			args.test_config = true;
			if(args.test_data.empty()){
				syslog(LOG_ERR, "Test Data location not specified!");
				std::cout << desc << std::endl;
				exit(1);
			}

		}

		setlogmask(LOG_UPTO(verbosity));

		syslog(LOG_INFO, "Sanity checking args");

		if(!args.test_config && args.gps_target.empty()){
			syslog(LOG_ERR, "Must set GPS target!\n");
			std::cout << desc << std::endl;
			exit(1);
		}

		if (!args.run_num) {
			if(args.test_config){
				args.run_num = SDR_TEST::getRunNum(args.test_data);
			}else{
				syslog(LOG_ERR, "Must set run number\n");
				std::cout << desc << std::endl;
				exit(1);
			}
		}
		syslog(LOG_DEBUG, "Got run_num as %lu\n", args.run_num);

		if (!args.test_config && args.gain < 0){
			syslog(LOG_ERR, "Must set gain\n");
			std::cout << desc << std::endl;
			exit(1);
		}
		syslog(LOG_DEBUG, "Got gain as %.2f\n", args.gain);

		if (!args.test_config && args.data_dir.empty()){
			syslog(LOG_ERR, "Must set directory\n");
			std::cout << desc << std::endl;
			exit(1);
		}
		syslog(LOG_DEBUG, "Got data_dir as %s\n", args.data_dir.c_str());

		if (!args.rx_freq){
			if(args.test_config){
				args.rx_freq = SDR_TEST::getRxFreq(args.test_data);
			}else{
				syslog(LOG_ERR, "Must set freq\n");
				std::cout << desc << std::endl;
				exit(1);
			}
		}
		syslog(LOG_DEBUG, "Got rx_freq as %lu\n", args.rx_freq);

		if (!args.rate){
			if(args.test_config){
				args.rate = SDR_TEST::getRate(args.test_data);
			}else{
				syslog(LOG_ERR, "Must set rate\n");
				std::cout << desc << std::endl;
				exit(1);
			}
		}
		syslog(LOG_DEBUG, "Got rate as %lu\n", args.rate);
	}

	void SDR_RECORD::init(int argc, char * const*argv){
		this->process_args(argc, argv);
		std::ostringstream buffer;

		try{
			syslog(LOG_INFO, "Initializing Radio");
			if(args.test_config){
				sdr = new RTT::SDR_TEST(args.test_data, program_on);
			}else{
				sdr = new RTT::SDR(args.gain, args.rate, args.rx_freq);
			}
		}catch(std::runtime_error e){
			syslog(LOG_CRIT, "No devices found!");
			exit(1);
		}

		if(args.test_config){
			buffer.str("");
			buffer.clear();
			buffer << args.test_data << "/";
			buffer << "GPS_";
			buffer << std::setw(6) << std::setfill('0') << args.run_num;
			gps = new RTT::GPS(RTT::GPS::TEST_FILE, buffer.str());
		}else{
			gps = new RTT::GPS(RTT::GPS::SERIAL, args.gps_target);
		}

		if(!args.test_config){
			buffer.str("");
			buffer.clear();
			buffer << args.data_dir << "/";
			buffer << "GPS_";
			buffer << std::setw(6) << std::setfill('0') << args.run_num;
			gps->setOutputFile(buffer.str());
		}

		std::vector<std::size_t> frequencies{};
		frequencies.push_back(173964000+1000);
		// frequencies.push_back(173000000);
		// frequencies.push_back(173100000);
		// frequencies.push_back(173200000);
		// frequencies.push_back(173300000);
		// frequencies.push_back(173400000);
		// frequencies.push_back(173500000);
		// frequencies.push_back(173600000);
		// frequencies.push_back(173700000);
		// frequencies.push_back(173800000);

		dsp = new RTT::DSP_V3{args.rate, args.rx_freq, frequencies};
		if(!args.test_config){
			buffer.str("");
			buffer.clear();
			buffer << "RAW_DATA_";
			buffer << std::setw(6) << std::setfill('0') << args.run_num;
			buffer << std::setw(1) << "_";
			buffer << std::setw(4) << "%06d";
			dsp->setOutputDir(args.data_dir, buffer.str());
		}


		buffer.str("");
		buffer.clear();
		if(args.test_config){
			buffer << args.test_data;
		}else{
			buffer << args.data_dir;
		}
		buffer << "/LOCALIZE_";
		buffer << std::setw(6) << std::setfill('0') << args.run_num;
		std::cout << "Estimate to " << buffer.str() << std::endl;
		_estimate_str = new std::ofstream{buffer.str()};
		*_estimate_str << "{}" << std::endl; // to write to disk!

		localizer = new RTT::PingLocalizer(*_estimate_str);
	}

	void SDR_RECORD::sig_handler(int sig){
		std::unique_lock<std::mutex> run_lock(SDR_RECORD::instance()->run_mutex);
		SDR_RECORD::instance()->program_on = false;
		run_lock.unlock();
		SDR_RECORD::instance()->run_var.notify_all();
		std::cout << "Caught termination signal" << std::endl;
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

		timing_stream << "start_time: " << std::fixed << std::setprecision(3) << start_time.tv_sec + (float)start_time.tv_nsec / 1.e9 << std::endl;
		timing_stream << "center_freq: " << args.rx_freq << std::endl;
		timing_stream << "sampling_freq: " << args.rate << std::endl;
		timing_stream << "gain: " << args.gain << std::endl;
		timing_stream << "width: " << sizeof(int16_t) << std::endl;

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
		sdr->startStreaming(sdr_queue, sdr_queue_mutex, sdr_var);
		dsp->setStartTime(sdr->getStartTime_ms());
		gps->start();
		if(args.test_config){
			// wait for GPS to finish loading!
			gps->waitForLoad();
		}
		localizer->start(ping_queue, ping_queue_mutex, ping_var, *gps);
		while(true){
			std::unique_lock<std::mutex> run_lock(run_mutex);
			if(program_on){
				run_var.wait_for(run_lock, std::chrono::milliseconds{1000});
			}
			if(!program_on){
				break;
			}
		}
		std::cout << "Stopping sdr" << std::endl;
		sdr->stopStreaming();
		std::cout << "Stopping dsp" << std::endl;
		dsp->stopProcessing();
		std::cout << "Stopping localizer" << std::endl;
		localizer->stop();
		if(!args.test_config){
			std::cout << "Stopping GPS" << std::endl;
			gps->stop();
		}
	}

	SDR_RECORD::~SDR_RECORD(){
		*_estimate_str << std::endl << "{\"stop\": 1}" << std::endl;
		_estimate_str->close();
		delete _estimate_str;
		if(args.test_config){
			delete (RTT::SDR_TEST*) sdr;
		}else{
			delete (RTT::SDR*) sdr;
		}
		delete dsp;
		// delete localizer;
	}
}

int main(int argc, char **argv){
	openlog("sdr_record", LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(4));

	syslog(LOG_INFO, "Getting command line options");
	#ifdef DEBUG
	std::cout << "Main Thread: " << syscall(__NR_gettid) << std::endl;
	#endif
	RTT::SDR_RECORD* program = RTT::SDR_RECORD::instance();
	program->init(argc, argv);
	program->run();
	
	return 0;
}