#include <ping.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <ping_classifier.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>
#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

void testSinusoidAmplitude(){
	std::size_t start_time = 0;
	RTT::PingClassifier testObj(0.5, start_time, 1);
	std::queue<double> input_queue;
	std::mutex input_mutex;
	std::condition_variable input_cv;
	std::queue<RTT::PingPtr> output_queue;
	std::mutex output_mutex;
	std::condition_variable output_cv;
	volatile bool run = true;
	testObj.start(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	auto start = std::chrono::steady_clock::now();
	for(std::size_t i = 0; i < 1000; i++){
		std::unique_lock<std::mutex> in_lock(input_mutex);
		input_queue.push(std::sin(2.0 * M_PI / 2000.0 * i));
		in_lock.unlock();
		input_cv.notify_all();
	}
	run = false;
	testObj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		// 1000.0 / 2000000));
	assert(input_queue.size() == 0);
	assert(output_queue.size() == 1);
	RTT::PingPtr ping = output_queue.front();
	std::cout << ping->time_ms << std::endl;
	assert(ping->time_ms == 500);
	assert(std::abs(ping->amplitude - 1.0) < 0.001);
}

void testSinusoidAmplitude_func(){
	std::size_t start_time = 0;
	RTT::PingClassifier testObj(0.5, start_time, 1);
	std::queue<double> input_queue;
	std::mutex input_mutex;
	std::condition_variable input_cv;
	std::queue<RTT::PingPtr> output_queue;
	std::mutex output_mutex;
	std::condition_variable output_cv;
	volatile bool run = false;
	for(std::size_t i = 0; i < 1000; i++){
		std::unique_lock<std::mutex> in_lock(input_mutex);
		input_queue.push(std::sin(2.0 * M_PI / 2000.0 * i));
		in_lock.unlock();
		input_cv.notify_all();
	}
	run = false;
	auto start = std::chrono::steady_clock::now();
	testObj._process(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		1000.0 / 2000000));
	assert(input_queue.size() == 0);
	assert(output_queue.size() == 1);
	RTT::PingPtr ping = output_queue.front();
	std::cout << ping->time_ms << std::endl;
	assert(ping->time_ms == 500);
	assert(std::abs(ping->amplitude - 1.0) < 0.001);
}

void testImpulseAmplitude(){
	
}

int main(int argc, char const *argv[])
{
	testSinusoidAmplitude();
	testSinusoidAmplitude_func();
	return 0;
}