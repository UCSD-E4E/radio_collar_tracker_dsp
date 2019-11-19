#include "gps.hpp"
#include "gps_test.hpp"
#include "serial_gps.hpp"
#include <chrono>
#include <iostream>
#include <sys/time.h>

// #define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

namespace RTT{
	GPS::GPS(GPS::Protocol protocol, std::string path){
		switch(protocol){
			case GPS::Protocol::TEST_FILE:
				_core = new GPSTest(path);
				break;
			case GPS::Protocol::SERIAL:
				_core = new SerialGPS{path};
				break;
			case GPS::Protocol::TEST_NULL:
				_core = nullptr;
				break;
		}
	}

	void GPS::start(){
		if(_core){
			_core->start(pointQueue, pointMutex, pointVar);
			_run = true;
			_map_thread = new std::thread(&GPS::_thread, this);
		}else{
			struct timeval start;
			gettimeofday(&start, NULL);
			first_time = start.tv_sec * 1e3 + start.tv_usec / 1e3;
			#ifdef DEBUG
			std::cout << "GPS starting with first time of " << first_time << " ms" << std::endl;
			#endif
		}
	}

	void GPS::waitForLoad(){
		if(_core){
			_core->stop();
			_run = false;
			std::unique_lock<std::mutex> lock(pointMutex);
			pointVar.notify_all();
			lock.unlock();
			_map_thread->join();
		}
	}

	void GPS::stop(){
		_run = false;
		if(_core){
			_core->stop();
		}
		std::unique_lock<std::mutex> lock(pointMutex);
		pointVar.notify_all();
		lock.unlock();
		_map_thread->join();
	}

	void GPS::_thread(){
		uint64_t prev_time = 0;
		size_t count = 0;

		#ifdef DEBUG
		struct timeval rxtime;
		#endif

		while(_run){
			std::unique_lock<std::mutex> lock(pointMutex);
			if(pointQueue.empty()){
				pointVar.wait(lock);
			}
			if(!pointQueue.empty()){
				Location* point = pointQueue.front();
				pointQueue.pop();
				count++;
				lock.unlock();

				#ifdef DEBUG
				gettimeofday(&rxtime, NULL);
				std::cout << "Received point at " << point->ltime << " at " << rxtime.tv_sec << "s" << std::endl;
				#endif

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
				lastLocation = point;
			}
		}
		// std::cout << "Got " << count << " points" << std::endl;
	}

	const Location* GPS::getPositionAtMs(uint64_t t){
		TimeBlock tblock(t);
		if(_core == nullptr){
			pointLookup[tblock] = new Location{};
			pointLookup[tblock]->ltime = t;
			pointLookup[tblock]->lat = 0;
			pointLookup[tblock]->lon = 0;
			return pointLookup[tblock];
		}
		if(t > last_time && t < last_time + 1000){
			return lastLocation;
		}
		if(t < first_time || t > last_time){
			#ifdef DEBUG
			std::cout << "First time: " << first_time << ", Last time: " << last_time << ", Requested " << t << std::endl;
			#endif
			
			return nullptr;
		}
		return pointLookup[tblock];
	}

	void GPS::setOutputFile(const std::string file){
		if(_core){
			_core->setOutputFile(file);
		}
	}

	const std::size_t GPS::getFirst_ms() const{
		return first_time;
	}

	void GPS::waitForPos(){
		std::unique_lock<std::mutex> lock(pointMutex);
		pointVar.wait(lock);
	}

}