#ifndef __PING_CLASSIFIER_H__
#define __PING_CLASSIFIER_H__

#include "ping.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class PingClassifier{
	private:
		/**
		 * Ping threshold in dB
		 */
		double _threshold;

		/**
		 * Start time of signal stream in ms
		 */
		const std::size_t& _time_start_ms;

		/**
		 * Milliseconds per sample
		 */
		const double _ms_per_sample;

		/**
		 * threshold mutex
		 */
		std::mutex _threshold_mutex{};

		/**
		 * Count of samples.  This is the index of the sample at the front of
		 * the input_queue.
		 */
		std::size_t _sample_count = 0;

		/**
		 * Thread pointer
		 */
		std::thread* _thread = nullptr;

		/**
		 * Input condition variable
		 */
		std::condition_variable* _input_cv = nullptr;

		/**
		 * Threshold update period
		 */
		std::size_t threshold_update_period = 10000;

		/**
		 * Takes in a queue of doubles representing the amplitude of match, and
		 * outputs a sequence of Ping objects representing detections.
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
			std::condition_variable& output_cv, const volatile bool* run);

	public:
		PingClassifier(const double threshold, const std::size_t& time_start_ms,
			const double ms_per_sample);
		~PingClassifier();

		void start(std::queue<double>& input_queue, std::mutex& input_mutex,
			std::condition_variable& input_cv, 
			std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv, const volatile bool* run);

		void stop();

		void setThreshold(const double threshold);
	};
}

#endif