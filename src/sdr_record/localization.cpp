#include "localization.hpp"
#include <queue>
#include <mutex>
#include <thread>

namespace RTT{
	void PingLocalizer::process(const bool& die){
		while(!die){

		}
	}

	void PingLocalizer::start(std::queue<Ping>& queue, std::mutex& mutex, const bool& die){
		inputQueue = &queue;
		queue_mutex = &mutex;
		localizer_thread = new std::thread(&PingLocalizer::process, this, die);
	}

	PingLocalizer::PingLocalizer(){
		
	}
}