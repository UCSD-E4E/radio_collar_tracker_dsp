#ifndef __RESAMPLER_H__
#define __RESAMPLER_H__

#include <queue>
#include <complex>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class Resampler{
	private:
		/**
		 * Upsampling factor
		 */
		std::size_t _upsample_factor;
		/**
		 * Downsampling factor.
		 */
		std::size_t _downsample_factor;
		/**
		 * Processing thread pointer
		 */
		std::thread* _thread;
		/**
		 * Input queue condition variable
		 */
		std::condition_variable* _input_cv;

		/**
		 * Process thread definition.  This takes data from the input queue,
		 * resamples it as needed, then places the resulting signal in the 
		 * output queue.
		 * @param input_queue  Input Queue
		 * @param input_mutex  Input mutex
		 * @param input_cv     Input condition variable
		 * @param output_queue Output queue
		 * @param output_mutex Output mutex
		 * @param output_cv    Output condition variable
		 * @param run          Run flag
		 */
		void _process(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<std::complex<double>>& output_queue,
			std::mutex& output_mutex, std::condition_variable& output_cv,
			const volatile bool* run);

	public:
		/**
		 * Creates a resampling object with the given upsample and downsample
		 * ratios.
		 * @param upsample_factor	Factor by which to upsample
		 * @param downsample_factor	Factor by which to downsample
		 */
		Resampler(std::size_t upsample_factor, std::size_t downsample_factor);

		/**
		 * Deconstructor
		 */
		~Resampler();

		/**
		 * Starts this thread.  This thread resamples data from the input queue
		 * and places it into the output queue while run is true.
		 * @param input_queue  Input queue
		 * @param input_mutex  Input mutex
		 * @param input_cv     Input condition variable
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
		 * Stops this thread, and waits for the thread to complete.
		 */
		void stop();
	};
}

#endif
