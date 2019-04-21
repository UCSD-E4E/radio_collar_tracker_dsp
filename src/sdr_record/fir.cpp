#include "fir.hpp"
#include <boost/circular_buffer.hpp>
#include <vector>

// #define DEBUG
#include <iostream>
#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	FIR::FIR() : _input_cv(nullptr), _thread(nullptr){
	}

	FIR::~FIR(){
	}

	void FIR::start(std::queue<std::complex<double>>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<TaggedSignal*>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv){
		// Store blocking condition variable to wake on later
		_input_cv = &input_cv;
		// Start thread
		_run = true;
		_thread = new std::thread(&FIR::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv), 
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv));
	}

	void FIR::stop(){
		_run = false;
		_input_cv->notify_all();
		_thread->join();
		delete _thread;
	}

	void FIR::_process(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<TaggedSignal*>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv){

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
		std::complex<double>mac_output = 0;
		double filter_output = 0;

		// Main Loop
		while(_run || !input_queue.empty()){
			// get lock
			std::unique_lock<std::mutex> in_lock(input_mutex);

			// get data
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

				// process filter
				auto sig = new std::vector<std::complex<double>>(_num_taps);
				// std::cout << "Creating new vector " << sig << std::endl;
				mac_output = 0;
				for(std::size_t i = 0; i < _num_taps; i++){
					mac_output += data[i] * _TAPS[i];
					(*sig)[i] = data[i];
				}
				// mac_output /= std::complex<double>(_num_taps, 0);
				filter_output = std::abs(mac_output) * std::abs(mac_output);



				// output
				TaggedSignal* output = new TaggedSignal(filter_output, *sig);
				// std::cout << "Creating new TS " << output << std::endl;
				std::unique_lock<std::mutex> out_lock(output_mutex);
				output_queue.push(output);
				out_lock.unlock();
				out_count++;
				output_cv.notify_all();

				#ifdef DEBUG
				_ostr2 << filter_output << std::endl;
				#endif
			}
		}

		#ifdef DEBUG
		_ostr2.close();
		_ostr1.close();
		#endif
	}
}