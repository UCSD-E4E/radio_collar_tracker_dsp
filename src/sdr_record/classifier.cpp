#include "classifier.hpp"
#include <boost/circular_buffer.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <limits>
#include "tagged_signal.hpp"
#include <fftw3.h>
#include <iostream>
#include <float.h>

#define DEBUG

#ifdef DEBUG
#include <fstream>
#endif

namespace RTT{
	Classifier::Classifier(const std::size_t time_start_ms, 
		const double input_freq, const double signal_freq, 
		const double initial_threshold) : 
		_time_start_ms(time_start_ms), _input_freq(input_freq), 
		_signal_freq(signal_freq),
		_ms_per_sample(1/input_freq * 1000.0),
		threshold(100),
		_average_len(0.25*input_freq),
		ping_width_samp(ping_width_ms * input_freq / 1000.){
		#ifdef DEBUG
		std::ofstream cfg_str{"classifier_config.log"};
		cfg_str << "_time_start_ms, " << _time_start_ms << std::endl;
		cfg_str << "_input_freq, " << _input_freq << std::endl;
		cfg_str << "_signal_freq, " << _signal_freq << std::endl;
		cfg_str << "_ms_per_sample, " << _ms_per_sample << std::endl;
		cfg_str << "threshold, " << threshold << std::endl;
		cfg_str << "_average_len, " << _average_len << std::endl;
		cfg_str << "ping_width_samp, " << ping_width_samp << std::endl;
		cfg_str.close();

		#endif
	}

	Classifier::~Classifier(){
	}

	void Classifier::start(std::queue<TaggedSignal*>& input_queue, 
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
		boost::circular_buffer<bool>::iterator end, 
		boost::circular_buffer<bool>::iterator begin){
		std::size_t width = 0;
		while(!is_rising_edge(end)){
			end--;
			width++;
			if(end == begin){
				return -1;
			}
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

	const double max(boost::circular_buffer<double>& sig){
		double maxVal = -DBL_MAX;
		for(auto it = sig.begin(); it != sig.end(); it++){
			maxVal = std::max(*it, maxVal);
		}
		return maxVal;
	}

	const double sig_median(boost::circular_buffer<double>& sig){
		boost::accumulators::accumulator_set<double, 
			boost::accumulators::features<boost::accumulators::tag::median> > acc;
		for(auto it = sig.begin(); it != sig.end(); it++){
			acc(*it);
		}
		return boost::accumulators::median(acc);
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

	void Classifier::_process(std::queue<TaggedSignal*>& input_queue, 
		std::mutex& input_mutex, std::condition_variable& input_cv, 
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv){

		#ifdef DEBUG
		std::ofstream _ostr1{"classifier_in.log"};
		std::ofstream _ostr2{"classifier_out.log"};
		std::ofstream _ostr3{"classifier_threshold.log"};
		std::ofstream _ostr4{"classifier_sig_fft.log"};
		std::ofstream _ostr5{"classifier_sig_in.log"};
		std::ofstream _ostr6{"classifier_test.log"};
		#endif

		// Local vars
		boost::circular_buffer<double> data{_average_len};
		boost::circular_buffer<double> pk_hist{ping_width_samp};
		boost::circular_buffer<double> peaks{_average_len};
		// FIXME set initial f_s as a variable!

		boost::circular_buffer<std::complex<double>> cplx_hist{(std::size_t)(ping_width_ms * _signal_freq / 1000)};
		for(std::size_t i = 0; i < ping_width_ms * _signal_freq / 1000; i++){
			cplx_hist.push_back(std::complex<double>(0));
		}
		
		std::size_t id_len = (std::size_t)(_input_freq * 0.5);
		boost::circular_buffer<bool> id_signal{id_len};
		for(std::size_t i = 0; i < id_len; i++){
			id_signal.push_back(false);
		}

		std::size_t signal_idx = 0;
		std::size_t out_count = 0;

		threshold = 100;
		TaggedSignal* tsig;

		fftw_complex* fft_in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFT_LEN);
		if(fft_in == nullptr){
			std::cout << "Failed to allocate fft_in!" << std::endl;
			return;
		}


		fftw_complex* fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * FFT_LEN);
		if(fft_out == nullptr){
			std::cout << "Failed to allocation fft_out!" << std::endl;
			return;
		}

		if(!fftw_init_threads()){
			std::cout << "Failed to initialize FFTW threads!" << std::endl;
			return;
		}
		fftw_plan_with_nthreads(4);
		fftw_plan fft_plan = fftw_plan_dft_1d(FFT_LEN, fft_in, fft_out,
			FFTW_FORWARD, FFTW_MEASURE);

		std::size_t fft_offset = 0; // FIXME TUNE ME!
		
		while(_run || !input_queue.empty()){
			// get lock
			std::unique_lock<std::mutex> in_lock(input_mutex);

			// get data
			if(input_queue.empty()){
				input_cv.wait(in_lock);
			}
			if(!input_queue.empty()){
				// Update threshold
				tsig = input_queue.front();
				if(threshold != 100){
					data.push_back(tsig->val);
					pk_hist.push_back(tsig->val);
					peaks.push_back(max(pk_hist));
					#ifdef DEBUG
					for(auto it = pk_hist.begin(); it != pk_hist.end(); it++){
						_ostr6 << *it << ", ";
					}
					_ostr6 << std::endl;
					#endif
					threshold = sig_median(peaks);
				}else{
					for(std::size_t i = 0; i < _average_len; i++){
						peaks.push_back(tsig->val);
						data.push_back(tsig->val);
					}
					for(std::size_t i = 0; i < ping_width_samp; i++){
						pk_hist.push_back(tsig->val);
					}
					threshold = tsig->val;
				}

				{
					std::vector<std::complex<double>>& sig = *tsig->sig;
					for(std::size_t i = 0; i < sig.size(); i++){
						cplx_hist.push_back(sig[i]);
						#ifdef DEBUG
						_ostr5 << sig[i].real();
						if(sig[i].imag() >= 0){
							_ostr5 << "+";
						}
						_ostr5 << sig[i].imag() << "i" << std::endl;
						#endif
					}
				}



				#ifdef DEBUG
				_ostr1 << tsig->val << std::endl;
				_ostr3 << threshold << std::endl;
				#endif

				input_queue.pop();
				delete tsig->sig;
				delete tsig;
				signal_idx++;
				
				// Generate classifier signal
				id_signal.push_back(data.back() > (threshold + MIN_SNR));

				// debounce????
				// debounce(id_signal.end() - 1);

				// Wait for entire ping to be sampled
				if(is_falling_edge(id_signal.end() - 1)){
					const std::size_t pulse_width = 
						get_pulse_width(id_signal.end() - 1, id_signal.begin());
					if(pulse_width < 0.5 * ping_width_samp){
						std::cout << "Ping rejected as too short!" << std::endl;
						#ifdef DEBUG
						#endif
						continue;
					}
					if(pulse_width > 2 * ping_width_samp){
						std::cout << "Ping rejected as too long!" << std::endl;
						#ifdef DEBUG
						#endif
						continue;
					}

					auto ping_start = data.end() - 1 - pulse_width;
					auto ping_end = data.end() - 1;
					const double amplitude = get_pulse_magnitude(ping_start, 
						ping_end);
					std::size_t ping_start_ms = (std::size_t)((signal_idx - 
						pulse_width) * _ms_per_sample + _time_start_ms);
					for(std::size_t i = 0; i < FFT_LEN; i++){
						fft_in[i][0] = cplx_hist[i + fft_offset].real();
						fft_in[i][1] = cplx_hist[i + fft_offset].imag();

						#ifdef DEBUG
						// _ostr4 << cplx_hist[i + fft_offset].real();
						// if(cplx_hist[i + fft_offset].imag() >= 0){
						// 	_ostr4 << "+";
						// }
						// _ostr4 << cplx_hist[i + fft_offset].imag() << "i, ";
						#endif

					}
					#ifdef DEBUG
					// _ostr4 << std::endl;
					#endif
					fftw_execute(fft_plan);
					std::size_t max_idx = 0;
					double max_amp = -100;
					for(std::size_t i = 0; i < FFT_LEN; i++){
						if(std::abs(std::complex<double>(fft_out[i][0], fft_out[i][1])) > max_amp){
							max_amp = std::abs(std::complex<double>(fft_out[i][0], fft_out[i][1]));
							max_idx = i;
						}
					}
					int sig_freq = 0;
					if(max_idx > FFT_LEN / 2){
						sig_freq = (max_idx - FFT_LEN) / (double)FFT_LEN * _signal_freq;
					}else{
						sig_freq = max_idx / (double)FFT_LEN * _signal_freq;
					}


					
					PingPtr ping = std::make_shared<Ping>(ping_start_ms, 
						amplitude, sig_freq);
					std::unique_lock<std::mutex> out_lock(output_mutex);
					output_queue.push(ping);
					out_count++;
					out_lock.unlock();
					output_cv.notify_all();

					std::cout << "Ping " << out_count << " at " << (ping_start_ms - _time_start_ms) / 1e3 << "s " << 
						", amplitude " << amplitude << ", threshold: " <<  threshold << 
						", width: " << pulse_width * _ms_per_sample << 
						", freq: " << sig_freq <<
						std::endl;
					#ifdef DEBUG
					_ostr2 << out_count << ", " << (ping_start_ms - _time_start_ms) / 1e3 << 
						", " << amplitude << ", average: " <<  threshold << 
						", width: " << pulse_width * _ms_per_sample << 
						", freq: " << sig_freq <<
						std::endl;
					#endif
				}
			}
		}

		fftw_free(fft_out);
		fftw_free(fft_in);
		fftw_destroy_plan(fft_plan);
		fftw_forget_wisdom();
		fftw_cleanup();
		fftw_cleanup_threads();

		std::cout << "Classifier consumed " << signal_idx << " samples" << 
			std::endl;
		std::cout << "Classifier output " << out_count << " pings" << std::endl;
		#ifdef DEBUG
		std::cout << "Threshold at " << threshold + MIN_SNR << std::endl;
		_ostr1.close();
		_ostr2.close();
		_ostr3.close();
		_ostr4.close();
		_ostr5.close();
		_ostr6.close();
		#endif
	}

	void Classifier::setStartTime(std::size_t time_start_ms){
		_time_start_ms = time_start_ms;
	}
}