#include "timeblock.hpp"

namespace RTT{
	TimeBlock::TimeBlock(uint64_t start, uint64_t stop) : start(start), 
			stop(stop){

	}

	const bool TimeBlock::operator<(const TimeBlock t) const{
		return stop < t.start;
	}

	TimeBlock::TimeBlock(uint64_t time) : start(time), stop(time + 1){
	}
}