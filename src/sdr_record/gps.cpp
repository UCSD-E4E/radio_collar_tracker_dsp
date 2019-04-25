#include "gps.hpp"
#include "gps_test.hpp"
#include "serial_gps.hpp"
#include <chrono>
#include <iostream>

namespace RTT{
	GPS::GPS(GPS::Protocol protocol, std::string path){
		switch(protocol){
			case GPS::Protocol::TEST_FILE:
				_core = new GPSTest(path);
				break;
			case GPS::Protocol::SERIAL:
				_core = new SerialGPS{path};
				break;
		}
	}

	void GPS::start(){
		_core->start(pointQueue, pointMutex, pointVar);
		_run = true;
		_map_thread = new std::thread(&GPS::_thread, this);
	}

	void GPS::stop(){
		_core->stop();
		_run = false;
		_map_thread->join();
	}

	void GPS::_thread(){
		uint64_t prev_time = 0;
		size_t count = 0;
		while(_run || !pointQueue.empty()){
			std::unique_lock<std::mutex> lock(pointMutex);
			if(pointQueue.empty()){
				pointVar.wait(lock);
			}
			if(!pointQueue.empty()){
				Location* point = pointQueue.front();
				pointQueue.pop();
				count++;
				lock.unlock();

				if(first_time == 0){
					first_time = point->ltime;
				}
				last_time = point->ltime;

				if(prev_time == 0){
					prev_time = point->ltime - 10000;
				}
				TimeBlock tblock(prev_time + 1, point->ltime);
				prev_time = point->ltime;
				pointLookup[tblock] = point;
			}
		}
	}

	const Location* GPS::getPositionAt(uint64_t t){
		TimeBlock tblock(t);
		if(t < first_time || t > last_time){
			return nullptr;
		}
		return pointLookup[tblock];
	}
}