#include "mixer.hpp"
#include "resampler.hpp"
#include "ping_fir.hpp"
#include "ping_classifier.hpp"
#include <utility.hpp>
#include <complex>

#define private public
#define protected public
#include <Processor.hpp>
#undef private
#undef protected

#include <cassert>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <ping.hpp>
#include <unistd.h>

void testSimulated(){
	const std::size_t t1 = 100000;
	const std::size_t f_s = 2000000;
	const std::size_t f_c = 172500000;
	const std::size_t N = 2000000;
	std::complex<double>* ping = RTT::generateSinusoid(t1, f_s, (int)(0.06 * f_s));
	std::complex<double>* test_signal = new std::complex<double>[N];

	std::size_t j = 0;
	for(; j < 3000; j++){
		test_signal[j] = 0.0001;
	}

	for(; j < 3000 + (int)(0.06 * f_s); j++){
		test_signal[j] = ping[j - 3000];
	}
	for(; j < N; j++){
		test_signal[j] = 0.0001;
	}

	RTT::Processor _proc{f_c + t1, f_c, f_s};
	
	std::queue<std::complex<double>> i_q;
	std::mutex i_mux;
	std::condition_variable i_cv;

	std::queue<RTT::PingPtr> o_q;
	std::mutex o_mux;
	std::condition_variable o_cv;

	bool run = true;

	_proc.start(i_q, i_mux, i_cv, o_q, o_mux, o_cv, &run);
	for(std::size_t i = 0; i < N; i++){
		std::unique_lock<std::mutex> lock(i_mux);
		i_q.push(test_signal[i]);
		lock.unlock();
		i_cv.notify_all();
	}
	sleep(1);
	run = false;

	assert(i_q.empty());
	std::cout << o_q.size() << std::endl;



	_proc.stop();
	delete(test_signal);
	delete(ping);
}

void testRecorded(){

}

int main(int argc, char const *argv[])
{
	testSimulated();
	testRecorded();
	return 0;
}