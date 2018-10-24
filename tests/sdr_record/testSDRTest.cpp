#include <glob.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <iostream>

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
	RTT::SDR_TEST testObj("/home/ntlhui/workspace/tmp/");
	assert(testObj._files.size() == 4);
	
	std::queue<RTT::IQdataPtr> input_queue;
	std::mutex input_mutex;
	std::condition_variable input_cv;

	volatile bool run = true;

	auto start = std::chrono::steady_clock::now();
	testObj._process(input_queue, input_mutex, input_cv, &run);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	assert(!input_queue.empty());
}

int main(int argc, char const *argv[])
{
	testConstructor();
	testThroughput();
	return 0;
}
