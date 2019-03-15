#include "dspv3.hpp"

#define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{
	DSP_V3::DSP_V3(const std::size_t sampling_freq) : _iq_data_queue(), 
		_iq_mux(), _iq_cv(), _fir(), _mag_data_queue(), _mag_mux(), _mag_cv(), 
		_int(INT_FACTOR), _candidate_queue(), _can_mux(), _can_cv(),
		_clfr(0, (double) sampling_freq / INT_FACTOR){
		
	}

	DSP_V3::~DSP_V3(){

	}

	void DSP_V3::startProcessing(std::queue<IQdataPtr>& i_q, std::mutex& i_m,
		std::condition_variable& i_v, std::queue<PingPtr>& o_q, std::mutex& o_m,
		std::condition_variable& o_v){
		_in_v = &i_v;
		_run = true;
		_thread = new std::thread(&DSP_V3::_unpack, this, std::ref(i_q), 
			std::ref(i_m), std::ref(i_v));
		_fir.start(_iq_data_queue, _iq_mux, _iq_cv, _mag_data_queue, _mag_mux,
			_mag_cv);
		_int.start(_mag_data_queue, _mag_mux, _mag_cv, _candidate_queue, 
			_can_mux, _can_cv);
		_clfr.start(_candidate_queue, _can_mux, _can_cv, o_q, o_m, o_v);
	}

	void DSP_V3::stopProcessing(){
		_in_v->notify_all();
		_run = false;
		_thread->join();

		#ifdef DEBUG
		std::cout << "DSP_V3 is stopping:" << std::endl;
		std::cout << "_fir has " << _iq_data_queue.size() << " data waiting, stopping" << std::endl;
		#endif
		_fir.stop();
		#ifdef DEBUG
		std::cout << "_fir has " << _iq_data_queue.size() << " waiting after being stopped" << std::endl;
		std::cout << "_int has " << _mag_data_queue.size() << " data waiting, stopping" << std::endl;
		#endif
		_int.stop();
		#ifdef DEBUG
		std::cout << "_int has " << _mag_data_queue.size() << " waiting after being stopped" << std::endl;
		std::cout << "_clfr has " << _candidate_queue.size() << " data waiting, stopping" << std::endl;
		#endif
		_clfr.stop();
		#ifdef DEBUG
		std::cout << "_clfr has " << _candidate_queue.size() << " waiting after being stopped" << std::endl;
		#endif

		delete _thread;
	}

	std::size_t IQdataToDouble(IQdataPtr dataObj, std::vector<std::complex<double>>& data){
		std::vector<short_cpx>& short_vec = *dataObj->data;
		data.resize(short_vec.size());
		std::size_t i = 0;
		for(; i < short_vec.size(); i++){
			data[i] = std::complex<double>(short_vec[i].real() / 4096.0, short_vec[i].imag() / 4096.0);
		}
		return i;
	}

	void DSP_V3::_unpack(std::queue<IQdataPtr>& i_q, std::mutex& i_m, 
		std::condition_variable& i_v){
		
		// Local vars
		std::vector<std::complex<double>> double_data{};

		// Loop
		while(_run || !i_q.empty()){
			// get data
			std::unique_lock<std::mutex> inputLock(i_m);
			if(i_q.empty()){
				i_v.wait(inputLock);
			}
			if(!i_q.empty()){
				IQdataPtr dataObj = i_q.front();
				i_q.pop();
				inputLock.unlock();

				IQdataToDouble(dataObj, double_data);

				// update time_start_ms
				if(time_start_ms == 0){
					// hasn't been set
					time_start_ms = dataObj->time_ms;
					// TODO: update classifier!
					_clfr.setStartTime(dataObj->time_ms);
				}

				std::unique_lock<std::mutex> dataLock(_iq_mux);
				for(std::size_t j = 0; j < double_data.size(); j++){
					_iq_data_queue.push(double_data[j]);
				}
				dataLock.unlock();
				_iq_cv.notify_all();
			}
		}
	}
}