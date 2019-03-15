#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <classifier.hpp>
#undef protected
#undef private

#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cmath>

const std::size_t f_s = 250000;

void testConstructor(){
	RTT::Classifier test_obj{0, f_s / 800.0};
}

void test_throughput(){
	const std::size_t N = 312;

	RTT::Classifier test_obj{0, f_s / 800.0, 0};

	std::queue<double> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::PingPtr> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		i_q.push(0);
		lock.unlock();
		i_cv.notify_all();
	}

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/(f_s / 800.0)) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / (f_s / 800.0));
	i_cv.notify_all();
	assert(i_q.empty());
}

void test_response(){
	const std::size_t N = 50;
	const std::size_t period_ms = 1000;
	const std::size_t pulse_width_ms = 60;
	const double ping_amplitude = 5;

	RTT::Classifier test_obj{0, f_s / 800.0, 0};

	std::queue<double> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::PingPtr> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	for(std::size_t i = 0; i < N; i++){
		for(std::size_t j = 0; j < pulse_width_ms /1000.0 * f_s / 800.0; j++){
			i_q.push(ping_amplitude);
		}
		for(std::size_t j = 0; j < (period_ms - pulse_width_ms) /1000.0 * f_s / 800.0; j++){
			i_q.push(0);
		}
	}

	auto start = std::chrono::steady_clock::now();
	test_obj._process(i_q, i_mux, i_cv, o_q, o_mux, o_cv);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s, ";
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)(N * period_ms / 1000.0)/(f_s / 800.0)) * 100 << "%" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) (N * period_ms / 1000.0) / (f_s / 800.0));
	i_cv.notify_all();
	assert(i_q.empty());
	std::cout << o_q.size() << std::endl;
	assert(o_q.size() == N);
	for(std::size_t i = 0; i < N; i++){
		RTT::PingPtr ping = o_q.front();
		assert(std::abs(ping->time_ms - i * period_ms) < 100);
		assert(std::fabs(ping->amplitude - ping_amplitude) < 0.1);
		o_q.pop();
	}
}

int main(int argc, char const *argv[])
{
	testConstructor();
	test_throughput();
	test_response();
	return 0;
}