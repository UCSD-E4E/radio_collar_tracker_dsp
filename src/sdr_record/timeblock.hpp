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

		/**
		 * Constructs a new TimeBlock at the specified time.  This will have a
		 * default stop time of time + 1.
		 * @param time Time in ms since Unix epoch
		 */
		TimeBlock(uint64_t time);

		/**
		 * Checks whether this object stops before the TimeBlock t starts.
		 * @param t TimeBlock to compare against
		 * @returns true if this object stops before t starts, false otherwise.
		 */
		const bool operator<(const TimeBlock t) const;
	};
}

#endif