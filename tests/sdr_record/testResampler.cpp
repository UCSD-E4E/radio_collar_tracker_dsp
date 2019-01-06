#include <queue>
#include <complex>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <resampler.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>

void testUpsample(){

}

void testDownsample_threaded(){
	const std::size_t factor = 1000;

	RTT::Resampler testObj(1, factor);

	const std::size_t N = 10000;

	std::queue<std::complex<double>> input_queue{};
	std::mutex input_mutex{};
	std::condition_variable input_cv{};
	
	std::queue<std::complex<double>> output_queue{};
	std::mutex output_mutex{};
	std::condition_variable output_cv{};
	
	volatile bool run = true;
	
	testObj.start(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	
	auto start = std::chrono::steady_clock::now();
	for(std::size_t i = 0; i < N; i++){
		// std::cout << "Try " << i << std::endl;
		std::unique_lock<std::mutex> lock(input_mutex);
		input_queue.push(std::complex<double>(i, 0));
		lock.unlock();
		input_cv.notify_all();
	}
	run = false;
	testObj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		// 10000.0 / 2000000));
	assert(input_queue.size() == 0);
	std::cout << output_queue.size() << std::endl;
	assert(output_queue.size() == N / factor);
	// for(std::size_t i = 0; i < N / factor; i++){
	// 	std::complex<double> value = output_queue.front();
	// 	output_queue.pop();
	// 	double ref_val = (i * 100.0 + ((i + 1) * 100.0 - 1)) / 2;
	// 	assert(std::abs(value.real() - ref_val) < 0.001);
	// 	assert(std::abs(value.imag()) < 0.001);
	// }
}

void testDownsample_func(){
	std::size_t factor = 1000;
	std::size_t N = 10000;
	RTT::Resampler testObj(1, factor);
	std::queue<std::complex<double>> input_queue{};
	std::mutex input_mutex{};
	std::condition_variable input_cv{};
	std::queue<std::complex<double>> output_queue{};
	std::mutex output_mutex{};
	std::condition_variable output_cv{};
	volatile bool run = true;
	for(std::size_t i = 0; i < N; i++){
		// std::cout << "Try " << i << std::endl;
		std::unique_lock<std::mutex> lock(input_mutex);
		input_queue.push(std::complex<double>(i, 0));
		lock.unlock();
		input_cv.notify_all();
	}
	run = false;
	auto start = std::chrono::steady_clock::now();
	testObj.start(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	testObj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		10000.0 / 2000000));
	assert(input_queue.size() == 0);
	assert(output_queue.size() == N / factor);
	for(std::size_t i = 0; i < N; i+= factor){
		std::complex<double> value = output_queue.front();
		output_queue.pop();
		double ref_val = (i + ((i + factor) - 1)) / 2.0;
		std::cout << value << ", " << ref_val << std::endl;
		assert(std::abs(value.real() - ref_val) < 0.001);
		assert(std::abs(value.imag()) < 0.001);
	}
}

void testResample(){

}

void testConstructor(){
	RTT::Resampler testObj(123, 456);
	assert(testObj._upsample_factor == 123);
	assert(testObj._downsample_factor == 456);
}

int main(int argc, char const *argv[])
{
	testConstructor();
	testUpsample();
	testDownsample_threaded();
	testDownsample_func();
	testResample();
	return 0;
}