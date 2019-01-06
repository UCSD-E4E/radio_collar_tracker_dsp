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

std::complex<double>* generateMixedSinusoid(std::int64_t t1, std::int64_t t2,
	std::size_t f_s, std::size_t N, double amplitude){
	std::complex<double>* retval = new std::complex<double>[N];
	for(std::size_t j = 0; j < N; j++){
		retval[j] = amplitude * amplitude * (std::exp(std::complex<double>(0, 2.0 * M_PI * (t1 + t2) * j / f_s)));
	}
	return retval;
}


void testMixer(){

	const std::size_t N = 2000000;
	const std::size_t f_s = 2000000;
	const std::size_t t1 = 5000000;
	const std::size_t t2 = 500000;

	RTT::Mixer testObj(t2, N);
	std::cout << testObj._period << std::endl;
	
	
	std::queue<std::complex<double>> input_queue{};
	std::mutex input_mutex{};
	std::condition_variable input_cv{};
	std::queue<std::complex<double>> output_queue{};
	std::mutex output_mutex{};
	std::condition_variable output_cv{};
	volatile bool run = true;
	


	std::complex<double>* testSignal = RTT::generateSinusoid(t1, f_s, N);
	
	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(input_mutex);
		input_queue.push(testSignal[i]);
		lock.unlock();
		input_cv.notify_all();
	}

	auto start = std::chrono::steady_clock::now();
	testObj.start(input_queue, input_mutex, input_cv, output_queue, 
		output_mutex, output_cv, &run);
	run = false;
	testObj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= N / double(f_s)));
	
	assert(input_queue.empty());
	std::cout << output_queue.size() << std::endl;
	assert(output_queue.size() == N);

	std::complex<double>* ref = generateMixedSinusoid(t1, -t2, f_s, N, 1.0);
	
	for(std::size_t i = 0; i < N; i++){
		if (std::abs(output_queue.front() - ref[i]) >= 0.01){
			std::cout << i << std::endl;
			std::cout << output_queue.front() << std::endl;
			std::cout << ref[i] << std::endl;
			std::cout << std::abs(output_queue.front() - ref[i]) << std::endl;
		}
		assert(std::abs(output_queue.front() - ref[i]) < 0.01);
		output_queue.pop();
	}
	delete(testSignal);
	delete(ref);
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
	
	std::complex<double>* ref = generateMixedSinusoid(1000, 1000, 2000000, 2000000, 1.0);
	
	for(std::size_t i = 0; i < 1999999; i++){
		if (std::abs(output_queue.front() - ref[i]) >= 0.01){
			std::cout << i << std::endl;
			std::cout << output_queue.front() << std::endl;
			std::cout << ref[i] << std::endl;
			std::cout << std::abs(output_queue.front() - ref[i]) << std::endl;
		}
		assert(std::abs(output_queue.front() - ref[i]) < 0.01);
		output_queue.pop();
	}
	delete(testSignal);
	delete(ref);
}

int main(int argc, char const *argv[])
{
	testMixer();
	testFunc();
	return 0;
}