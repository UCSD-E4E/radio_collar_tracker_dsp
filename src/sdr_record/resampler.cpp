#include "resampler.hpp"
#include <functional>
#include <iostream>
#include <chrono>

#ifdef DEBUG
#include <fstream>
#endif


namespace RTT{
	Resampler::Resampler(std::size_t upsample_factor, 
		std::size_t downsample_factor) : 
		_upsample_factor(upsample_factor),
		_downsample_factor(downsample_factor),
		_thread(nullptr),
		_input_cv(nullptr)
	{
	}

	Resampler::~Resampler(){
	}

	void Resampler::start(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue,
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){
		_input_cv = &input_cv;
		_thread = new std::thread(&Resampler::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv), 
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv),
			run);
	}

	void Resampler::stop(){
		_input_cv->notify_all();
		_thread->join();
		delete _thread;
	}

	/**
	 * Upsamples data into the out_queue from the in_queue using a linear
	 * interpolation.  This method will insert factor samples into out_queue
	 * and consume 1 sample from in_queue.  This method is not thread safe.
	 * @param in_queue  Input queue
	 * @param out_queue Output queue
	 * @param factor    Upsampling factor
	 */
	void upsample(std::queue<std::complex<double>>& in_queue, 
		std::queue<std::complex<double>>& out_queue, std::size_t factor){
		std::complex<double> start_value = in_queue.front();
		in_queue.pop();
		std::complex<double> end_value = in_queue.front();
		std::complex<double> y_inc = (end_value - start_value) / 
		std::complex<double>(factor);
		for(std::size_t i = 0; i < factor; i++){
			out_queue.push(start_value);
			start_value += y_inc;
		}
	}

	/**
	 * Downsamples data from in_queue into out_queue using averaging 
	 * downsampling.  This method will consume factor samples from in_queue and
	 * insert 1 sample into out_queue.  This method is not thread safe.
	 * @param in_queue  Input queue
	 * @param out_queue Output queue
	 * @param factor    Downsampling factor
	 */
	void downsample(std::queue<std::complex<double>>& in_queue,
		std::queue<std::complex<double>>& out_queue, std::size_t factor){

		std::complex<double> acc(0, 0);
		for(std::size_t i = 0; i < factor; i++){
			acc += in_queue.front();
			in_queue.pop();
		}
		acc /= std::complex<double>(factor);
		out_queue.push(acc);
	}

	void Resampler::_process(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue,
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){

		#ifdef DEBUG
		std::ofstream _ostr{"resampler.log"};
		std::size_t _out_idx = 0;
		#endif
		
		// for testing only

		std::queue<std::complex<double>> internal_queue;

		std::size_t count = 0;
		std::size_t out_count = 0;
		while(*run || !input_queue.empty()){
			std::unique_lock<std::mutex> in_lock(input_mutex);
			if(input_queue.empty()){
				input_cv.wait_for(in_lock, std::chrono::milliseconds{10});
			}
			if(input_queue.size() >= _downsample_factor){
				for(std::size_t i = 0; i < _downsample_factor; i++){
					upsample(input_queue, internal_queue, _upsample_factor);
					count++;
				}
				in_lock.unlock();


				std::unique_lock<std::mutex> out_lock(output_mutex);
				downsample(internal_queue, output_queue, _downsample_factor);
				out_count++;
				#ifdef DEBUG
				_ostr << _out_idx++ << ", " << output_queue.back() << std::endl;
				#endif
				out_lock.unlock();
				output_cv.notify_all();
			}else{
				// std::cout << "Resampler doesn't have enough samples!" << std::endl;
				if(!*run && !input_queue.empty()){
					std::size_t size = input_queue.size();
					for(std::size_t i = 0; i < _downsample_factor - size; i++){
						input_queue.push(std::complex<double>(0.0001,0));
					}
				}
				in_lock.unlock();
			}
		}

		#ifdef DEBUG
		_ostr.close();
		#endif

		std::cout << "Resampler consumed " << count << " samples" << std::endl;
		std::cout << "Resampler output " << out_count << " samples" << std::endl;
	}
}