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

void testCopy_threaded(){
	std::cout << "Testing copy threads" << std::endl;
	std::vector<uint32_t> freqs;
	freqs.push_back(172500000);
	// freqs.push_back(172500001);
	// freqs.push_back(172500002);
	// freqs.push_back(172500003);

	RTT::DSP_V2 dsp(freqs, 172500000, 2000000);

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
		// 1024.0 / 2000000));
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

	RTT::DSP_V2 dsp(freqs, 172500000, 2000000);


	std::queue<RTT::IQdataPtr> dataQueue;
	std::mutex dataMutex;
	std::condition_variable dataVar;

	std::queue<RTT::PingPtr> pingQueue;
	std::mutex pingMutex;
	std::condition_variable pingVar;
	volatile bool ndie = false;

	RTT::IQdataPtr data1(new RTT::IQdata(1024));
	dataQueue.push(data1);
	
	auto start = std::chrono::steady_clock::now();
	dsp.copyQueue(&ndie, dataQueue, dataMutex, dataVar);
	auto end = std::chrono::steady_clock::now();
	assert(dataQueue.empty());
	auto diff = end - start;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		1024.0 / 2000000));
	assert(dsp._innerQueue.size() == 1024);
	std::cout << "Done testing copy function" << std::endl;
}

void testDSP_throughput(){
	std::cout << "Testing DSP threads" << std::endl;
	std::vector<uint32_t> freqs;
	freqs.push_back(172000000);
	RTT::DSP_V2 testObj(freqs, 172000000, 2000000);

	std::queue<RTT::IQdataPtr> dataQueue{};
	std::mutex dataMutex{};
	std::condition_variable dataVar{};

	std::queue<RTT::PingPtr> pingQueue{};
	std::mutex pingMutex{};
	std::condition_variable pinVar{};
	std::queue<std::complex<short>> inputSignal{};

	volatile bool run = true;
	std::size_t samples_per_ping = 0.06 * 2000000;
	for(std::size_t i = 0; i < samples_per_ping; i++){
		inputSignal.push(std::complex<short>(55, 0));
	}
	for(std::size_t i = 0; i < 2000000 - samples_per_ping; i++){
		inputSignal.push(std::complex<short>(1, 0));
	}
	for(std::size_t i = 0; i < inputSignal.size() / 1000; i++){
		RTT::IQdataPtr data(new RTT::IQdata(1000));
		for(std::size_t j = 0; j < 1000; j++){
			data->data->push_back(inputSignal.front());
			inputSignal.pop();
		}
		data->time_ms = 1000;
		std::unique_lock<std::mutex> data_lock{dataMutex};
		dataQueue.push(data);
		data_lock.unlock();
		dataVar.notify_all();
	}
	auto start = std::chrono::steady_clock::now();
	testObj.startProcessing(dataQueue, dataMutex, dataVar, pingQueue, pingMutex,
		pinVar, &run);
	// sleep(1);
	run = false;
	testObj.stopProcessing();
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Duration: " << std::chrono::duration <double, std::ratio<1, 1>> (diff).count() << "s" << std::endl;
	assert((std::chrono::duration <double, std::ratio<1, 1>> (diff).count() <= 
		1));
	assert(dataQueue.empty());
	assert(testObj._processor.queue1.empty());
	assert(!pingQueue.empty());
	assert(pingQueue.size() == 1);
	assert((pingQueue.front()->time_ms - 1000) / 1e3 < 0.06);
	std::cout << "Done testing DSP threads" << std::endl;
}

int main(int argc, char const *argv[])
{
	testCopy_threaded();
	testCopy_func();
	testDSP_throughput();
	return 0;
}