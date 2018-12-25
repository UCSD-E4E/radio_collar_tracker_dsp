#include "ping_fir.hpp"
#include "utility.hpp"
#include <list>
#include <complex>
#include <functional>
#include <iostream>
#include <limits>

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
		delete _filter_taps;
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

		// Local vars
		// std::complex<double> data[_num_taps];
		std::list<std::complex<double>> data{};
		const std::size_t SAMPLE_STEP = 1;

		std::size_t count = 0;
		std::size_t out_count = 0;

		while(*run || !input_queue.empty()){
			std::unique_lock<std::mutex> in_lock(input_mutex);
			if(input_queue.empty()){
				input_cv.wait(in_lock);
			}
			if(input_queue.size() >= SAMPLE_STEP){
				load_data(input_queue, data, SAMPLE_STEP, _num_taps);
				count += SAMPLE_STEP;
				in_lock.unlock();
				if(data.size() < _num_taps){
					continue;
				}
				// std::complex<double> filter_output = convolve(data, 
					// _filter_taps, _num_taps);
				std::complex<double> filter_output = 0;
				for(auto it = data.begin(); it != data.end(); it++){
					filter_output += *it;
					// std::cout << filter_output << std::endl;
				}
				double amplitude = std::abs(filter_output);
				double dBvalue = powerToDB(amplitude);
				// std::cout << "FIR " << dBvalue << std::endl;
				std::unique_lock<std::mutex> out_lock(output_mutex);
				output_queue.push(dBvalue);
				out_lock.unlock();
				out_count++;
				output_cv.notify_all();
			}
		}
		std::cout << "PingFIR consumed " << count << " samples" << std::endl;
		std::cout << "PingFIR output " << out_count << " samples" << std::endl;
	}
}