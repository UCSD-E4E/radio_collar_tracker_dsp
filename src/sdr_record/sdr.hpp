#ifndef __SDR_H__
#define __SDR_H__

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <complex>
#include <uhd.h>

namespace RTT{
	typedef std::complex<short> short_cpx;
	class SDR{
		uhd_usrp_handle usrp;

		size_t rx_buffer_size = 16384;
		std::string device_args;
		std::string subdev;
		std::string ant;
		std::string ref;
		std::string cpu_format;
		std::string wire_format;
		size_t channel;
		double if_gain;
		long int rx_rate;
		long int rx_freq;
		void streamer(const volatile bool* die);
		std::queue<std::vector<std::complex<short>>*>* output_queue;
		std::mutex* output_mutex;
		std::thread* stream_thread;

		uhd_rx_streamer_handle rx_streamer;
	public:
		SDR(double gain, long int rate, long int freq);
		~SDR();
		void setBufferSize(size_t buff_size);
		int getBufferSize();
		void startStreaming(std::queue<std::vector<short_cpx>*>&, std::mutex&, const volatile bool* ndie);
		void stopStreaming();
	};
}

#endif