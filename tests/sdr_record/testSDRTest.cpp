#include <glob.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define private public
#define protected public
#include <sdr_test.hpp>
#undef private
#undef protected

#include <cassert>
#include <iq_data.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

void testConstructor(){
	RTT::SDR_TEST testObj("/home/ntlhui/workspace/tmp/testData/");
	assert(testObj._files.size() == 1);
}

void testThroughput(){
	RTT::SDR_TEST testObj("/home/ntlhui/workspace/tmp/testData/");
	assert(testObj._files.size() == 1);
	double maxTime = 67108864 / 4 / 2000000.0 * testObj._files.size();
	
	std::queue<RTT::IQdataPtr> input_queue;
	std::mutex input_mutex;
	std::condition_variable input_cv;

	auto start = std::chrono::steady_clock::now();
	testObj._process(input_queue, input_mutex, input_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	double time_elapsed = std::chrono::duration <double, std::ratio<1, 1>> (diff).count();
	assert(abs(time_elapsed / maxTime - 1) < 0.1);
	std::cout << "Difference: " << time_elapsed / maxTime - 1 << '%' << std::endl;
	assert(!input_queue.empty());
}


void testTime(){
	RTT::SDR_TEST testObj("/home/ntlhui/workspace/tmp/testData/");
	std::cout << testObj.getStartTime_ms() << std::endl;
	assert(testObj.getStartTime_ms() == 1502770079180);
}

void testGetRunNum(){
	std::string dirname{"/tmp/test"};
	int check = mkdir(dirname.c_str(), 0777);
	if(check == -1){
		std::cout << "Failed to create directory!" << std::endl;
		return;
	}
	std::ofstream ostr1{"/tmp/test/META_000001"};
	ostr1.close();

	int retval = RTT::SDR_TEST::getRunNum(dirname);

	if(unlink("/tmp/test/META_000001") == -1){
		std::cout << "Failed to remove file" << std::endl;
	}
	if(rmdir(dirname.c_str()) == -1){
		std::cout << "Failed to remove directory" << std::endl;
	}
	assert(retval == 1);
}

void testGetRxFreq(){
	std::string dirname{"/tmp/test"};
	int check = mkdir(dirname.c_str(), 0777);
	if(check == -1){
		std::cout << "Failed to create directory!" << std::endl;
		return;
	}
	std::ofstream ostr1{"/tmp/test/META_000001"};
	ostr1 << "start_time: 1.55707e+09" << std::endl;
	ostr1 << "center_freq: 173000000" << std::endl;
	ostr1 << "sampling_freq: 250000" << std::endl;
	ostr1 << "gain: 22" << std::endl;
	ostr1 << "width: 2" << std::endl;
	ostr1.close();

	// check here
	uint64_t retval = RTT::SDR_TEST::getRxFreq(dirname);

	if(unlink("/tmp/test/META_000001") == -1){
		std::cout << "Failed to remove file" << std::endl;
	}
	if(rmdir(dirname.c_str()) == -1){
		std::cout << "Failed to remove directory" << std::endl;
	}
	assert(retval == 173000000);
}

void testGetRate(){
	std::string dirname{"/tmp/test"};
	int check = mkdir(dirname.c_str(), 0777);
	if(check == -1){
		std::cout << "Failed to create directory!" << std::endl;
		return;
	}
	std::ofstream ostr1{"/tmp/test/META_000001"};
	ostr1 << "start_time: 1.55707e+09" << std::endl;
	ostr1 << "center_freq: 173000000" << std::endl;
	ostr1 << "sampling_freq: 250000" << std::endl;
	ostr1 << "gain: 22" << std::endl;
	ostr1 << "width: 2" << std::endl;
	ostr1.close();

	// check here
	uint64_t retval = RTT::SDR_TEST::getRate(dirname);

	if(unlink("/tmp/test/META_000001") == -1){
		std::cout << "Failed to remove file" << std::endl;
	}
	if(rmdir(dirname.c_str()) == -1){
		std::cout << "Failed to remove directory" << std::endl;
	}
	assert(retval == 250000);
}

int main(int argc, char const *argv[])
{
	// testConstructor();
	// testThroughput();
	// testTime();
	testGetRunNum();
	testGetRxFreq();
	testGetRate();
	return 0;
}
