#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <tagged_signal.hpp>

#define private public
#define protected public
#include <integrator.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>
#include <cmath>
#include <utility.hpp>
#include <stdlib.h>

const std::size_t f_s = 250000;

void test_constructor(){
	RTT::Integrator test_obj{6400};
	assert(test_obj._decimation == 6400);
}

void test_throughput(){
	const std::size_t N = 250000;

	RTT::Integrator test_obj{6400};

	std::queue<RTT::TaggedSignal*> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::TaggedSignal*> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	srand(1);
	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		auto vec = new std::vector<std::complex<double>>();
		vec->push_back(std::complex<double>(double(rand() / RAND_MAX)));
		auto sig = new RTT::TaggedSignal(0.0001*i, *vec);
		i_q.push(sig);
		lock.unlock();
		i_cv.notify_all();
	}

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/f_s) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / f_s);
	assert(i_q.empty());

	const std::size_t k = o_q.size();
	for(std::size_t i = 0; i < k; i++){
		auto tsig = o_q.front();
		o_q.pop();
		delete tsig->sig;
		delete tsig;
	}
}

void test_buffer_frame(){
	const std::size_t N = 50;
	const std::size_t buffer_size = 6400;

	RTT::Integrator test_obj{buffer_size};

	std::queue<RTT::TaggedSignal*> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::TaggedSignal*> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	srand(1);
	for(std::size_t i = 0; i < N; i++){
		for(std::size_t j = 0; j < buffer_size; j++){
			std::unique_lock<std::mutex> lock(i_mux);
			auto vec = new std::vector<std::complex<double>>();
			vec->push_back(std::complex<double>(rand()));
			auto sig = new RTT::TaggedSignal((i + 1) / (double)buffer_size, *vec);
			i_q.push(sig);
			lock.unlock();
			i_cv.notify_all();
		}
	}
	
	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N * buffer_size/f_s) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N * buffer_size / f_s);
	i_cv.notify_all();
	assert(i_q.empty());
	assert(o_q.size() == N);
	srand(1);
	for(std::size_t i = 0; i < N; i++){
		RTT::TaggedSignal* test_sig = o_q.front();
		o_q.pop();
		double test_val = test_sig->val;
		// std::cout << test_val << ", " << RTT::powerToDB(i+1) / 10 << std::endl;
		assert(std::fabs(test_val - RTT::powerToDB(i+1) / 10) < 0.01);
		assert(test_sig->sig->size() == 6400);
		std::vector<std::complex<double>>& sig = *test_sig->sig;
		for(std::size_t j = 0; j < test_sig->sig->size(); j++){
			// std::cout << i << ", " << j << std::endl;
			auto expected_val = rand();
			auto actual_val = sig[j].real();
			// std::cout << std::abs(actual_val / expected_val) << std::endl;
			assert(std::abs(actual_val / expected_val - 1) < 0.01);
		}
		delete test_sig->sig;
		delete test_sig;
	}
}

int main(int argc, char const *argv[]){
	test_constructor();
	test_throughput();
	test_buffer_frame();
	return 0;
}