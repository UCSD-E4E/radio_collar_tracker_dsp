#ifndef __PING_H__
#define __PING_H__

#include <memory>

namespace RTT{
	/**
	 * Data structure representing a single ping detection.
	 */
	struct Ping{

		/**
		 * Convenience constructor
		 *
		 * @param	time_ms		time in ms of ping according to local system clock
		 * @param	amplitude	amplitude of ping in dB (reference not important)
		 */
		Ping(std::uint64_t time_ms, double amplitude) : time_ms{time_ms}, amplitude{amplitude}{};

		/**
		 * Timestamp of ping in ms according to local system clock
		 */
		std::uint64_t time_ms;

		/**
		 * Relative amplitude of ping in dB
		 */
		double amplitude;
	};
	
	typedef std::shared_ptr<Ping> PingPtr;
}

#endif