#include "integrator.hpp"
#include "utility.hpp"

// #define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{
	Integrator::Integrator(const std::size_t factor) : _input_cv(nullptr), 
		_thread(nullptr), _decimation(factor){
	}
	Integrator::~Integrator(){
	}
	void Integrator::start(std::queue<double>& i_q, std::mutex& i_m,
		std::condition_variable& i_c, std::queue<double>& o_q,
		std::mutex& o_m, std::condition_variable& o_c){
		_input_cv = &i_c;
		_run = true;
		_thread = new std::thread(&Integrator::_process, this, std::ref(i_q),
			std::ref(i_m), std::ref(i_c), std::ref(o_q), std::ref(o_m), 
			std::ref(o_c));
	}
	void Integrator::stop(){
		_input_cv->notify_all();
		_run = false;
		_thread->join();
		delete _thread;
	}
	void Integrator::_process(std::queue<double>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<double>& output_queue, std::mutex& output_mutex, 
		std::condition_variable& output_cv){
		#ifdef DEBUG
		std::ofstream _ostr1{"int_in.log"};
		std::ofstream _ostr2{"int_out.log"};
		#endif

		// Local vars
		std::size_t count = 0;
		std::size_t out_count = 0;
		std::size_t data_idx = 0;
		double acc = 0;

		// Main loop
		while(_run || !input_queue.empty()){
			// get lock
			std::unique_lock<std::mutex> in_lock(input_mutex);

			// get data
			if(input_queue.empty()){
				#ifdef DEBUG
				std::cout << "Integrator is waiting" << std::endl;
				#endif
				input_cv.wait(in_lock);
				#ifdef DEBUG
				std::cout << "Integrator woke up" << std::endl;
				#endif
			}
			if(input_queue.size() >= 1){
				#ifdef DEBUG
				std::cout << "Integrator has data!" << std::endl;
				#endif
				for(;data_idx < _decimation && !input_queue.empty(); data_idx++){
					#ifdef DEBUG
					_ostr1 << input_queue.front() << std::endl;
					#endif
					
					acc += input_queue.front();
					input_queue.pop();
					count++;

				}
			}
			if(data_idx == _decimation){
				// accumulated full buffer!
				std::unique_lock<std::mutex> out_lock(output_mutex);
				output_queue.push(amplitudeToDB(acc));
				out_lock.unlock();
				output_cv.notify_all();

				#ifdef DEBUG
				_ostr2 << acc << std::endl;
				#endif

				acc = 0;
				data_idx = 0;
				out_count++;
			}
		}
		#ifdef DEBUG
		std::cout << "Integrator out!" << std::endl;
		_ostr2.close();
		_ostr1.close();
		#endif
	}
}