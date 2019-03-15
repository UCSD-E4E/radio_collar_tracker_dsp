#include "localization.hpp"
#include <queue>
#include <mutex>
#include <thread>
// #include <dlib/optimization.h>
#include <vector>
#include <math.h>
#include <iostream>

namespace RTT{


	double distance(const double ref_x, const double ref_y, const double ref_z,
		const double obj_x, const double obj_y, const double obj_z){

		const double diff_x = ref_x - obj_x;
		const double diff_y = ref_y - obj_y;
		const double diff_z = ref_z - obj_z;

		const double sqr = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;

		return sqrt(sqr);
	}

	// double model(const PingLocalizer::input_vector& input, const PingLocalizer::parameter_vector& params){
	// 	const double amplitude = input(0);
	// 	const double drone_x = input(1);
	// 	const double drone_y = input(2);
	// 	const double drone_z = input(3);

	// 	const double alpha = params(0);
	// 	const double beta = params(1);
	// 	const double tx_x = params(2);
	// 	const double tx_y = params(3);
	// 	const double tx_z = params(4);

	// 	const double dist = distance(drone_x, drone_y, drone_z, tx_x, tx_y, tx_z);

	// 	return amplitude * alpha / (pow(dist, beta));

	// }

	// double residual(const std::pair<PingLocalizer::input_vector, double>& data, const PingLocalizer::parameter_vector& params){
	// 	return model(data.first, params) - data.second;
	// }

	void PingLocalizer::process(std::queue<PingPtr>& queue, std::mutex& mutex,
		std::condition_variable& var, GPS& gps_module, const volatile bool* die){
		
		// std::vector<std::pair<input_vector, double>> data_samples;
		// input_vector input;
		// params = 1, 2, 0, 0, 0;
		
		// while(!*die && !queue.empty()){
		// 	std::cout << "itr" << std::endl;
		// 	std::unique_lock<std::mutex> inputLock(mutex);
		// 	if(queue.empty()){
		// 		var.wait(inputLock);
		// 	}else{
		// 		// add ping to data matrix
		// 		Ping ping = *queue.front();
		// 		queue.pop();
		// 		inputLock.unlock();
		// 		std::cout << "Got ping" << std::endl;
		// 		input(1) = ping.amplitude;
		// 		Location loc = gps_module.getPositionAt(ping.time_ms);
		// 		input(2) = loc.lon;
		// 		input(3) = loc.lat;
		// 		input(4) = loc.alt;

		// 		if(params(2) == 0){
		// 			params(2) = loc.lon;
		// 			params(3) = loc.lat;
		// 			params(4) = loc.alt;
		// 		}

		// 		const double output = model(input, params);

		// 		data_samples.push_back(std::make_pair(input, output));
		// 		if(data_samples.size() > 5){
		// 			const double error = dlib::solve_least_squares_lm(dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
		// 				residual, dlib::derivative(residual), data_samples, params);
		// 			std::cout << "run" << std::endl;
		// 		}
		// 	}
		// }
	}

	void PingLocalizer::start(std::queue<PingPtr>& queue, std::mutex& mutex, std::condition_variable& var, GPS& gps_module, const volatile bool* die){
		_input_cv = &var;
		std::cout << "Start" << std::endl;
		localizer_thread = new std::thread(&PingLocalizer::process, this, std::ref(queue), std::ref(mutex), std::ref(var), std::ref(gps_module), die);
	}

	void PingLocalizer::stop(){
		_input_cv->notify_all();
		localizer_thread->join();
		delete localizer_thread;
	}

	PingLocalizer::PingLocalizer(){
		
	}
	PingLocalizer::~PingLocalizer(){
		
	}
}