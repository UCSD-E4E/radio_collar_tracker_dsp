#ifndef __SDR_RECORD_H__
#define __SDR_RECORD_H__

#include <complex>
#include <mutex>
#include <queue>
#include <vector>
#include "sdr.hpp"

namespace RTT{
	class SDR_RECORD{

		SDR_RECORD();
		void print_meta_data();
		void print_help();

		struct cmd_args{
			double gain = 0;
			long int rate = 0;
			long int rx_freq = 0;
			int run_num = 0;
			std::string data_dir = "";
		} args;

		static SDR_RECORD* m_pInstance;
		volatile bool program_on = true;
		std::mutex sdr_queue_mutex;
		std::queue<std::vector<std::complex<short>>*> sdr_queue;
		RTT::SDR* sdr;
	protected:
		void receiver();
	public:
		static void sig_handler(int sig);
		static SDR_RECORD* instance();
		void init(int argc, char * const *argv);
		void run();
	};
}

#endif