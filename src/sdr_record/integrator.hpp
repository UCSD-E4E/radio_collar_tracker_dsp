#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class Integrator{
	public:
		Integrator(const std::size_t factor);
		~Integrator();
		void start(std::queue<double>& input_queue, std::mutex& input_mutex,
			std::condition_variable& input_cv, std::queue<double>& output_queue,
			std::mutex& output_mutex, std::condition_variable& output_cv);
		void stop();
	private:
		std::condition_variable* _input_cv;
		std::thread* _thread;
		std::size_t _decimation;

		void _process(std::queue<double>& input_queue, std::mutex& input_mutex, 
			std::condition_variable& input_cv, std::queue<double>& output_queue,
			std::mutex& output_mutex, std::condition_variable& output_cv);
		volatile bool _run = false;
	};
}

#endif