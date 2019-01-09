#ifndef __SDR_TEST_H__
#define __SDR_TEST_H__

#include "iq_data.hpp"
// #include "sdr.hpp"
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <fstream>
#include <thread>
namespace RTT{
	class SDR_TEST{
	private:
		std::string _input_dir;
		std::fstream _stream;
		std::vector<std::string> _files;
		std::size_t _buffer_size;

		std::thread* _thread;
		std::condition_variable* _input_cv;

		std::size_t _sampling_freq;
		
		void _process(std::queue<IQdataPtr>&, std::mutex&, 
			std::condition_variable&, const volatile bool* ndie);

	public:
		const size_t rx_buffer_size = 16384;
		SDR_TEST(std::string input_dir);
		~SDR_TEST();

		void setBufferSize(size_t buff_size);
		int getBufferSize();

		void startStreaming(std::queue<IQdataPtr>&, std::mutex&, 
			std::condition_variable&, const volatile bool* ndie);
		void stopStreaming();
	};
}

#endif
