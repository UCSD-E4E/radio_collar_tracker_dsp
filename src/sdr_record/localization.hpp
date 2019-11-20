#ifndef __RTT_LOCAL_H__
#define __RTT_LOCAL_H__

#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "ping.hpp"
#include "gps.hpp"

namespace RTT{
	
	/**
	 * The PingLocalizer is responsible for localizing pings and writing the 
	 * data to a file, that udp_command.CommandListener then forwards to the 
	 * ground station.
	 */
	class PingLocalizer{

	public:
		/**
		 * Creates the PingLocalizer that writes to the specified output file.
		 * @param out Output file
		 */
		PingLocalizer(std::ostream& out);

		/**
		 * Destructor
		 */
		~PingLocalizer();

		/**
		 * Starts the PingLocalizer threads with the specified input queue for
		 * pings and GPS module.
		 * @param i_q		Input queue of PingPtr objects
		 * @param i_m		Input queue mutex
		 * @param i_v		Input queue condition variable
		 * @param module	GPS Module to retrieve localization information from
		 */
		void start(std::queue<PingPtr>& i_q, std::mutex& i_m, 
			std::condition_variable& i_v, GPS& module);

		/**
		 * Informs the PingLocalizer module to complete processing of queued
		 * pings and exit.
		 */
		void stop();

	protected:
	private:
		/**
		 * Localizes the input pings using data from the gps_module
		 * @param queue      Input queue
		 * @param mutex      Input queue mutex
		 * @param var        Input queue condition variable
		 * @param gps_module GPS data source
		 */
		void process(std::queue<PingPtr>& queue, std::mutex& mutex,
			std::condition_variable& var, GPS& gps_module);

		/**
		 * Internal state variable
		 */
		volatile bool run;

		/**
		 * Thread pointer for process
		 */
		std::thread* localizer_thread;

		/**
		 * handle to condition variable to wake thread
		 */
		std::condition_variable* _input_cv;

		/**
		 * Output filestream
		 */
		std::ostream& _out;

		/**
		 * Secondary queue for recycling pings that arrived before GPS fix.
		 */
		std::queue<PingPtr> pingQueue;
	};
}

#endif
