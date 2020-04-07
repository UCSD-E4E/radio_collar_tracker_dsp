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
	/**
	 * GPS module.  This is responsible for aggregating data from the underlying
	 * GPS datastream, and referencing timestamps to particular positions.
	 */
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

		/**
		 * Pointer to the aggregator thread
		 */
		std::thread* _map_thread;

		/**
		 * Internal run flag.
		 */
		volatile bool _run = false;

		/**
		 * Timestamp of first GPS position in ms since Unix epoch
		 */
		std::size_t first_time = 0;

		/**
		 * Timestamp of latest GPS position in ms since Unix epoch
		 */
		std::size_t last_time = 0;

		/**
		 * Pointer to the last received GPS location for quick reference.
		 */
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

			/**
			 * For data coming over a serial line as JSON packets.  Each JSON
			 * packet must have the following:
			 * lat - latitude in degrees * 1e7 with regards to the WGS84 datum 
			 * 		as an int
			 * lon - longitude in degrees * 1e7 with regards to the WGS84 datum 
			 * 		as an int
			 * hdg - heading in degrees from True North as an int
			 * tme - GPS week time
			 * run - "true" or "false" as the run switch status
			 * fix - Fix type, as defined in Sensor_Module::GPSFix
			 * sat - Number of satellites used in the GPS fix
			 * dat - GPS Date
			 */
			SERIAL,

			/**
			 * No GPS datastream.  Default values
			 */
			TEST_NULL,
		};

		/**
		 * Constructor for the GPS module.  Specify appropriate protocol and
		 * device handle.  For GPS::Protocol::TEST_FILE, specify the source
		 * file.  For GPS::Protocol::SERIAL, specify the serial device. For
		 * GPS::Protocol::TEST_NULL, path is ignored.
		 *
		 * @param	protocol	GPS data source type
		 * @param	path		GPS data source location
		 */
		GPS(GPS::Protocol protocol, const std::string &path);

		/**
		 * Gets the estimated location at the given timestamp.  Time is 
		 * specified in ms since Unix epoch
		 * @param  t_ms Timestamp at which to get position
		 * @return      Estimated location at given timestamp.
		 */
		const Location* getPositionAtMs(uint64_t t_ms);

		/**
		 * Initializes the GPS module
		 */
		void start();

		/**
		 * Stops the GPS module from parsing more data
		 */
		void stop();

		/**
		 * Sets the output data location.
		 * @param path Path to write data to
		 */
		void setOutputFile(const std::string path);

		/**
		 * Only valid for GPS::Protocol::TEST_FILE.  Waits for the system to
		 * load all data from file.  Behavior for GPS::Protocol::SERIAL is 
		 * undefined!
		 */
		void waitForLoad();

		/**
		 * Returns the timestamp of the first location in ms since the Unix epoch
		 * @return Timestamp of first location
		 */
		const std::size_t getFirst_ms() const;

		/**
		 * Waits for first location fix from underlying datastream.
		 */
		void waitForPos();
	};
}

#endif
