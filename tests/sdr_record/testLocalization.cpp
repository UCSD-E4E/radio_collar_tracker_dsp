#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "ping.hpp"
#include "gps.hpp"
#include <dlib/optimization.h>
#define private public
#define protected public
#include <localization.hpp>
#undef private
#undef protected

#include <sdr_test.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "dspv2.hpp"
#include <vector>
#include <location.hpp>
#include <gps_test.hpp>
#include <syslog.h>
#include <iostream>
#include <unistd.h>

void testThroughPut(){
}

int main(int argc, char const* argv[]){
	openlog("sdr_record", LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(8));
	RTT::PingLocalizer localizer{}                                ;

	volatile bool run = true;

	RTT::SDR_TEST sdr("/home/ntlhui/workspace/tmp/testData/RUN_000014");
	std::queue<RTT::IQdataPtr> iq_data_queue;
	std::mutex iq_data_mutex;
	std::condition_variable iq_data_cv;
	

	std::vector<uint32_t> freqs;
	freqs.push_back(172017000);
	RTT::DSP_V2 dsp(freqs, 172500000, 2000000);
	std::queue<RTT::PingPtr> pingQueue{};
	std::mutex pingMutex{};
	std::condition_variable pingVar{};

	RTT::GPS gps(RTT::GPS::Protocol::TEST_FILE, "/home/ntlhui/workspace/tmp/testData/RUN_000014/GPS_000014");

	sdr.startStreaming(iq_data_queue, iq_data_mutex, iq_data_cv, &run);
	dsp.startProcessing(iq_data_queue, iq_data_mutex, iq_data_cv, pingQueue, pingMutex, pingVar, &run);
	gps.start(&run);
	localizer.start(pingQueue, pingMutex, pingVar, gps, &run);

	sleep(20);
	run = false;
	std::cout << "Stopping all" << std::endl;
	
	localizer.stop();
	gps.stop();
	dsp.stopProcessing();
	sdr.stopStreaming();
}