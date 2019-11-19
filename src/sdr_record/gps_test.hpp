#ifndef __GPS_TEST_H__
#define __GPS_TEST_H__

#include <queue>
#include <timeblock.hpp>
#include <location.hpp>
#include "gps_core.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace RTT{
	/**
	 * Test GPS core module.  This loads data from a local file.
	 */
	class GPSTest: public GPSCore{
	private:
		/**
		 * Pointer to the input stream to the data source.
		 */
		std::ifstream* data_source;

		/**
		 * Loads data from the data_source stream into the queue.
		 */
		void load_data();

		/**
		 * Parses the location from the given string.
		 * @param  input       String containing the location information
		 * @return             A reference to a fully initialized Location 
		 *                     object.
		 */
		Location& parseLocation(const std::string input);

		void _process(std::queue<Location*>&, std::mutex&, std::condition_variable&);

		std::thread* _thread;

	protected:
	public:


		/**
		 * Starts the thread to read in data and output to the supplied queue.
		 * @param output_queue Output queue of RTT::Location pointers.
		 * @param output_mutex Mutex for the output queue
		 * @param output_var   Conditition variable for the output queue
		 * @param run         Non null pointer to the boolean variable that is
		 *                     not the signal die
		 */
		void start(std::queue<Location*>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_var);

		/**
		 * Stops the data reading thread.
		 */
		void stop();

		/**
		 * Initializes this GPS Core to take data from the specified path.
		 * @param path Path to data source
		 */
		GPSTest(const std::string path);
		
		/**
		 * Initializes this GPS Core to take data from the specified path.
		 * @param path Path to data source
		 */
		GPSTest(const char* path);

		/**
		 * Sets the output data location
		 * @param path Path to write data to
		 */
		void setOutputFile(const std::string);

	};
}

#endif