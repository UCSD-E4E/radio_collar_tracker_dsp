#ifndef __SERIAL_GPS__
#define __SERIAL_GPS__

#include "gps_core.hpp"
#include <string>
#include <thread>

namespace RTT{
	/**
	 * GPS Core for serial gps modules.  This particular module accepts GPS data
	 * as JSON packets.  Each JSON packet must have the following:
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
	class SerialGPS : public GPSCore{
	private:
		/**
		 * Parses the location from the given string. Each JSON packet must have
		 * the following:
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
		 * @param  std::string String containing the location information
		 * @return             A reference to a fully initialized Location 
		 *                     object.
		 * @throws boost::ptree_error if the packet could not be successfully
		 *         					  parsed
		 */
		Location& parseLocation(const std::string);

		/**
		 * Device path
		 */
		std::string* dev_path;

		/**
		 * Internal state variable
		 */
		volatile bool _run = false;

		/**
		 * Thread handle
		 */
		std::thread* _thread;

		/**
		 * Thread to receive and parse data from the serial device.  Locations
		 * will be placed into the queue.
		 * @param o_q output queue
		 * @param o_m output queue mutex
		 * @param o_v output queue condition variable
		 */
		void _process(std::queue<Location*>& o_q, std::mutex& o_m,
			std::condition_variable& o_v);

		/**
		 * Output file to log data into
		 */
		std::string _outputFile;
	protected:
	public:
		/**
		 * Starts the thread to read in data and output Location pointers to the
		 * supplied queue.
		 * @param o_q Output queue of RTT::Location pointers.
		 * @param o_m Mutex for the output queue
		 * @param o_v Condition variable for the output queue
		 */
		void start(std::queue<Location*>& o_q, std::mutex& o_m, 
			std::condition_variable& o_v);

		/**
		 * Stops the data reading thread.
		 */
		void stop();

		/**
		 * Creates a new SerialGPS object pointed at the specified device.
		 * @param path Path to Linux device node
		 */
		SerialGPS(const char* path);

		/**
		 * Creates a new SerialGPS object pointed at the specified device.
		 * @param path Path to Linux device node
		 */
		SerialGPS(const std::string path);

		/**
		 * Sets the output file to log locations into
		 * @param std::string Path to output file.
		 */
		void setOutputFile(const std::string);
	};
}

#endif