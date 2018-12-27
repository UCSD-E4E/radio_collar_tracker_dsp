#include "mixer.hpp"
#include "utility.hpp"
#include <functional>
#include <boost/math/common_factor.hpp>
#include <iostream>
#include <chrono>
#include <syslog.h>

#include <iostream>
#include <complex>

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	Mixer::Mixer(std::int64_t shift, std::size_t sampling_frequency) : 
		_period(2 * sampling_frequency / boost::math::gcd(std::abs(shift), 
			(std::int64_t)sampling_frequency)),
		_bbeat(generateSinusoid(-1 * shift, sampling_frequency, _period, 1.0)),
		_thread(nullptr),
		_input_cv(nullptr)
	{
		std::cout << "Creating Mixer at " << shift << " Hz" << std::endl;
	}

	Mixer::~Mixer(){
		delete _bbeat;
		if(_thread){
		}
	}

	void Mixer::start(std::queue<std::complex<double>>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue, 
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){
		// Store blocking condition variable to wake on later
		_input_cv = &input_cv;
		syslog(LOG_INFO, "Mixer storing input condition variable as 0x%08p", _input_cv);
		// Start thread
		_thread = new std::thread(&Mixer::_process, this, std::ref(input_queue),
			std::ref(input_mutex), std::ref(input_cv), std::ref(output_queue),
			std::ref(output_mutex), std::ref(output_cv), run);
	}

	void Mixer::stop(){
		// wake up _process
		_input_cv->notify_all();
		// wait for _process to complete
		_thread->join();
		delete _thread;
	}

	void Mixer::_process(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue, 
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){

		#ifdef DEBUG
		std::ofstream _output{"mixer.log"};
		std::size_t _out_idx = 0;
		#endif

		// Local vars
		std::size_t period_idx = 0;
		std::size_t count = 0;
		while(*run || !input_queue.empty()){
			std::unique_lock<std::mutex> lock(input_mutex);
			if(input_queue.empty()){
				input_cv.wait_for(lock, std::chrono::milliseconds{10});
			}

			if(input_queue.size() > 1000000){
				if(count % 1000 == 0)
					syslog(LOG_WARNING, "Mixer input queue is filling up!");
			}

			if(!input_queue.empty()){
				std::complex<double> sample = input_queue.front();
				input_queue.pop();
				lock.unlock();

				#ifdef DEBUG
				_output << _out_idx++ << ", " << sample.real() << ", " << sample.imag();
				#endif

				sample *= _bbeat[period_idx];

				period_idx++;
				period_idx %= _period;

				std::unique_lock<std::mutex> lock(output_mutex);
				output_queue.push(sample);
				lock.unlock();

				output_cv.notify_all();
				count++;


				#ifdef DEBUG
				_output << ", " << sample.real() << ", " << sample.imag() << std::endl;
				#endif
				
			}
		}
		std::cout << "Mixer processed " << count << " samples" << std::endl;
		#ifdef DEBUG
		_output.close();
		#endif
	}
}