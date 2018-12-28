#ifndef __DSPV2_H__
#define __DSPV2_H__

#include "dsp.hpp"
#include "Processor.hpp"
#include <mutex>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>

namespace RTT{
	class IQdata;

	class DSP_V2 : public DSP{
	private:
		/**
		 * Center frequency of incoming data
		 */
		const std::size_t _center_freq;

		/**
		 * Sampling frequency of incoming data
		 */
		const std::size_t _sampling_freq;
		
		/**
		 * Vector of frequencies to process
		 */
		// std::vector<uint32_t> _frequencies;
		 std::uint32_t _frequency;

		/**
		 * Array of processor objects
		 */
		Processor _processor;

		/**
		 * Time of first sample
		 */
		std::size_t time_start_ms = 0;

		/**
		 * Thread group containing all the threads related to processing.
		 */
		std::thread* _copy_thread = nullptr;

		/**
		 * Input condition variable for bumping the copy thread
		 */
		std::condition_variable* _input_var = nullptr;
		
		/**
		 * Internal queues from the stream splitter (copyQueue) to processing
		 * threads (process).
		 */
		std::queue<std::complex<double>> _innerQueue;

		/**
		 * Mutexes for each internal data queue
		 */
		std::mutex _innerMutex;

		/**
		 * Conditional variables for internal data queues
		 */
		std::condition_variable _innerVar;


		/**
		 * Frame size
		 */
		std::size_t _frame_size;

		void _process(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<PingPtr>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv, const volatile bool* run,
			const std::size_t frequency);

		/**
		 * Splits data to each individual process thread
		 * @param ndie       Not Die flag
		 * @param inputQueue Input Queue to pull data from
		 * @param inputMutex Mutex on Input Queue
		 */
		void copyQueue(const volatile bool* ndie, 
			std::queue<IQdataPtr>& inputQueue, std::mutex& inputMutex,
			std::condition_variable& inputVar);
	public:
		/**
		 * Constructor
		 * @param freqs2process	RF Frequencies to process in Hz
		 * @param center_freq	Center frequency of IQ data
		 */
		DSP_V2(const std::vector<uint32_t>& freqs2process, 
			const std::size_t center_freq, const std::size_t sampling_freq, 
			const std::size_t frame_size);

		~DSP_V2();

		void startProcessing(std::queue<IQdataPtr>& inputQueue,
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex, 
			std::condition_variable& outputVar, const volatile bool* ndie);
		void stopProcessing();
	};
}

#endif