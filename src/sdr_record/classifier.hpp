#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__

#include "ping.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <boost/circular_buffer.hpp>

namespace RTT{
	class Classifier{
	public:
		Classifier(const std::size_t time_start_ms, const double sampling_freq,
			const double initial_threshold = -12.0);
		~Classifier();
		void start(std::queue<double>& input_queue, std::mutex& input_mutex,
			std::condition_variable& input_cv, 
			std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv);
		void stop();
		void setStartTime(std::size_t time_start_ms);
	private:
		std::size_t _time_start_ms;
		double _sampling_freq;
		double _ms_per_sample;
		double threshold;
		std::thread* _thread = nullptr;
		std::condition_variable* _input_cv = nullptr;
		const std::size_t _AVERAGE_LEN = 4000;
		const double MIN_SNR = 4;

		const std::size_t ping_width_ms = 25;
		std::size_t ping_width_samp;

		/**
		 * Takes in a queue of doubles representing the amplitude of the signal
		 * in dB, and outputs a sequence of Ping objects representing
		 * detections.
		 * @param input_queue  Input queue
		 * @param input_mutex  Input mutex
		 * @param input_cv     Input condition variable
		 * @param output_queue Output queue
		 * @param output_mutex Output mutex
		 * @param output_cv    Output condition variable
		 * @param run          Run flag
		 */
		void _process(std::queue<double>& input_queue, std::mutex& input_mutex,
			std::condition_variable& input_cv, 
			std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv);
		volatile bool _run = false;

		void debounce(boost::circular_buffer<bool>::iterator it);
	};
}

#endif