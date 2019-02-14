#include "dspv2.hpp"
#include "Processor.hpp"
#include "iq_data.hpp"
#include <iostream>
#include <complex.h>
#include <syslog.h>

#define DEBUG

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{

	typedef std::complex<double> cmpx;

	DSP_V2::DSP_V2(const std::vector<uint32_t>& freqs2process, 
		const std::size_t center_freq, const std::size_t sampling_freq,
		const std::size_t frame_size) : 
		_center_freq(center_freq),
		_sampling_freq(sampling_freq), 
		_frequency(freqs2process[0]),
		_processor(freqs2process[0], center_freq, sampling_freq),
		_innerQueue{},
		_innerMutex{},
		_innerVar{},
		_frame_size{frame_size}
		{
		
	}

	DSP_V2::~DSP_V2(){
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

	std::size_t IQdataToDouble(IQdataPtr data_obj, std::complex<double>* data, std::size_t N){
		std::vector<short_cpx>& short_vec =*data_obj->data;
		std::size_t i = 0;
		for(; i < short_vec.size() && i < N; i++){
			data[i] = std::complex<double>(short_vec[i].real() / 4096.0, short_vec[i].imag() / 4096.0);
		}
		return i;
	}

	std::size_t IQdataToDouble(IQdataPtr data_obj, std::vector<std::complex<double>>& data, std::size_t N){
		std::vector<short_cpx>& short_vec =*data_obj->data;
		std::size_t i = 0;
		for(; i < short_vec.size() && i < N; i++){
			data[i] = std::complex<double>(short_vec[i].real() / 4096.0, short_vec[i].imag() / 4096.0);
		}
		return i;
	}

	void DSP_V2::copyQueue(const volatile bool* run, 
		std::queue<IQdataPtr>& inputQueue, std::mutex& inputMutex, 
		std::condition_variable& inputVar){


		#ifdef DEBUG
		std::ofstream _ostr{"dspv2_copy.log"};
		std::size_t _output_idx = 0;
		std::size_t _output_idx_2 = 0;
		#endif
		// std::complex<double>* double_data = new std::complex<double>[_frame_size];
		std::vector<std::complex<double>>* double_data = new std::vector<std::complex<double>>{};
		double_data->resize(_frame_size);
		std::cout << "_frame_size: " << _frame_size << std::endl;
		

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
				#ifdef DEBUG
				_output_idx_2++;
				#endif
				// release mutex
				inputLock.unlock();
				// convert to doubles
				std::size_t numVals = IQdataToDouble(dataObj, *double_data, _frame_size);

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
				for(std::size_t j = 0; j < _frame_size; j++){
					_innerQueue.push((*double_data)[j]);
					#ifdef DEBUG
					_output_idx++;
					_ostr << (*double_data)[j].real();
					if((*double_data)[j].imag() >= 0){
						_ostr << "+";
					}
					_ostr << (*double_data)[j].imag() << "i" << std::endl;
					#endif
				}
				innerLock.unlock();
				_innerVar.notify_all();
			}
		}

		#ifdef DEBUG
		_ostr.close();
		std::cout << "DSP Copy received " << _output_idx_2 << " data objects" << std::endl;
		std::cout << "DSP Copy output " << _output_idx << " samples" << std::endl;
		#endif
		delete double_data;
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