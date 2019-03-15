#include "classifier.hpp"
#include <boost/circular_buffer.hpp>
#include <limits>

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{
	Classifier::Classifier(const std::size_t time_start_ms, 
		const double sampling_freq, const double initial_threshold) : 
		_time_start_ms(time_start_ms), _sampling_freq(sampling_freq), 
		_ms_per_sample(1/sampling_freq * 1000.0),
		threshold(initial_threshold),
		ping_width_samp(ping_width_ms / _ms_per_sample){
	}

	Classifier::~Classifier(){
	}

	void Classifier::start(std::queue<double>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv){
		_input_cv = &input_cv;
		_run = true;
		_thread = new std::thread(&Classifier::_process, this, 
			std::ref(input_queue), std::ref(input_mutex), std::ref(input_cv), 
			std::ref(output_queue), std::ref(output_mutex), std::ref(output_cv));
	}

	void Classifier::stop(){
		_input_cv->notify_all();
		_run = false;
		_thread->join();
		delete _thread;
	}

	bool is_rising_edge(boost::circular_buffer<bool>::iterator it){
		if(*(it - 1) == false and *(it) == true){
			return true;
		}else{
			return false;
		}
	}

	bool is_falling_edge(boost::circular_buffer<bool>::iterator it){
		if(*(it - 1) == true and *(it) == false){
			return true;
		}else{
			return false;
		}
	}

	/**
	 * Gets number of samples to previous rising edge
	 * @param  end Iterator pointing to the end (falling edge) of the pulse
	 * @return     Samples to the most recent rising edge
	 */
	const std::size_t get_pulse_width(
		boost::circular_buffer<bool>::iterator end){
		std::size_t width = 0;
		while(!is_rising_edge(end)){
			end--;
			width++;
		}
		return width;
	}

	/**
	 * Calculates the amplitude of the pulse, specified by the start and end
	 * of the pulse.
	 * @param  start  Iterator pointing to the rising edge of the pulse
	 * @param  end    Iterator pointing to the falling edge of the pulse
	 * @return        Maximum amplitude of the pulse
	 */
	const double get_pulse_magnitude(
		boost::circular_buffer<double>::iterator start,
		boost::circular_buffer<double>::iterator end){
		double max_amplitude = std::numeric_limits<double>::lowest();
		for(;start != end; start++){
			if(*start > max_amplitude){
				max_amplitude = *start;
			}
		}
		return max_amplitude;
	}

	/**
	 * Calculates the average of the entire signal
	 * @param  sig Signal as a circular buffer
	 * @return     Average of the circular buffer.
	 */
	const double average(boost::circular_buffer<double>& sig){
		double acc = 0;
		for(auto it = sig.begin(); it != sig.end(); it++){
			acc += *it;
		}
		return acc / sig.size();
	}

	/**
	 * Debounces the pulse.
	 * @param it Iterator pointing to the end of the pulse to debounce
	 */
	void Classifier::debounce(boost::circular_buffer<bool>::iterator it){
		if(is_rising_edge(it)){
			// check if there was a rising edge within 1 pulse prior
			bool prior_pulse = false;
			int prior_pulse_idx = 0;
			for(std::size_t i = 0; i < ping_width_samp; i++){
				if(is_rising_edge(it - i)){
					prior_pulse = true;
					prior_pulse_idx = i;
				}
			}
			if(prior_pulse){
				for(std::size_t i = prior_pulse_idx + 1; i > 0; i--){
					*(it - i) = true;
				}
			}

		}
	}

	void Classifier::_process(std::queue<double>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv){

		#ifdef DEBUG
		std::ofstream _ostr1{"classifier_in.log"};
		std::ofstream _ostr2{"classifier_out.log"};
		std::ofstream _ostr3{"classifier_threshold.log"};
		#endif

		// Local vars
		boost::circular_buffer<double> data{_AVERAGE_LEN};
		for(std::size_t i = 0; i < _AVERAGE_LEN; i++){
			data.push_back(threshold);
		}
		std::size_t id_len = (std::size_t)(_sampling_freq * 0.5);
		boost::circular_buffer<bool> id_signal{id_len};
		for(std::size_t i = 0; i < id_len; i++){
			id_signal.push_back(false);
		}

		std::size_t signal_idx = 0;
		std::size_t out_count = 0;

		while(_run || !input_queue.empty()){
			// get lock
			std::unique_lock<std::mutex> in_lock(input_mutex);

			// get data
			if(input_queue.empty()){
				input_cv.wait(in_lock);
			}
			if(!input_queue.empty()){
				// Update threshold
				data.push_back(input_queue.front());
				threshold = average(data);

				#ifdef DEBUG
				_ostr1 << input_queue.front() << std::endl;
				_ostr3 << threshold << std::endl;
				#endif

				input_queue.pop();
				signal_idx++;
				
				// Generate classifier signal
				id_signal.push_back(data.back() > (threshold + MIN_SNR));

				// debounce????
				// debounce(id_signal.end() - 1);

				// Wait for entire ping to be sampled
				if(is_falling_edge(id_signal.end() - 1)){
					const std::size_t pulse_width = 
						get_pulse_width(id_signal.end() - 1);
					if(pulse_width < 0.75 * ping_width_samp){
						continue;
					}
					auto ping_start = data.end() - 1 - pulse_width;
					auto ping_end = data.end() - 1;
					const double amplitude = get_pulse_magnitude(ping_start, 
						ping_end);
					std::size_t ping_start_ms = (std::size_t)((signal_idx - 
						pulse_width) * _ms_per_sample + _time_start_ms);
					PingPtr ping = std::make_shared<Ping>(ping_start_ms, 
						amplitude);
					std::unique_lock<std::mutex> out_lock(output_mutex);
					output_queue.push(ping);
					out_count++;
					out_lock.unlock();
					output_cv.notify_all();

					#ifdef DEBUG
					std::cout << out_count << ", " << (ping_start_ms - _time_start_ms) / 1e3 << 
						", " << amplitude << ", average: " <<  threshold << 
						", width: " << pulse_width * _ms_per_sample << 
						std::endl;
					_ostr2 << out_count << ", " << (ping_start_ms - _time_start_ms) / 1e3 << 
						", " << amplitude << ", average: " <<  threshold << 
						", width: " << pulse_width * _ms_per_sample << 
						std::endl;
					#endif
				}
			}
		}

		#ifdef DEBUG
		std::cout << "Classifier consumed " << signal_idx << " samples" << 
			std::endl;
		std::cout << "Classifier output " << out_count << " pings" << std::endl;
		std::cout << "Threshold at " << threshold + MIN_SNR << std::endl;
		_ostr1.close();
		_ostr2.close();
		_ostr3.close();
		#endif
	}

	void Classifier::setStartTime(std::size_t time_start_ms){
		_time_start_ms = time_start_ms;
	}
}