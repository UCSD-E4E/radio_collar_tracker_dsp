#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <mixer.hpp>
#undef private
#undef protected

#include <cassert>
#include <utility.hpp>
#include <iostream>

void testMixer(){
	RTT::Mixer testObj(-1000, 2000000);
	std::cout << testObj._period << std::endl;
	std::queue<std::complex<double>> input_queue{};
	std::mutex input_mutex{};
	std::condition_variable input_cv{};
	std::queue<std::complex<double>> output_queue{};
	std::mutex output_mutex{};
	std::condition_variable output_cv{};
	volatile bool run = true;
	testObj.start(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);

	std::complex<double>* testSignal = RTT::generateSinusoid(1000, 2000000, 
		2000000);
	auto start = std::chrono::steady_clock::now();
	for(std::size_t i = 0; i < 2000000; i++){
		std::unique_lock<std::mutex> lock(input_mutex);
		input_queue.push(testSignal[i]);
		lock.unlock();
		input_cv.notify_all();
	}

	run = false;
	testObj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 2000000.0 / 2000000));
	assert(input_queue.empty());
	std::cout << output_queue.size() << std::endl;
	assert(output_queue.size() == 2000000);
	output_queue.pop();
	double phase_angle = arg(output_queue.front());
	for(std::size_t i = 0; i < 1999999; i++){
		assert(std::abs(arg(output_queue.front()) - phase_angle) < 0.001);
		output_queue.pop();
	}
	delete(testSignal);
}

void testFunc(){
	RTT::Mixer testObj(-1000, 2000000);

	std::queue<std::complex<double>> input_queue{};
	std::mutex input_mutex{};
	std::condition_variable input_cv{};

	std::queue<std::complex<double>> output_queue{};
	std::mutex output_mutex{};
	std::condition_variable output_cv{};

	volatile bool run = false;

	std::complex<double>* testSignal = RTT::generateSinusoid(1000, 2000000, 
		2000000);
	for(std::size_t i = 0; i < 2000000; i++){
		input_queue.push(testSignal[i]);
	}
	auto start = std::chrono::steady_clock::now();
	testObj._process(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 2000000.0 / 2000000));
	assert(input_queue.empty());
	std::cout << output_queue.size() << std::endl;
	assert(output_queue.size() == 2000000);
	output_queue.pop();
	double phase_angle = arg(output_queue.front());
	for(std::size_t i = 0; i < 1999999; i++){
		assert(std::abs(arg(output_queue.front()) - phase_angle) < 0.001);
		output_queue.pop();
	}
	delete[] testSignal;
}

int main(int argc, char const *argv[])
{
	testMixer();
	testFunc();
	return 0;
}