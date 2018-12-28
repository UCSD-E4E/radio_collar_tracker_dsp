#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <limits>
#define private public
#define protected public
#include <ping_fir.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>
#include <utility.hpp>
#include <unistd.h>
#include <chrono>

void test_constructor(){
	RTT::PingFIR test_obj(172500000, 2000000, 200);
	assert(test_obj._num_taps == 200);
	assert(test_obj._filter_taps != nullptr);
	
}

void test_func(){
	const std::int64_t filter_freq = 0;
	const std::size_t f_s = 2000000;
	const std::size_t N = 2000;
	const std::size_t taps = 100;

	RTT::PingFIR test_obj(filter_freq, f_s, 100);

	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<double> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		i_q.push(std::complex<double>(0.0001, 0));
		lock.unlock();
		i_cv.notify_all();
	}

	volatile bool run = true;

	run = false;
	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv, &run);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// sleep(2);
	std::cout << "notified" << std::endl;
	i_cv.notify_all();

	assert(i_q.empty());


}

int main(int argc, char const *argv[])
{
	test_constructor();
	test_func();
	return 0;
}