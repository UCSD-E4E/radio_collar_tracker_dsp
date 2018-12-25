#include "gps.hpp"
#include "gps_test.hpp"
#include <chrono>
#include <iostream>

namespace RTT{
	GPS::GPS(GPS::Protocol protocol, std::string path){
		switch(protocol){
			case GPS::Protocol::TEST_FILE:
				_core = new GPSTest(path);
				break;
		}
	}

	void GPS::start(const volatile bool* run){
		_core->start(pointQueue, pointMutex, pointVar, run);
		_map_thread = new std::thread(&GPS::_thread, this, run);
	}

	void GPS::stop(){
		_core->stop();
		_map_thread->join();
	}

	void GPS::_thread(const volatile bool* run){
		uint64_t prev_time = 0;
		size_t count = 0;
		while(*run || !pointQueue.empty()){
			std::unique_lock<std::mutex> lock(pointMutex);
			if(pointQueue.empty()){
				pointVar.wait_for(lock, std::chrono::milliseconds{10});
			}
			if(!pointQueue.empty()){
				Location* point = pointQueue.front();
				pointQueue.pop();
				count++;
				lock.unlock();

				if(prev_time == 0){
					prev_time = point->ltime - 10000;
				}
				TimeBlock tblock(prev_time + 1, point->ltime);
				prev_time = point->ltime;
				pointLookup[tblock] = point;
			}
		}
	}

	const Location& GPS::getPositionAt(uint64_t time){
		TimeBlock tblock(time);
		return *pointLookup[tblock];
	}
}