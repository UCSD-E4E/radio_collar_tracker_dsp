#include "localization.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <dlib/optimization.h>
#include <vector>
#include <math.h>
#include <iostream>
#include "UTM.h"

// #define DEBUG

#ifdef DEBUG
#include <fstream>
#include <iostream>
#endif

namespace RTT{


	double distance(const double ref_x, const double ref_y, const double ref_z,
		const double obj_x, const double obj_y, const double obj_z){

		const double diff_x = ref_x - obj_x;
		const double diff_y = ref_y - obj_y;
		const double diff_z = ref_z - obj_z;

		const double sqr = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;

		return sqrt(sqr);
	}

	double model(const PingLocalizer::input_vector& input, const PingLocalizer::parameter_vector& params){
		// const double amplitude = input(0);
		const double drone_x = input(0);
		const double drone_y = input(1);
		const double drone_z = input(2);

		const double tx_pow = params(0);
		const double order = params(1);
		const double tx_x = params(2);
		const double tx_y = params(3);
		const double tx_z = params(4);

		const double dist = distance(drone_x, drone_y, drone_z, tx_x, tx_y, tx_z);

		return tx_pow - 10 * order * std::log10(dist);
	}

	double residual(const std::pair<PingLocalizer::input_vector, double>& data, const PingLocalizer::parameter_vector& params){
		return model(data.first, params) - data.second;
	}

	PingLocalizer::estimate_result& PingLocalizer::estimate(
		std::vector<std::pair<input_vector, double>> data){
		PingLocalizer::estimate_result* result = new PingLocalizer::estimate_result ;
		result->params = params;
		result->mse = dlib::solve_least_squares_lm(
			dlib::objective_delta_stop_strategy(1e-7),
			residual, 
			dlib::derivative(residual),
			data,
			result->params);
		return *result;
	}

	void PingLocalizer::process(std::queue<PingPtr>& queue, std::mutex& mutex,
		std::condition_variable& var, GPS& gps_module){
		
		std::vector<std::pair<input_vector, double>> data_samples;
		input_vector input;
		params = 90, 0.125, 0, 0, 0;
		
		#ifdef DEBUG
		std::ofstream _pings{"localizer_in.log"};
		std::ofstream _estimates{"localizer_out.log"};
		std::ofstream _bad_gps{"localizer_err.log"};
		std::ofstream _test{"localizer_check.log"};
		#endif

		while(run || !queue.empty()){
		// 	std::cout << "itr" << std::endl;
			std::unique_lock<std::mutex> inputLock(mutex);
			if(queue.empty()){
				var.wait(inputLock);
			}
			if(!queue.empty()){
				// add ping to data matrix
				Ping ping = *queue.front();
				queue.pop();
				inputLock.unlock();
				const Location* loc = gps_module.getPositionAt(ping.time_ms);
				if(loc == nullptr){
					#ifdef DEBUG
					std::cout << "No GPS data at " << ping.time_ms << "!" << std::endl;
					_bad_gps << ping.time_ms << std::endl;
					#endif
					continue;
				}

				// convert to UTM

				double northing;
				double easting;
				char zone[4];
				UTM::LLtoUTM(loc->lat * 1e-7, loc->lon * 1e-7, northing, easting, zone);

				input(0) = easting;
				input(1) = northing;
				input(2) = loc->alt;

				#ifdef DEBUG
				std::cout << "Got ping at " << (int)(ping.time_ms / 1e3) % (4000)
					<< " at " << loc->lat << ", " << loc->lon << std::endl;
				_pings << ping.time_ms << ", " << ping.amplitude << ", " 
					<< (long long)northing << ", " << (long long)easting << ", " << loc->alt 
					<< std::endl;
				#endif

				if(params(2) == 0){
					params(2) = easting + 1;
					params(3) = northing + 1;
					params(4) = 0;
				}

				const double output = ping.amplitude;

				data_samples.push_back(std::make_pair(input, output));

				#ifdef DEBUG
				for (auto it = data_samples.begin(); it != data_samples.end(); it++){
					_test << "{" << it->first(0) << ", " << (long long)((it->first(1))) << ", "
						<< (long long)((it->first(2))) <<  ", " <<
						it->second << "}, ";
				}
				_test << std::endl;
				#endif

				if(data_samples.size() >= 5){
					PingLocalizer::estimate_result& result = estimate(data_samples);
					params = result.params;
					double lat;
					double lon;
					UTM::UTMtoLL(result.params(3), result.params(2), zone, lat, lon);
					std::cout << "Estimate run, estimate at: " << lat 
						<< ", " << lon << " with " << data_samples.size() << " data" << std::endl;
					#ifdef DEBUG
					_estimates << result.params(0) << ", " << result.params(1) << ", " 
						<< (long long)result.params(2) << ", " << (long long)result.params(3) << ", " 
						<< (long long)result.params(4) << ", " << result.mse << std::endl;
					#endif
					delete &result;
				}
			}
		}
		#ifdef DEBUG
		_pings.close();
		_estimates.close();
		_bad_gps.close();
		_test.close();
		#endif

	}

	void PingLocalizer::start(std::queue<PingPtr>& queue, std::mutex& mutex, std::condition_variable& var, GPS& gps_module){
		_input_cv = &var;
		run = true;
		localizer_thread = new std::thread(&PingLocalizer::process, this, std::ref(queue), std::ref(mutex), std::ref(var), std::ref(gps_module));
	}

	void PingLocalizer::stop(){
		run = false;
		_input_cv->notify_all();
		localizer_thread->join();
		delete localizer_thread;
	}

	PingLocalizer::PingLocalizer(){
		
	}
	PingLocalizer::~PingLocalizer(){
		
	}
}