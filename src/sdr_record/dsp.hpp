#ifndef __RTT_DSP_H__
#define __RTT_DSP_H__

#include <complex>
#include <queue>
#include <vector>
#include <mutex>
#include "localization.hpp"

namespace RTT{
	/**
	 * 
	 */
	class DSP{
	public:
		
		/**
		 * @param	inputQueue	a std::queue<std::vector<std::complex<short>>*>
		 * 							pointer for incoming data
		 * @param	inputMutex	a std::mutex pointer for the input queue
		 * @param	outputQueue	a std::queue<RTT::Ping*> pointer for detected
		 * 							pings
		 * @param	outputMutex	a std::mutex pointer for the output queue
		 * @param	ndie			a pointer to a bool that is true when the 
		 * 							program should be running, and false when 
		 * 							the program should be shutting down
		 *
		 * This should be a non-blocking function that starts a thread which 
		 * processes the incoming data and detects the pings in that data, and 
		 * sends it along out the outputQueue queue.
		 */
		virtual void startProcessing(
			std::queue<std::vector<std::complex<short>>*>& inputQueue, 
			std::mutex& inputMutex, std::queue<Ping*>& outputQueue, 
			std::mutex& outputMutex, const volatile bool* ndie){}

		/**
		 * Waits for the processing thread to stop.
		 */
		virtual void stopProcessing(){}
	};
}

#endif