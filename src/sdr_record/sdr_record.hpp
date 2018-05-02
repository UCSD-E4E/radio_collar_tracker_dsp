#ifndef __SDR_RECORD_H__
#define __SDR_RECORD_H__

#include <complex>
#include <mutex>
#include <queue>
#include <vector>
#include "sdr.hpp"
#include "dsp.hpp"
#include "dspv1.hpp"
#include "localization.hpp"

namespace RTT{
	class SDR_RECORD{

		SDR_RECORD();
		void print_meta_data();
		void print_help();
		void process_args(int argc, char* const *argv);

		struct cmd_args{
			double gain = 0;
			long int rate = 0;
			long int rx_freq = 0;
			int run_num = 0;
			std::string data_dir = "";
		} args;

		static SDR_RECORD* m_pInstance;
		volatile bool program_on = true;

		std::queue<std::vector<std::complex<short>>*> sdr_queue;
		std::mutex sdr_queue_mutex;

		std::queue<Ping*> ping_queue;
		std::mutex ping_queue_mutex;

		RTT::SDR* sdr;
		RTT::DSP* dsp;
		RTT::PingLocalizer* localizer;
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