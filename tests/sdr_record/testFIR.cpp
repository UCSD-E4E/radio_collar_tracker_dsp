#include <queue>
#include <complex>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "tagged_signal.hpp"

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

	std::queue<RTT::TaggedSignal*> o_q;
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
	assert(o_q.size() == N);
	std::cout << "N: " << N << ", size: " << o_q.size() << std::endl;
	for(std::size_t i = 0; i < N; i++){
		RTT::TaggedSignal* tsig = o_q.front();
		o_q.pop();
		// std::cout << "Deleting vector " << tsig->sig << std::endl;
		delete tsig->sig;
		// std::cout << "Deleting TS " << tsig << std::endl;
		delete tsig;
	}
}

void test_threaded(){
	const std::size_t N = 250000;

	RTT::FIR test_obj{};

	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::TaggedSignal*> o_q;
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
	assert(o_q.size() == N);
	for(std::size_t i = 0; i < N; i++){
		auto tsig = o_q.front();
		o_q.pop();
		delete tsig->sig;
		delete tsig;
	}
}

void test_response(){
	RTT::FIR test_obj{};

	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::TaggedSignal*> o_q;
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
	std::size_t N = o_q.size();
	for(std::size_t i = 0; i < N; i++){
		// std::cout << std::abs(o_q.front() - test_obj._TAPS[i]) << std::endl;
		assert(std::abs(o_q.front()->val - test_obj._TAPS[i]) < 0.001);
		auto tsig = o_q.front();
		o_q.pop();
		delete tsig->sig;
		delete tsig;
	}
}

int main(int argc, char const *argv[]){
	std::cout << "Testing Constructor" << std::endl;
	test_constructor();
	std::cout << "Testing Throughput" << std::endl;
	test_throughput();
	std::cout << "Testing Response" << std::endl;
	test_response();
	std::cout << "Testing Threaded" << std::endl;
	test_threaded();
	return 0;
}