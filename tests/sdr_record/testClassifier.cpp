#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "tagged_signal.hpp"

#define private public
#define protected public
#include <classifier.hpp>
#undef protected
#undef private

#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <complex>
#include "utility.hpp"

const std::size_t f_s = 250000;

void testConstructor(){
	RTT::Classifier test_obj{0, f_s / 800.0, f_s};
}

void test_throughput(){
	const std::size_t N = 312;

	RTT::Classifier test_obj{0, f_s / 800.0, f_s, 0};

	std::queue<RTT::TaggedSignal*> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::PingPtr> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	srand(1);
	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		auto sig = RTT::generateVectorSinusoid(100, f_s, 800);
		auto tsig = new RTT::TaggedSignal(0, *sig);
		i_q.push(tsig);
		lock.unlock();
		i_cv.notify_all();
	}
	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		auto sig = RTT::generateVectorSinusoid(100, f_s, 800);
		auto tsig = new RTT::TaggedSignal(0, *sig);
		i_q.push(tsig);
		lock.unlock();
		i_cv.notify_all();
	}

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / (2*(double)N/(f_s / 800.0)) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) 2*N / (f_s / 800.0));
	i_cv.notify_all();
	assert(i_q.empty());
}

void test_response(){
	const std::size_t N = 50;
	const std::size_t period_ms = 1000;
	const std::size_t pulse_width_ms = RTT::Classifier::ping_width_ms;
	const double ping_amplitude = 5;
	const std::int64_t ping_freq = 1500;

	RTT::Classifier test_obj{0, f_s / 800.0, f_s, 0};

	std::queue<RTT::TaggedSignal*> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::PingPtr> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	srand(1);
	for(std::size_t i = 0; i < N; i++){
		for(std::size_t j = 0; j < pulse_width_ms /1000.0 * f_s / 800.0; j++){
			auto sig = RTT::generateVectorSinusoid(ping_freq, f_s, 800);
			auto tsig = new RTT::TaggedSignal(ping_amplitude, *sig);
			i_q.push(tsig);
		}
		for(std::size_t j = 0; j < (period_ms - pulse_width_ms) /1000.0 * f_s / 800.0; j++){
			auto sig = new std::vector<std::complex<double>>();
			sig->reserve(800);
			for(std::size_t i = 0; i < 800; i++){
				sig->push_back(std::complex<double>(rand() / RAND_MAX * 1e-6));
			}
			auto tsig = new RTT::TaggedSignal(0, *sig);
			i_q.push(tsig);
		}
	}

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / (double)(N * period_ms / 1000.0) * 100 << "%" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) (N * period_ms / 1000.0) / (f_s / 800.0));
	i_cv.notify_all();
	assert(i_q.empty());
	// std::cout << o_q.size() << std::endl;
	assert(o_q.size() == N - 1); // classifier will ignore the first since it starts at the beginning!
	for(std::size_t i = 1; i < N; i++){
		RTT::PingPtr ping = o_q.front();
		assert(std::abs(ping->time_ms - i * period_ms) < 100);
		assert(std::fabs(ping->amplitude - ping_amplitude) < 0.1);
		assert(std::abs(ping->frequency - ping_freq) < 1e3);
		o_q.pop();
	}
}

int main(int argc, char const *argv[])
{
	std::cout << "Testing Constructor" << std::endl;
	testConstructor();

	std::cout << "Testing Throughput" << std::endl;
	test_throughput();

	std::cout << "Testing Response" << std::endl;
	test_response();
	return 0;
}