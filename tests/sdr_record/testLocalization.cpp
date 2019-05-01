#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "ping.hpp"
#include <dlib/optimization.h>
#define private public
#define protected public
#include "gps.hpp"
#include <localization.hpp>
#undef private
#undef protected

#include <sdr_test.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "dspv3.hpp"
#include <vector>
#include <location.hpp>
#include <gps_test.hpp>
#include <syslog.h>
#include <iostream>
#include <unistd.h>
#include <glob.h>
#include <sstream>

void testData(){
	openlog("sdr_record", LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(LOG_INFO));
	RTT::PingLocalizer localizer{};

	std::string data_dir{"/media/ntlhui/FA56-CFCD/2017.08.22/RUN_000055"};
	glob_t glob_output;
	std::ostringstream globfilestream{};
	globfilestream << data_dir << "/GPS_??????";
	assert(!glob(globfilestream.str().c_str(), 0, nullptr, &glob_output));


	RTT::SDR_TEST sdr(data_dir);
	std::queue<RTT::IQdataPtr> iq_data_queue;
	std::mutex iq_data_mutex;
	std::condition_variable iq_data_cv;
	

	std::vector<uint32_t> freqs;
	freqs.push_back(172017000);
	RTT::DSP_V3 dsp(2000000);
	std::queue<RTT::PingPtr> pingQueue{};
	std::mutex pingMutex{};
	std::condition_variable pingVar{};

	RTT::GPS gps(RTT::GPS::Protocol::TEST_FILE, glob_output.gl_pathv[0]);

	sdr.startStreaming(iq_data_queue, iq_data_mutex, iq_data_cv);
	dsp.setStartTime(sdr.getStartTime_ms());
	dsp.startProcessing(iq_data_queue, iq_data_mutex, iq_data_cv, pingQueue, pingMutex, pingVar);
	gps.start();
	localizer.start(pingQueue, pingMutex, pingVar, gps);
	sleep(15);
	// std::cout << pingQueue.size() << std::endl;
	// auto start = std::chrono::steady_clock::now();
	// auto end = std::chrono::steady_clock::now();
	// auto diff = end - start;
	// std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	
	// std::cout << "Stopping all" << std::endl;
	
	sdr.stopStreaming();
	localizer.stop();
	gps.stop();
	dsp.stopProcessing();
	globfree(&glob_output);
}

void testEstimate(){
	
}

int main(int argc, char const* argv[]){
	testEstimate();
	testData();
}