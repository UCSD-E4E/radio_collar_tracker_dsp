#ifndef __MIXER_H__
#define __MIXER_H__

#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class Mixer{
		// FIXME implement period limiting to avoid memory hogging
		/**
		 * Maximum Period to store, periods exceeding this should implement as
		 * calculate on the fly.
		 */
		const std::size_t MAX_PERIOD = 2048;
	private:
		/**
		 * Period of shift signal in samples.
		 */
		std::size_t _period;
		/**
		 * Shift signal
		 */
		std::complex<double>* _bbeat;
		/**
		 * Processing thread
		 */
		std::thread* _thread;
		/**
		 * Input condition variable
		 */
		std::condition_variable* _input_cv;

		/**
		 * Processing thread definition
		 * @param input_queue  Input Queue
		 * @param input_mutex  Mutex for Input Queue
		 * @param input_cv     Condition Variable for input queue
		 * @param output_queue Output Queue
		 * @param output_mutex Mutex for Output Queue
		 * @param output_cv    Condition Variable for output queue
		 * @param run          Run flag
		 */
		void _process(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<std::complex<double>>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_cv,
			const volatile bool* run);
	public:
		/**
		 * Mixer object constructor
		 * @param shift					Frequency to shift by
		 * @param sampling_frequency	Sampling frequency of incoming signal
		 */
		Mixer(std::int64_t shift, std::size_t sampling_frequency);
		/**
		 * Destructor
		 */
		~Mixer();

		/**
		 * Start the processing thread
		 * @param input_queue  Input Queue
		 * @param input_mutex  Mutex for input queue
		 * @param input_cv     Condition Variable for input queue
		 * @param output_queue Output queue
		 * @param output_mutex Mutex for output queue
		 * @param output_cv    Condition Variable for output queue
		 * @param run          Run flag
		 */
		void start(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<std::complex<double>>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_cv,
			const volatile bool* run);

		/**
		 * Stops the processing thread.  This method blocks until all incoming
		 * data has been consumed.
		 */
		void stop();
	};
}

#endif