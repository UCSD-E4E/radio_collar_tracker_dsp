#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#define private public
#define protected public
#include <integrator.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>
#include <cmath>
#include <utility.hpp>

const std::size_t f_s = 250000;

void test_constructor(){
	RTT::Integrator test_obj{6400};
	assert(test_obj._decimation == 6400);
}

void test_throughput(){
	const std::size_t N = 250000;

	RTT::Integrator test_obj{6400};

	std::queue<double> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<double> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		i_q.push(0.0001 * i);
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
}

void test_buffer_frame(){
	const std::size_t N = 50;
	const std::size_t buffer_size = 6400;

	RTT::Integrator test_obj{buffer_size};

	std::queue<double> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<double> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	for(std::size_t i = 0; i < N; i++){
		for(std::size_t j = 0; j < buffer_size; j++){
			std::unique_lock<std::mutex> lock(i_mux);
			i_q.push((i+1) / (double)buffer_size);
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
	for(std::size_t i = 0; i < N; i++){
		double test_val = o_q.front();
		o_q.pop();
		assert(std::fabs(test_val - RTT::amplitudeToDB(i+1)) < 0.01);
	}
}

int main(int argc, char const *argv[]){
	test_constructor();
	test_throughput();
	test_buffer_frame();
	return 0;
}