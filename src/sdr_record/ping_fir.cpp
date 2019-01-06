#include "ping_fir.hpp"
#include "utility.hpp"
#include <list>
#include <complex>
#include <functional>
#include <iostream>
#include <limits>

#include <boost/circular_buffer.hpp>

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	PingFIR::PingFIR(std::size_t frequency, std::size_t sampling_frequency,
		std::size_t size) : 
		_num_taps(size), 
		_input_cv(nullptr),
		_thread(nullptr)
		{
		// generate filter taps
		_filter_taps = generateSinusoid(frequency, sampling_frequency, 
			_num_taps, 1.0);
	}

	PingFIR::~PingFIR(){
		delete[] _filter_taps;
	}

	void PingFIR::start(std::queue<std::complex<double>>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<double>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv, const volatile bool* run){
		// Store blocking condition variable to wake on later
		_input_cv = &input_cv;
		// Start thread
		_thread = new std::thread(&PingFIR::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv), 
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv),
			run);
	}

	void PingFIR::stop(){
		// wake up _process
		_input_cv->notify_all();
		// wait for _process to complete
		_thread->join();
		delete _thread;
	}

	void PingFIR::_process(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<double>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv, const volatile bool* run){

		#ifdef DEBUG
		std::ofstream _ostr1{"fir_in.log"};
		std::ofstream _ostr2{"fir_out.log"};
		#endif

		// Local vars
		boost::circular_buffer<std::complex<double>> data{_num_taps};
		for(std::size_t i = 0; i < _num_taps; i++){
			data.push_back(std::complex<double>(0, 0));
		}
		const std::size_t SAMPLE_STEP = 1;

		std::size_t count = 0;
		std::size_t out_count = 0;

		while(*run || !input_queue.empty()){
			std::unique_lock<std::mutex> in_lock(input_mutex);
			if(input_queue.empty()){
				input_cv.wait(in_lock);
			}
			if(input_queue.size() >= SAMPLE_STEP){
				for(std::size_t i = 0; i < SAMPLE_STEP; i++){
					data.push_back(input_queue.front());
					#ifdef DEBUG
					_ostr1 << input_queue.front().real();
					if(input_queue.front().imag() >= 0){
						_ostr1 << "+";
					}
					_ostr1 << input_queue.front().imag() << "i" << std::endl;
					#endif
					input_queue.pop();
				}
				count += SAMPLE_STEP;
				in_lock.unlock();
				std::complex<double> filter_output = 0;
				for(std::size_t i = 0; i < _num_taps; i++){
					filter_output += data[i];
					// std::cout << filter_output << std::endl;
				}
				filter_output /= std::complex<double>(_num_taps, 0);
				double amplitude = std::abs(filter_output);
				double dBvalue = amplitudeToDB(amplitude);
				// std::cout << "FIR " << dBvalue << std::endl;


				std::unique_lock<std::mutex> out_lock(output_mutex);
				output_queue.push(dBvalue);
				out_lock.unlock();
				#ifdef DEBUG
				_ostr2 << dBvalue << std::endl;
				#endif
				out_count++;
				output_cv.notify_all();
			}
		}

		#ifdef DEBUG
		_ostr2.close();
		_ostr1.close();
		#endif

		std::cout << "PingFIR consumed " << count << " samples" << std::endl;
		std::cout << "PingFIR output " << out_count << " samples" << std::endl;
	}
}