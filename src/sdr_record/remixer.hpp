#ifndef __REMIXER_H__
#define __REMIXER_H__

#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class Remixer{
	private:
		std::size_t _period;
		std::complex<double>* _bbeat;
		std::thread* _thread;
		std::condition_variable* _input_cv;
		std::size_t _upsample_factor;
		std::size_t _downsample_factor;
		void _process(std::queue<std::complex<double>>& input_queue, 
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<std::complex<double>>& output_queue,
			std::mutex& output_mutex, std::condition_variable& output_cv,
			const volatile bool* run);
		/**
		 * Aggregates N elements into the data from queue.
		 * @param queue	Queue to grab elements from
		 * @param data	Array to dump to
		 * @param N		Number of elements to gather
		 */
		void aggregate(std::queue<std::complex<double>>& queue,
			std::unique_lock<std::mutex>& lock, std::condition_variable& q_cv, 
			std::complex<double>* data, std::size_t N, const volatile bool* run);
		/**
		 * Process a block of data of size _period.
		 * @param data	Pointer to a block of memory of at least _period 
		 *             	elements
		 */
		void func(std::complex<double>* data, std::complex<double>* output);
	protected:
	public:
		/**
		 * Creates a remixer object that shifts the signal at the sampling
		 * frequency by the shift amount and resmaples with the given resampling
		 * factors.
		 * @param shift					Amount to shift by
		 * @param sampling_frequency	Sampling frequency of the signal
		 * @param up_factor				Upsampling factor
		 * @param down_factor			Downsampling factor
		 */
		Remixer(std::int64_t shift, std::size_t sampling_frequency,
			std::size_t up_factor, std::size_t down_factor);

		~Remixer();

		/**
		 * Starts this thread.  This thread mixes down and resamples data from
		 * the input queue and places it into the output queue while run is
		 * true.
		 * @param input_queue  Input queue
		 * @param input_mutex  Input mutex
		 * @param input_cv     Input Condition Variable
		 * @param output_queue Output queue
		 * @param output_mutex Output mutex
		 * @param output_cv    Output condition variable
		 * @param run          Run flag
		 */
		void start(std::queue<std::complex<double>>& input_queue, 
			std::mutex& input_mutex, std::condition_variable& input_cv, 
			std::queue<std::complex<double>>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_cv, 
			const volatile bool* run);

		/**
		 * Indicates to this thread to stop, and waits for the thread to finish.
		 */
		void stop();
	};
}

#endif