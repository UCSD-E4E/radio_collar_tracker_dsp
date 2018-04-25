#ifndef __SDR_H__
#define __SDR_H__

#include <uhd/usrp/multi_usrp.hpp>
#include <string>
#include <queue>
#include <mutex>
namespace RTT{
	class SDR{
		uhd::usrp::multi_usrp::sptr usrp;
		size_t rx_buffer_size = 16384;
		std::string device_args;
		std::string subdev;
		std::string ant;
		std::string ref;
		std::string cpu_format;
		std::string wire_format;
		std::string channel;
		double if_gain;
		long int rx_rate;
		long int rx_freq;
		void streamer();
		std::queue<std::vector<std::complex<short>>*>* output_queue;
		std::mutex* output_mutex;
	public:
		SDR(double gain, long int rate, long int freq);
		~SDR();
		void setBufferSize(size_t buff_size);
		int getBufferSize();
		void startStreaming(std::vector<std::complex<short>>&, std::mutex&);
	};
}

#endif