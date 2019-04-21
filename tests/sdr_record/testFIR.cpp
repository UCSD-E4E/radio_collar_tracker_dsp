#include <queue>
#include <complex>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <fir.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>

const std::size_t f_s = 250000;

void test_constructor(){
	RTT::FIR test_obj();
	assert(true);
}

void test_throughput(){
	const std::size_t N = 250000;

	RTT::FIR test_obj{};

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

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Nonthreaded Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/f_s) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / f_s);
	i_cv.notify_all();
	assert(i_q.empty());
}

void test_threaded(){
	const std::size_t N = 250000;

	RTT::FIR test_obj{};

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

	auto start = std::chrono::steady_clock::now();
	test_obj.start(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	test_obj.stop();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Threaded Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/f_s) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / f_s);
	i_cv.notify_all();
	assert(i_q.empty());
}

void test_response(){
	RTT::FIR test_obj{};

	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<double> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	std::unique_lock<std::mutex> lock(i_mux);
	i_q.push(std::complex<double>(1, 0));
	lock.unlock();
	i_cv.notify_all();
	for(std::size_t i = 1; i < test_obj._num_taps; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		i_q.push(std::complex<double>(0, 0));
		lock.unlock();
		i_cv.notify_all();
	}

	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	i_cv.notify_all();
	assert(i_q.empty());
	for(std::size_t i = 0; i < test_obj._num_taps; i++){
		// std::cout << std::abs(o_q.front() - test_obj._TAPS[i]) << std::endl;
		assert(std::abs(o_q.front() - test_obj._TAPS[i]) < 0.001);
		o_q.pop();
	}
}

int main(int argc, char const *argv[]){
	test_constructor();
	test_throughput();
	test_response();
	test_threaded();
	return 0;
}