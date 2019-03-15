#ifndef __RTT_LOCAL_H__
#define __RTT_LOCAL_H__

#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "ping.hpp"
#include "gps.hpp"
// #include <dlib/optimization.h>

namespace RTT{
	

	class PingLocalizer{

	public:
		// typedef dlib::matrix<double, 5, 1> parameter_vector;
		// typedef dlib::matrix<double, 4, 1> input_vector;
		PingLocalizer();
		~PingLocalizer();
		void start(std::queue<PingPtr>&, std::mutex&, std::condition_variable&, GPS&, const volatile bool* die);
		void stop();
	protected:
	private:
		void process(std::queue<PingPtr>& queue, std::mutex& mutex, std::condition_variable&, GPS& gps_module, const volatile bool* die);
		std::thread* localizer_thread;
		std::condition_variable* _input_cv;
		// parameter_vector params;

	};
}

#endif