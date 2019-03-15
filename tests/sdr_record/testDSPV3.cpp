#include <dsp.hpp>
#include <complex>

#define private public
#define protected public
#include <dspv3.hpp>
#undef protected
#undef private

#include <cassert>
#include <iostream>
#include <utility.hpp>
#include <random>
#include <fstream>
#include <cmath>
#include <unistd.h>

const std::size_t f_s = 250000;

void test_throughput(){
	const std::size_t f1 = 1000;
	const std::size_t t = 3;
	const std::size_t N = t * f_s;
	const double mag_k = 4096.0;
	const std::size_t ping_offset_samples = 1000;
	const double ping_offset_ms = ping_offset_samples / f_s * 1000;

	const std::size_t samples_per_ping = 0.06 * f_s;
	std::complex<double>* ping = RTT::generateSinusoid(f1, f_s, samples_per_ping);
	std::complex<double>* test_signal = new std::complex<double>[N];

	RTT::DSP_V3 test_obj{f_s};

	std::queue<RTT::IQdataPtr> dataQueue{};
	std::mutex dataMutex{};
	std::condition_variable dataVar{};

	std::queue<RTT::PingPtr> pingQueue{};
	std::mutex pingMutex{};
	std::condition_variable pingVar{};

	// for noise
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> dis{0, 1e-3};

	std::size_t j = 0;

	for(; j < ping_offset_samples; j++){
		test_signal[j] = mag_k * dis(gen);
	}

	for(std::size_t i = 0; i < samples_per_ping; i++, j++){
		test_signal[j] = mag_k * (ping[j - ping_offset_samples] + dis(gen));
	}

	for(; j < N; j++){
		test_signal[j] = mag_k * dis(gen);
	}

	std::ofstream _ostr{"test_signal.log"};

	for(std::size_t i = 0; i < N / 1000; i++){
		RTT::IQdataPtr data(new RTT::IQdata(0));
		for(std::size_t j = 0; j < 1000; j++){
			data->data->push_back(test_signal[i * 1000 + j]);
		}
		data->time_ms = 1000;
		dataQueue.push(data);
		for(auto it = data->data->begin(); it != data->data->end(); it++){
			_ostr << it->real();
			if(it->imag() >= 0){
				_ostr << "+";
			}
			_ostr << it->imag() << "i" << std::endl;
		}
	}
	_ostr.close();

	auto start = std::chrono::steady_clock::now();
	test_obj.startProcessing(dataQueue, dataMutex, dataVar, pingQueue, 
		pingMutex, pingVar);
	sleep(t/2);
	test_obj.stopProcessing();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/(f_s)) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / (f_s / 800.0));
	assert(dataQueue.empty());
	assert(test_obj._iq_data_queue.empty());
	std::cout << test_obj._mag_data_queue.size() << std::endl;
	assert(test_obj._mag_data_queue.empty());
	assert(test_obj._candidate_queue.empty());
	assert(pingQueue.size() == 1);
	std::cout << pingQueue.front()->time_ms << std::endl;
	assert(std::abs(pingQueue.front()->time_ms - ping_offset_ms - 1000) < 60);

	delete[] ping;
	delete[] test_signal;
}

void test_constructor(){
	RTT::DSP_V3 test_obj{f_s};
}

void test_unpack(){
	std::size_t N = 4096;

	RTT::DSP_V3 test_obj{f_s};

	std::queue<RTT::IQdataPtr> dataQueue;
	std::mutex dataMutex;
	std::condition_variable dataVar;

	RTT::IQdataPtr data1(new RTT::IQdata(N));
	dataQueue.push(data1);

	auto start = std::chrono::steady_clock::now();
	test_obj._unpack(dataQueue, dataMutex, dataVar);
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;

	assert(dataQueue.empty());
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	std::cout << "Usage: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() / ((double)N/(f_s)) * 100 << "%" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count()) < (double) N / (f_s));
	assert(test_obj._iq_data_queue.size() == N);

}

int main(int argc, char const *argv[])
{
	test_constructor();
	test_unpack();
	test_throughput();
	return 0;
}