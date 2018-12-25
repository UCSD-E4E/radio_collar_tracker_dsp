#include "gps_test.hpp"

#include <sstream>
#include <limits>
#include <iostream>

namespace RTT{
	void GPSTest::start(std::queue<Location*>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_var,
			const volatile bool* run){
		size_t count = 0;
		while(!data_source->eof()){
			std::string line;
			std::getline(*data_source, line);
			Location& point = parseLocation(line);

			std::unique_lock<std::mutex> guard(output_mutex);
			output_queue.push(&point);
			count++;
			guard.unlock();
			output_var.notify_all();
		}
	}

	void GPSTest::stop(){
	}

	Location& GPSTest::parseLocation(const std::string line){
		Location* retval = new Location();
		std::stringstream data(line, std::ios_base::in);
		
		double raw_ltime;
		double raw_gtime;


		data >> raw_ltime;
		retval->ltime = raw_ltime * 1e3;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->lat;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->lon;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> raw_gtime;
		retval->gtime = raw_gtime * 1e3;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->alt;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->rel_alt;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->vx;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->vy;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->vz;
		data.ignore(std::numeric_limits<std::streamsize>::max(),',');
		data >> retval->hdg;
		return *retval;
	}

	GPSTest::GPSTest(const std::string path){
		data_source = new std::ifstream(path.c_str(), std::ios_base::in);
	}

	GPSTest::GPSTest(const char* path){
		data_source = new std::ifstream(path, std::ios_base::in);
	}
}