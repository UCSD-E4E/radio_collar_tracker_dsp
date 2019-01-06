#include "ping_classifier.hpp"
#include <functional>
#include <limits>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <syslog.h>

using namespace boost::accumulators;

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	PingClassifier::PingClassifier(const double threshold, 
		const std::size_t& time_start_ms, const double ms_per_sample) : 
		_threshold(threshold),
		_time_start_ms(time_start_ms),
		_ms_per_sample(ms_per_sample){
		std::cout << ms_per_sample << " ms per sample for Classifier" << std::endl;
		std::cout << "Classifier threshold: " << _threshold << std::endl;
	}

	PingClassifier::~PingClassifier(){
	}

	void PingClassifier::setThreshold(const double threshold){
		std::unique_lock<std::mutex> lock(_threshold_mutex);
		_threshold = threshold;
	}

	void PingClassifier::start(std::queue<double>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv, const volatile bool* run){
		_input_cv = &input_cv;
		_thread = new std::thread(&PingClassifier::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv), 
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv),
			run);
	}

	void PingClassifier::stop(){
		_input_cv->notify_all();
		_thread->join();
		delete _thread;
	}

	void PingClassifier::_process(std::queue<double>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv, const volatile bool* run){

		double maxVal = std::numeric_limits<double>::lowest();
		std::size_t maxIndex = 0;

		accumulator_set<double, features<tag::mean, tag::variance>> acc;

		std::size_t count = 0;
		std::size_t out_count = 0;

		#ifdef DEBUG
		std::ofstream _ostr1{"classifier_in.log"};
		std::ofstream _ostr2{"classifier_out.log"};
		#endif

		while(*run || !input_queue.empty()){
			std::unique_lock<std::mutex> in_lock(input_mutex);
			if(input_queue.empty()){
				input_cv.wait(in_lock);
			}
			if(!input_queue.empty()){
				double value = input_queue.front();
				// std::cout << _sample_count << ": " << value << std::endl;
				input_queue.pop();
				count++;
				in_lock.unlock();

				#ifdef DEBUG
				_ostr1 << value << std::endl;
				#endif

				if(value > maxVal){
					maxVal = value;
					maxIndex = _sample_count;
				}
				acc(value);
				// std::cout << value << std::endl;
				if(_sample_count % threshold_update_period == threshold_update_period - 1){
					_threshold = mean(acc) + 3 * sqrt(variance(acc));
					std::cout << "Classifier threshold: " << _threshold << std::endl;
				}
				if(value < _threshold){
					if(maxVal > _threshold){
						// maxVal is the ping
						// std::cout << "Ping at idx " << maxIndex << std::endl;
						syslog(LOG_NOTICE, "Detected ping at %.0f with amplitude %0.2f", maxIndex * 
							_ms_per_sample + _time_start_ms, maxVal);
						PingPtr ping = std::make_shared<Ping>(maxIndex * 
							_ms_per_sample + _time_start_ms, maxVal);
						std::unique_lock<std::mutex> out_lock(output_mutex);
						output_queue.push(ping);
						out_count++;
						out_lock.unlock();
						output_cv.notify_all();

						#ifdef DEBUG
						_ostr2 << out_count << ", " << maxIndex << ", " << maxVal << std::endl;
						#endif
					}
					maxVal = std::numeric_limits<double>::lowest();
				}
				_sample_count++;
			}
		}
		std::cout << "Classifier consumed " << count << " samples" << std::endl;
		std::cout << "Classifier output " << out_count << " pings" << std::endl;
		std::cout << "Mean at " << mean(acc) << std::endl;
		std::cout << "Variance at " << variance(acc) << std::endl;
		#ifdef DEBUG
		_ostr1.close();
		_ostr2.close();
		#endif
	}
}