#ifndef __GPS_CORE_H__
#define __GPS_CORE_H__

#include <queue>
#include <timeblock.hpp>
#include <location.hpp>
#include <mutex>
#include <condition_variable>

namespace RTT{
	class GPSCore{
	private:
	protected:
	public:

		/**
		 * Starts the thread to read in data and output to the supplied queue.
		 * @param output_queue Output queue of RTT::Location pointers.
		 * @param output_mutex Mutex for the output queue
		 * @param output_var   Conditition variable for the output queue
		 */
		virtual void start(std::queue<Location*>& output_queue, 
			std::mutex& output_mutex, std::condition_variable& output_var) = 0;

		/**
		 * Stops the data reading thread.
		 */
		virtual void stop() = 0;

		/**
		 * Sets the output data location
		 * @param path Path to write data to
		 */
		virtual void setOutputFile(const std::string path) = 0;

	};
}

#endif