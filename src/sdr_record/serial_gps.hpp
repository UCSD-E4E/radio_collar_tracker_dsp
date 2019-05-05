#ifndef __SERIAL_GPS__
#define __SERIAL_GPS__

#include "gps_core.hpp"
#include <string>
#include <thread>

namespace RTT{
	class SerialGPS : public GPSCore{
	private:
		/**
		 * Parses the location from the given string.
		 * @param  std::string String containing the location information
		 * @return             A reference to a fully initialized Location 
		 *                     object.
		 */
		Location& parseLocation(const std::string);

		std::string* dev_path;

		volatile bool _run = false;

		std::thread* _thread;

		void _process(std::queue<Location*>& o_q, std::mutex& o_m,
			std::condition_variable& o_v);

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

		SerialGPS(const char* path);
		SerialGPS(const std::string path);
		void setOutputFile(const std::string);
	};
}

#endif