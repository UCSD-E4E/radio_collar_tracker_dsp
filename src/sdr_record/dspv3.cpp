#include "dspv3.hpp"
#include "tagged_signal.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>
// #define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

namespace RTT{
	DSP_V3::DSP_V3(const std::size_t sampling_freq, 
		const std::size_t center_freq) : _iq_data_queue(), _iq_mux(), _iq_cv(),
		_fir(), _mag_data_queue(), _mag_mux(), _mag_cv(),
		int_factor(int_time_s * sampling_freq), 
		_int(int_factor), _candidate_queue(), _can_mux(), _can_cv(),
		_clfr(0, (double) sampling_freq / int_factor, sampling_freq, 
			center_freq), _output_fmt{nullptr}{
		
	}

	DSP_V3::~DSP_V3(){
		if(_output_fmt != nullptr){
			delete[] _output_fmt;
		}
	}

	void DSP_V3::startProcessing(std::queue<IQdataPtr>& i_q, std::mutex& i_m,
		std::condition_variable& i_v, std::queue<PingPtr>& o_q, std::mutex& o_m,
		std::condition_variable& o_v){
		_in_v = &i_v;
		_run = true;
		_thread = new std::thread(&DSP_V3::_unpack, this, std::ref(i_q), 
			std::ref(i_m), std::ref(i_v));
		// _fir.start(_iq_data_queue, _iq_mux, _iq_cv, _mag_data_queue, _mag_mux,
		// 	_mag_cv);
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
		// std::cout << "_fir has " << _iq_data_queue.size() << " data waiting, stopping" << std::endl;
		#endif
		// _fir.stop();
		#ifdef DEBUG
		// std::cout << "_fir has " << _iq_data_queue.size() << " waiting after being stopped" << std::endl;
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
		
		std::size_t file_counter = 0;
		std::size_t sample_counter = 0;
		char fname[1024];
		sprintf(fname, _output_fmt, file_counter + 1);
		std::ofstream data_str;
		if(!_output_dir.empty()){
			data_str.open(fname, std::ofstream::binary);
			std::cout << "Opening " << fname << std::endl;
		}

		int16_t buffer[2];

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

				if(!_output_dir.empty()){
					for(std::size_t i = 0; i < dataObj->size(); i++){
						buffer[0] = (*(dataObj->data))[i].real();
						buffer[1] = (*(dataObj->data))[i].imag();
						data_str.write((char*)buffer, 2*sizeof(int16_t));
						sample_counter++;

						if(sample_counter >= SAMPLES_PER_FILE){
							data_str.close();
							file_counter++;
							sprintf(fname, _output_fmt, file_counter + 1);
							std::cout << "Opening " << fname << std::endl;
							data_str.open(fname, std::ofstream::binary);
							sample_counter = 0;
						}
					}
					data_str.flush();
				}

				// update time_start_ms
				if(time_start_ms == 0){
					// hasn't been set
					time_start_ms = dataObj->time_ms;
					// TODO: update classifier!
					_clfr.setStartTime(dataObj->time_ms);
				}

				std::unique_lock<std::mutex> dataLock(_mag_mux);
				for(std::size_t j = 0; j < double_data.size(); j++){
					double data = std::abs(double_data[j]) * std::abs(double_data[j]);
					auto sig = new std::vector<std::complex<double>>();
					sig->push_back(double_data[j]);
					auto tsig = new TaggedSignal(data, *sig);
					_mag_data_queue.push(tsig);
				}
				dataLock.unlock();
				_mag_cv.notify_all();
			}

			// if(!_output_dir.empty()){
			// 	data_str.write((char*)raw_buffer, rx_buffer_size * 2 * sizeof(int16_t));
			// 	data_str.flush();
			// 	if(frame_counter++ == FRAMES_PER_FILE){
			// 		data_str.close();
			// 		sprintf(fname, _output_fmt, file_counter++);
			// 		std::cout << "Opening " << fname << std::endl;
			// 		data_str.open(fname, std::ofstream::binary);
			// 		frame_counter = 0;
			// 	}
			// }
		}
		if(!_output_dir.empty()){
			data_str.close();
		}
	}

	void DSP_V3::setStartTime(std::size_t start_time_ms){
		time_start_ms = start_time_ms;
		_clfr.setStartTime(time_start_ms);
	}

	void DSP_V3::setOutputDir(const std::string dir, const std::string fmt){
		_output_dir = dir;
		_output_fmt = new char[fmt.length() + dir.length() + 1];
		sprintf(_output_fmt, "%s/%s", dir.c_str(), fmt.c_str());
	}
}