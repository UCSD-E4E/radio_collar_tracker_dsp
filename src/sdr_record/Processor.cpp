#include "Processor.hpp"
#include "mixer.hpp"
#include "resampler.hpp"
#include "ping_fir.hpp"
#include "ping_classifier.hpp"
#include <iostream>

namespace RTT{
	Processor::Processor(const std::size_t frequency, 
		const std::size_t center_freq, const std::size_t sampling_freq,
		std::size_t start_time, const std::size_t fir_size,
		const std::size_t down_factor, const std::size_t up_factor,
		const std::size_t filter_freq,
		const double initial_threshold) :
		_center_freq(center_freq),
		_sampling_freq(sampling_freq), 
		_frequency(frequency),
		_time_start_ms(start_time),
		_fir_size(fir_size),
		_decimation_factor(down_factor),
		_upsampling_factor(up_factor),
		_filter_freq(filter_freq),
		_initial_threshold(initial_threshold),
		_mixer((std::int64_t)frequency - (std::int64_t)center_freq - filter_freq, sampling_freq),
		_resampler(up_factor, down_factor),
		_fir(filter_freq, sampling_freq / down_factor, fir_size),
		_classifier{_initial_threshold, _time_start_ms, 
			down_factor * 1000.0 / sampling_freq}{
	}
	void Processor::start(std::queue<std::complex<double>>& input_queue,
		std::mutex& input_mutex, std::condition_variable& input_cv,
		std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
		std::condition_variable& output_cv, const volatile bool* run){

		_mixer.start(input_queue, input_mutex, input_cv, queue1, mutex1, var1, 
			&_mixer_run);
		_resampler.start(queue1, mutex1, var1, queue2, mutex2, var2, 
			&_resampler_run);
		_fir.start(queue2, mutex2, var2, queue3, mutex3, var3, &_fir_run);
		_classifier.start(queue3, mutex3, var3, output_queue, output_mutex, 
			output_cv, &_classifier_run);
	}

	void Processor::set_start_time(std::size_t start_time_ms){
		_time_start_ms = start_time_ms;
	}

	void Processor::stop(){
		_mixer_run = false;
		std::cout << "Stopping mixer" << std::endl;
		_mixer.stop();
		_resampler_run = false;
		std::cout << "Stopping resampler" << std::endl;
		_resampler.stop();
		_fir_run = false;
		std::cout << "Stopping fir" << std::endl;
		_fir.stop();
		_classifier_run = false;
		std::cout << "Stopping classfier" << std::endl;
		_classifier.stop();
	}
}