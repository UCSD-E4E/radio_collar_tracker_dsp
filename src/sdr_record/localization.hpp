#ifndef __RTT_LOCAL_H__
#define __RTT_LOCAL_H__

#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>

namespace RTT{
	struct Ping{
		long int northing;
		long int easting;
		double amplitude;
		std::string zone;
	};

	class PingLocalizer{
		void process(const bool& die);
		std::queue<Ping>* inputQueue;
		std::mutex* queue_mutex;
		std::thread* localizer_thread;
	protected:
	public:
		PingLocalizer();
		~PingLocalizer();
		void start(std::queue<Ping>&, std::mutex&, const bool& die);
	};
}

#endif