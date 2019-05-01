#ifndef __ABSTRACT_SDR__
#define __ABSTRACT_SDR__

#include <queue>
#include <mutex>
#include <condition_variable>
#include "iq_data.hpp"

namespace RTT{
	class AbstractSDR{
	public:
		const size_t rx_buffer_size = 16384;
		virtual void startStreaming(std::queue<IQdataPtr>&, std::mutex&, 
			std::condition_variable&) = 0;
		virtual void stopStreaming() = 0;
		virtual ~AbstractSDR() = default;
	};
}
#endif