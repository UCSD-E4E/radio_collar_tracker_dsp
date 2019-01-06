#include "remixer.hpp"
#include <functional>
#include <boost/math/common_factor.hpp>
#include <utility.hpp>
#include <syslog.h>

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	Remixer::Remixer(std::int64_t shift, std::size_t sampling_frequency, 
		std::size_t up_factor, std::size_t down_factor) :
		_thread(nullptr),
		_input_cv(nullptr),
		_upsample_factor(up_factor),
		_downsample_factor(down_factor)
		{
		_period = 2 * sampling_frequency / boost::math::gcd(std::abs(shift), 
			(std::int64_t)sampling_frequency);
		_period = boost::math::lcm(_period, down_factor);
		_bbeat = generateSinusoid(shift, sampling_frequency, _period);
		syslog(LOG_DEBUG, "Remixer: period %lu", _period);
	}

	Remixer::~Remixer(){
		delete[] _bbeat;
	}

	void Remixer::start(std::queue<std::complex<double>>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue, 
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){

		_input_cv = &input_cv;
		syslog(LOG_INFO, "Remixer stored input condition_variable as 0x%p", 
			(void*)_input_cv);

		_thread = new std::thread(&Remixer::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv),
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv),
			run);
	}

	void Remixer::stop(){
		_input_cv->notify_all();
		_thread->join();
		delete _thread;
	}

	void Remixer::_process(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<std::complex<double>>& output_queue, 
		std::mutex& output_mutex, std::condition_variable& output_cv,
		const volatile bool* run){

		std::complex<double>* data_array = new std::complex<double>[_period];
		std::complex<double>* output_array = new std::complex<double>[_period / 
			_downsample_factor];
		std::complex<double> retval;
		std::unique_lock<std::mutex> in_lock(input_mutex, std::defer_lock);
		std::unique_lock<std::mutex> out_lock(output_mutex, std::defer_lock);

		std::size_t in_count = 0;

		#ifdef DEBUG
		std::ofstream _ostr1{"remixer_in.log"};
		std::ofstream _ostr2{"remixer_out.log"};
		std::size_t _out_count = 0;
		#endif

		syslog(LOG_INFO, "Remixer: running process");
		
		while(*run || !input_queue.empty()){
			in_lock.lock();
			if(input_queue.empty()){
				syslog(LOG_DEBUG, "Remixer: input empty, sleeping");
				input_cv.wait(in_lock);
				in_lock.unlock();
			}else{
				syslog(LOG_DEBUG, "Remixer: input has data");
				aggregate(input_queue, in_lock, input_cv, data_array, 
					_period, run);
				in_lock.unlock();
				in_count += _period;
				syslog(LOG_DEBUG, "Remixer: got data, remaining %7lu", input_queue.size());

				#ifdef DEBUG
				for(std::size_t i = 0; i < _period; i++){
					_ostr1 << data_array[i].real();
					if(data_array[i].imag() >= 0){
						_ostr1 << "+";
					}
					_ostr1 << data_array[i].imag() << "i" << std::endl;
				}
				#endif

				func(data_array, output_array);
				syslog(LOG_DEBUG, "Remixer: processed block");
				
				out_lock.lock();
				for(std::size_t i = 0; i < _period / _downsample_factor; i++){
					output_queue.push(output_array[i]);
					#ifdef DEBUG
					_ostr2 << output_array[i].real();
					if(output_array[i].imag() >= 0){
						_ostr2 << "+";
					}
					_ostr2 << output_array[i].imag() << "i" << std::endl;
					#endif
				}
				out_lock.unlock();
				output_cv.notify_all();
				syslog(LOG_DEBUG, "Remixer: pushed output block");
			}
		}
		syslog(LOG_DEBUG, "Remixer: Exited loop");
		delete[] output_array;
		delete[] data_array;

		#ifdef DEBUG
		_ostr2.close();
		_ostr1.close();
		#endif
	}

	void Remixer::func(std::complex<double>* data, std::complex<double>* out){
		for(std::size_t i = 0; i < _period / _downsample_factor; i++){
			std::size_t base = i * _downsample_factor;
			for(std::size_t j = 0; j < _downsample_factor; j++){
				out[i] += data[base + j] * _bbeat[base + j];
			}
			out[i] /= std::complex<double>(_downsample_factor, 0);
		}
	}

	void Remixer::aggregate(std::queue<std::complex<double>>& queue,
		std::unique_lock<std::mutex>& lock, std::condition_variable& q_cv, 
		std::complex<double>* data, std::size_t N, const volatile bool* run){
		while(queue.size() < N && *run){
			syslog(LOG_NOTICE, "Aggregate waiting size = %lu", queue.size());
			q_cv.wait_for(lock, std::chrono::milliseconds{10000});
			// q_cv.wait(lock);
		}
		for(std::size_t i = 0; i < N && !queue.empty(); i++){
			data[i] = queue.front();
			queue.pop();
		}
	}
}
