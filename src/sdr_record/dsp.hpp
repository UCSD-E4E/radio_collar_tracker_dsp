#ifndef __RTT_DSP_H__
#define __RTT_DSP_H__

#include <queue>
#include "iq_data.hpp"
#include <mutex>
#include <condition_variable>
#include "ping.hpp"

namespace RTT{


	/**
	 * DSP Class
	 */
	class DSP{
	public:

		virtual ~DSP(){
			
		}

		/**
		 * @param	inputQueue	a std::queue<IQdata> reference for incoming 
		 *                   	data
		 * @param	inputMutex	a std::mutex for the input queue
		 * @param	inputVar	a std::condition_variable for the input queue
		 * @param	outputQueue	a std::queue<RTT::Ping*> for detected pings
		 * @param	outputMutex	a std::mutex for the output queue
		 * @param	outputVar	a std::condition_variable for the output queue
		 * @param	ndie		a pointer to a bool that is true when the 
		 * 						program should be running, and false when the 
		 * 						program should be shutting down
		 *
		 * This should be a non-blocking function that starts a thread which 
		 * processes the incoming data and detects the pings in that data, and 
		 * sends it along out the outputQueue queue.
		 */
		virtual void startProcessing(
			std::queue<std::complex<double>*>& inputQueue, 
			std::mutex& inputMutex, std::condition_variable& inputVar,
			std::queue<PingPtr>& outputQueue, std::mutex& outputMutex, 
			std::condition_variable& outputVar) = 0;

		/**
		 * Waits for the processing thread to stop.
		 */
		virtual void stopProcessing() = 0;

		virtual void setStartTime(std::size_t) = 0;
		virtual void setOutputDir(const std::string&, const std::string&) = 0;
	};
}

#endif