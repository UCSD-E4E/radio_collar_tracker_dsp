#include "dspv2.hpp"
#include "Processor.hpp"
#include "iq_data.hpp"
#include <iostream>
#include <complex.h>
#include <syslog.h>

namespace RTT{

	typedef std::complex<double> cmpx;

	DSP_V2::DSP_V2(const std::vector<uint32_t>& freqs2process, 
		const std::size_t center_freq, const std::size_t sampling_freq) : 
		_center_freq(center_freq),
		_sampling_freq(sampling_freq), 
		_processor(freqs2process[0], center_freq, sampling_freq),
		_innerQueue{},
		_innerMutex{},
		_innerVar{}
		// _frequencies(),
		{
		
		// _frequencies.insert(_frequencies.end(), freqs2process.cbegin(), 
		// 	freqs2process.cend());
		_frequency = freqs2process[0];
		// _innerQueues = new std::queue<std::complex<double> >[_frequencies.size()];
		// _innerQueue{};
		// _innerMutexes = new std::mutex*[_frequencies.size()];
		// _innerMutex{};
		// _innerVars = new std::condition_variable[_frequencies.size()];
		// _innerVar{};
		// _processors = new Processor*[_frequencies.size()];
		// _processors = new Processor*[_frequencies.size()];
		// for(std::size_t i = 0; i < _frequencies.size(); i++){
			// _innerMutexes[i] = new std::mutex();
			// _processors[i] = new Processor(_frequencies[i], center_freq, 
			// 	sampling_freq);
		// }
	}

	DSP_V2::~DSP_V2(){
		// for(std::size_t i = 0; i < _frequencies.size(); i++){
		// 	delete _innerMutexes[i];
		// 	delete _processors[i];
		// }
		// delete[] _innerQueues;
		// delete[] _innerMutexes;
		// delete[] _innerVars;
		if(_copy_thread){
			delete _copy_thread;
		}
	}

	void DSP_V2::startProcessing(std::queue<IQdataPtr>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex, 
		std::condition_variable& output_cv, const volatile bool* run){

		syslog(LOG_INFO, "DSP Starting threads");
		_input_var = &input_cv;

		syslog(LOG_DEBUG, "DSP Starting copy thread");
		_copy_thread = new std::thread(&DSP_V2::copyQueue, this, run,
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv));
		// for(std::size_t i = 0; i < _frequencies.size(); i++){
			// _processors[i]->start(_innerQueues[i], *_innerMutexes[i], 
			// 	_innerVars[i], output_queue, output_mutex, output_cv, run);
		// }

		syslog(LOG_INFO, "DSP Starting processor");
		_processor.start(_innerQueue, _innerMutex, _innerVar, output_queue, 
			output_mutex, output_cv, run);
	}

	std::vector<cmpx>* IQdataToDouble(IQdataPtr data_obj){
		std::vector<short_cpx>& short_vec= *data_obj->data;
		std::vector<cmpx>* retval = new std::vector<cmpx>(short_vec.size());
		std::vector<cmpx>& double_vec = *retval;
		for(std::size_t i = 0; i < double_vec.size(); i++){
			double_vec[i] = std::complex<double>(short_vec[i].real() / 4096.0,
				short_vec[i].imag() / 4096.0);
		}
		return retval;
	}

	void DSP_V2::copyQueue(const volatile bool* run, 
		std::queue<IQdataPtr>& inputQueue, std::mutex& inputMutex, 
		std::condition_variable& inputVar){

		// Thread body
		while(*run || !inputQueue.empty()){
			// Lock input queues
			std::unique_lock<std::mutex> inputLock(inputMutex);
			if(inputQueue.empty()){
				inputVar.wait(inputLock);
			}

			// If queue has something
			if(!inputQueue.empty()){
				// get data object
				IQdataPtr dataObj = inputQueue.front();
				// pop queue
				inputQueue.pop();
				// release mutex
				inputLock.unlock();
				// convert to doubles
				std::vector<cmpx>* double_data = IQdataToDouble(dataObj);

				// update time_start_ms
				if(time_start_ms == 0){
					// hasn't been set
					time_start_ms = dataObj->time_ms;
					std::cout << "Start time: " << time_start_ms << std::endl;
					_processor.set_start_time(time_start_ms);
				}

				// For each processing thread
				// for(std::size_t i = 0; i < _frequencies.size(); i++){
				// 	// Lock inner mutex
				// 	std::cout << "Getting lock" << std::endl;
				// 	std::unique_lock<std::mutex> lock(*_innerMutexes[i]);
				// 	// put data in
				// 	for(std::size_t j = 0; j < double_data.size(); j++){
				// 		_innerQueues[i].push(double_data[i]);
				// 	}
				// 	// unlock inner mutex
				// 	lock.unlock();
				// 	// Notify sleeping thread (if any)
				// 	_innerVars[i].notify_all();
				// }
				std::unique_lock<std::mutex> innerLock(_innerMutex);
				for(std::size_t j = 0; j < double_data->size(); j++){
					_innerQueue.push((*double_data)[j]);
				}
				innerLock.unlock();
				_innerVar.notify_all();
				delete double_data;
			}
		}
	}

	void DSP_V2::stopProcessing(){
		if(_input_var){
			_input_var->notify_all();
		}
		if(_copy_thread){
			_copy_thread->join();
		}
		// for(std::size_t i = 0; i < _frequencies.size(); i++){
		// 	_processors[i]->stop();
		// }
		_processor.stop();
	}
}