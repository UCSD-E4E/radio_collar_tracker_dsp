#ifndef __FIR_H__
#define __FIR_H__

#include <complex>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{

	/**
	 * Finite Impulse Response Class
	 */
	class PingFIR{
	private:
		std::size_t _num_taps;
		std::complex<double>* _filter_taps;
		// std::queue<std::complex<double>>* _input_queue;
		// std::mutex* _input_mutex;
		std::condition_variable* _input_cv;
		// std::queue<double>* _output_queue;
		// std::mutex* _output_mutex;
		// std::condition_variable* _output_cv;
		std::thread* _thread;

		void _process(std::queue<std::complex<double>>& input_queue,
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<double>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv, const volatile bool* run);

	public:
		PingFIR(std::size_t frequency, std::size_t sampling_frequency,
			std::size_t size);
		~PingFIR();
		void start(std::queue<std::complex<double>>& input_queue, 
			std::mutex& input_mutex, std::condition_variable& input_cv,
			std::queue<double>& output_queue, std::mutex& output_mutex,
			std::condition_variable& output_cv, const volatile bool* run);
		void stop();
	};
}

#endif