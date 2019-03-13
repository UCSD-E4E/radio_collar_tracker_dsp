#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

// #include "mixer.hpp"
// #include "resampler.hpp"
#include "ping_fir.hpp"
#include "ping_classifier.hpp"
#include "remixer.hpp"

namespace RTT{
	class Processor{
	private:
		const std::size_t _center_freq;

		const std::size_t _sampling_freq;
		
		const std::size_t _frequency;

		std::size_t _time_start_ms;

		const std::size_t _fir_size;

		/**
		 * Decimation factor
		 */
		const std::size_t _decimation_factor;

		const std::size_t _upsampling_factor;

		const std::int64_t _filter_freq;

		const double _initial_threshold;

		// Mixer _mixer;

		// volatile bool _mixer_run = true;

		// Resampler _resampler;

		// volatile bool _resampler_run = true;

		Remixer _mixer;

		volatile bool _mixer_run = true;

		PingFIR _fir;

		volatile bool _fir_run = true;

		PingClassifier _classifier;

		volatile bool _classifier_run = true;

		// std::queue<std::complex<double>> queue1{};
		// std::mutex mutex1{};
		// std::condition_variable var1{};

		std::queue<std::complex<double>> queue2{};
		std::mutex mutex2{};
		std::condition_variable var2{};

		std::queue<double> queue3{};
		std::mutex mutex3{};
		std::condition_variable var3{};


	public:
		Processor(const std::size_t frequency, const std::size_t center_freq, 
			const std::size_t sampling_freq, std::size_t start_time = 0,
			const std::size_t fir_size = 100,
			const std::size_t down_factor = 20, 
			const std::size_t up_factor = 1, 
			const std::size_t filter_freq = 0,
			const double initial_threshold = -20);

		/**
		 * Takes in a sequence of complex<double> IQ samples, and identifies
		 * individual pings within the data and outputs a sequence of PingPtr
		 * objects.
		 * @param input_queue  	Input queue
		 * @param input_mutex  	Input mutex
		 * @param input_cv     	Input condition variable
		 * @param output_queue 	Output queue
		 * @param output_mutex 	Output mutex
		 * @param output_cv    	Output condition variable
		 * @param run          	Run flag
		 * @param time_start_ms	Variable containing time of first sample in ms
		 */
		void start(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv, const volatile bool* run);
		/**
		 * Stops the processor.  This function shall not return until all 
		 * threads are complete.
		 */
		void stop();

		void set_start_time(std::size_t start_time_ms);

	};
}

#endif