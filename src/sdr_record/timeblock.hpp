#ifndef __TIMEBLOCK_H__
#define __TIMEBLOCK_H__

#include <cstdint>

namespace RTT{
	/**
	 * Data structure representing a block in time
	 */
	struct TimeBlock{
		/**
		 * Start time in ms since Unix Epoch
		 */
		std::uint64_t start;
		/**
		 * Stop time in ms since Unix Epoch
		 */
		std::uint64_t stop;

		/**
		 * Constructs a new TimeBlock with the specified start and stop times
		 * @param start start time in ms since Unix Epoch
		 * @param stop  stop time in ms since Unix Epoch
		 */
		TimeBlock(uint64_t start, uint64_t stop);

		TimeBlock(uint64_t time);

		const bool operator<(const TimeBlock t) const;
	};
}

#endif