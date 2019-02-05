#ifndef __SDR_RECORD_H__
#define __SDR_RECORD_H__

#include <complex>
#include <mutex>
#include <queue>
#include <vector>
// #include "sdr.hpp"
#include "sdr_test.hpp"
#include "dsp.hpp"
#include "dspv2.hpp"
#include "localization.hpp"
#include <condition_variable>

namespace RTT{
	class SDR_RECORD{

		SDR_RECORD();
		~SDR_RECORD();
		void print_meta_data();
		void print_help();
		void process_args(int argc, char* const *argv);

		struct cmd_args{
			double gain = 10.0;
			long int rate = 2000000;
			long int rx_freq = 172500000;
			long int tx_freq = 173763000;
			int run_num = 1;
			std::string data_dir = "";
		} args;

		static SDR_RECORD* m_pInstance;
		volatile bool program_on = true;

		std::queue<IQdataPtr> sdr_queue;
		std::mutex sdr_queue_mutex;
		std::condition_variable sdr_var;

		std::queue<PingPtr> ping_queue;
		std::mutex ping_queue_mutex;
		std::condition_variable ping_var;

		RTT::SDR_TEST* sdr;
		RTT::DSP* dsp;
		RTT::PingLocalizer* localizer;

		std::condition_variable run_var;
		std::mutex run_mutex;
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