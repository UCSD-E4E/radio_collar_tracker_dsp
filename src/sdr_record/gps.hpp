#ifndef __GPS_H__
#define __GPS_H__

#include <string>
#include <map>
#include "gps_core.hpp"
#include "timeblock.hpp"
#include <queue>
#include "location.hpp"
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	class GPS{

	private:
		/**
		 * Internal handle to the GPS Core object, which handles interfacing
		 * with the underlying data stream
		 */
		GPSCore* _core;

		/**
		 * Map of TimeBlock objects and Location pointers.
		 */
		std::map<TimeBlock, Location*> pointLookup;

		/**
		 * Queue of Location pointers.
		 */
		std::queue<Location*> pointQueue;
		/**
		 * Mutex for the point queue.
		 */
		std::mutex pointMutex;
		/**
		 * Condition variable for items in the queue.
		 */
		std::condition_variable pointVar;

		std::thread* _map_thread;

		volatile bool _run = false;

		std::size_t first_time = 0;
		std::size_t last_time = 0;

		Location* lastLocation;

	protected:

		/**
		 * Execution thread function
		 */
		void _thread();
	public:
		/**
		 * Protocol definitions
		 */
		enum Protocol{
			/**
			 * For a CSV data file, with the following fields:
			 * ltime - local time in seconds since Unix epoch as a float
			 * lat - latitude in degrees * 1e7 with regards to the WGS84 datum 
			 * 		as an int
			 * lon - longitude in degrees * 1e7 with regards to the WGS84 datum 
			 * 		as an int
			 * gtime - GPS time in seconds since GPS epoch as a float
			 * alt - altitude in meters above sea level as defined by the WGS84
			 * 		spheroid as an int
			 * rel_alt - altitude in meters above the starting altitude
			 * vx - velocity in meters per second with regards to the x axis
			 * 		as an int
			 * vy - velocity in meters per second with regards to the y axis
			 * 		as an int
			 * vz - velocity in meters per second with regards to the z axis
			 * 		as an int
			 * hdg - heading in degrees from True North as an int
			 */
			TEST_FILE,
			SERIAL,
		};
		GPS(GPS::Protocol, std::string);	
		const Location* getPositionAt(uint64_t);

		void start();
		void stop();

		void setOutputFile(const std::string);
	};
}

#endif