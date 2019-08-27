#ifndef __ABSTRACT_SDR__
#define __ABSTRACT_SDR__

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "iq_data.hpp"

namespace RTT{
	class AbstractSDR{
	public:
		const static size_t rx_buffer_size = 2048;
		virtual void startStreaming(std::queue<std::complex<double>*>&, std::mutex&, 
			std::condition_variable&) = 0;
		virtual void stopStreaming() = 0;
		virtual ~AbstractSDR() = default;
		virtual const size_t getStartTime_ms() const = 0;
	};
}
#endif
