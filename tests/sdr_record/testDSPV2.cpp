// #define DEBUG

#include <vector>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <condition_variable>
#include <functional>
#include "dsp.hpp"
#include <thread>
#include <iostream>

#define private public
#define protected public
#include "dspv2.hpp"
#undef private
#undef protected

#include <cassert>
#include <chrono>
#include <utility.hpp>
#include <sdr_test.hpp>
#include <unistd.h>
#include <syslog.h>

#ifdef DEBUG
#include <fstream>
#endif

void testCopy_threaded(){
	std::cout << "Testing copy threads" << std::endl;
	std::vector<uint32_t> freqs;
	freqs.push_back(172500000);
	// freqs.push_back(172500001);
	// freqs.push_back(172500002);
	// freqs.push_back(172500003);

	RTT::DSP_V2 dsp(freqs, 172500000, 2000000, 1000);

	std::queue<RTT::IQdataPtr> dataQueue;
	std::mutex dataMutex;
	std::condition_variable dataVar;

	std::queue<RTT::PingPtr> pingQueue;
	std::mutex pingMutex;
	std::condition_variable pingVar;
	volatile bool ndie = true;
	std::thread copyThread(&RTT::DSP_V2::copyQueue, &dsp, &ndie,
		std::ref(dataQueue), std::ref(dataMutex), std::ref(dataVar));

	RTT::IQdataPtr data1(new RTT::IQdata(1024));
	std::unique_lock<std::mutex> dataLock(dataMutex);
	
	auto start = std::chrono::steady_clock::now();
	dataQueue.push(data1);
	dataLock.unlock();
	dataVar.notify_all();
	ndie = false;
	std::cout << "run is now false" << std::endl;
	dataVar.notify_all();
	copyThread.join();
	auto end = std::chrono::steady_clock::now();
	assert(dataQueue.empty());
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
	// 	1024.0 / 2000000));
	// for(std::size_t i = 0; i < dsp._frequencies.size(); i++){
	// 	assert(dsp._innerQueues[i].size() == 1024);
	// }
	assert(dsp._innerQueue.size() == 1024);
	std::cout << "Done testing copy threads" << std::endl;
}

void testCopy_func(){
	std::cout << "Testing copy function" << std::endl;
	std::vector<uint32_t> freqs;
	freqs.push_back(172500000);

	std::size_t N = 4096;

	RTT::DSP_V2 dsp(freqs, 172500000, 2000000, 1000);


	std::queue<RTT::IQdataPtr> dataQueue;
	std::mutex dataMutex;
	std::condition_variable dataVar;

	std::queue<RTT::PingPtr> pingQueue;
	std::mutex pingMutex;
	std::condition_variable pingVar;
	volatile bool ndie = false;

	RTT::IQdataPtr data1(new RTT::IQdata(N));
	dataQueue.push(data1);
	
	auto start = std::chrono::steady_clock::now();
	dsp.copyQueue(&ndie, dataQueue, dataMutex, dataVar);
	auto end = std::chrono::steady_clock::now();
	assert(dataQueue.empty());
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
	// 	N / 2000000.0));
	assert(dsp._innerQueue.size() == N);
	std::cout << "Done testing copy function" << std::endl;
}

void testDSP_throughput(){

	const std::size_t f1 = 100000;
	const std::size_t f_s = 2000000;
	const std::size_t f_c = 172500000;
	const std::size_t N = 1 * f_s;
	volatile bool run = true;

	const std::size_t samples_per_ping = 0.06 * f_s;
	std::complex<double>* ping = RTT::generateSinusoid(f1, f_s, samples_per_ping);
	std::complex<double>* test_signal = new std::complex<double>[N];

	std::cout << "Testing DSP threads" << std::endl;
	std::vector<uint32_t> freqs;
	freqs.push_back(f_c + f1);
	RTT::DSP_V2 testObj(freqs, f_c, f_s, 1000);

	std::queue<RTT::IQdataPtr> dataQueue{};
	std::mutex dataMutex{};
	std::condition_variable dataVar{};

	std::queue<RTT::PingPtr> pingQueue{};
	std::mutex pingMutex{};
	std::condition_variable pinVar{};
	
	#ifdef DEBUG
	std::ofstream _ostr{"test_signal.log"};
	std::size_t _out_idx = 0;
	#endif

	std::size_t j = 0;
	for(; j < 3000; j++){
		test_signal[j] = 1;
	}

	for(std::size_t i = 0; i < samples_per_ping; i++, j++){
		// test_signal[j] = 5;
		test_signal[j] = 4096.0 * ping[j - 3000];
	}
	for(; j < N; j++){
		test_signal[j] = 1;
	}

	for(std::size_t i = 0; i < N / 1000; i++){
		RTT::IQdataPtr data(new RTT::IQdata(0));
		for(std::size_t j = 0; j < 1000; j++){
			data->data->push_back(test_signal[i * 1000 + j]);
		}
		data->time_ms = 1000;
		std::unique_lock<std::mutex> data_lock{dataMutex};
		dataQueue.push(data);
		data_lock.unlock();
		dataVar.notify_all();
		#ifdef DEBUG
		for(auto it = data->data->begin(); it != data->data->end(); it++){
			_ostr << _out_idx++ << ", " << *it << std::endl;
		}
		#endif
	}

	// std::unique_lock<std::mutex> data_lock{dataMutex};
	// for(auto it = dataQueue.front(); it != dataQueue.back(); it++){
	// 	_ostr << _out_idx++ << ", " << (*it)->data << std::endl;	
	// }
	// data_lock.unlock();

	auto start = std::chrono::steady_clock::now();
	testObj.startProcessing(dataQueue, dataMutex, dataVar, pingQueue, pingMutex,
		pinVar, &run);
	// sleep(1);
	run = false;
	testObj.stopProcessing();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	// assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= N / f_s));
	assert(dataQueue.empty());
	assert(testObj._processor.queue2.empty());
	assert(!pingQueue.empty());
	assert(pingQueue.size() == 1);
	assert((pingQueue.front()->time_ms - 1000) / 1e3 < 0.06);
	std::cout << "Done testing DSP threads" << std::endl;
	#ifdef DEBUG
	_ostr.close();
	#endif

	delete[] ping;
	delete[] test_signal;
}

void test_signal(){
	openlog("DSPV2 Test signal", LOG_PID | LOG_PERROR, LOG_USER);
	setlogmask(LOG_UPTO(LOG_INFO));
	const std::size_t f_1 =		172017000;
	const std::size_t f_s = 	  2000000;
	const std::size_t f_c = 	172500000;
	const std::size_t rbuf = 	    16384;
	const std::string d_loc = "/home/ntlhui/workspace/tmp/testData";
	volatile bool run = true;

	RTT::SDR_TEST sdr{d_loc};
	std::queue<RTT::IQdataPtr> iq_q{};
	std::mutex iq_mux{};
	std::condition_variable iq_cv{};

	std::vector<uint32_t> freqs{};
	freqs.push_back(f_1);
	RTT::DSP_V2 dsp(freqs, f_c, f_s, rbuf);
	std::queue<RTT::PingPtr> ping_q{};
	std::mutex ping_mux{};
	std::condition_variable ping_cv{};

	auto start = std::chrono::steady_clock::now();
	sdr.startStreaming(iq_q, iq_mux, iq_cv, &run);
	dsp.startProcessing(iq_q, iq_mux, iq_cv, ping_q, ping_mux, ping_cv, &run);
	// sleep(1);
	sdr.stopStreaming();
	run = false;
	dsp.stopProcessing();
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration<double, std::ratio<1, 1>>(diff).count() << "s" << std::endl;


}

int main(int argc, char const *argv[])
{
	testCopy_threaded();
	testCopy_func();
	testDSP_throughput();
	test_signal();
	return 0;
}