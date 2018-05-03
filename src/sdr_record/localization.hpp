#ifndef __RTT_LOCAL_H__
#define __RTT_LOCAL_H__

#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>

namespace RTT{
	/**
	 * Data structure representing a single ping detection.
	 */
	struct Ping{

		/**
		 * Convenience constructor
		 *
		 * @param	time_ms		time in ms of ping w.r.t. local system clock
		 * @param	amplitude	amplitude of ping in dB (reference not important)
		 */
		Ping(long long int time_ms, double amplitude) : time_ms{time_ms}, amplitude{amplitude}{};

		long long int time_ms;
		double amplitude;
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