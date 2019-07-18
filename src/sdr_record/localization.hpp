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

namespace RTT{
	

	class PingLocalizer{

	public:
		// typedef dlib::matrix<double, 5, 1> parameter_vector;
		// typedef dlib::matrix<double, 3, 1> input_vector;
		PingLocalizer(std::ostream& out);
		~PingLocalizer();
		void start(std::queue<PingPtr>&, std::mutex&, std::condition_variable&, 
			GPS&);
		void stop();

		double getLat();
		double getLon();
		double getK1();
		double getK2();
	protected:
	private:
		void process(std::queue<PingPtr>& queue, std::mutex& mutex,
			std::condition_variable&, GPS& gps_module);
		volatile bool run;
		std::thread* localizer_thread;
		std::condition_variable* _input_cv;
		// parameter_vector params;
		// struct estimate_result{
		// 	parameter_vector params;
		// 	double mse;
		// };

		// estimate_result _estimate;

		// estimate_result& estimate(std::vector<std::pair<input_vector, double>> data);

		std::ostream& _out;
		std::queue<PingPtr> pingQueue;
	};
}

#endif
