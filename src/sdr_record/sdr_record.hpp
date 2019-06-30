#ifndef __SDR_RECORD_H__
#define __SDR_RECORD_H__

#include <complex>
#include <mutex>
#include <queue>
#include <vector>
#ifdef TEST_SDR
#include "sdr_test.hpp"
#else
#include "sdr.hpp"
#endif
#include "dsp.hpp"
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
			double gain = -1;
			std::size_t rate = 0;
			std::size_t rx_freq = 0;
			std::size_t run_num = 0;
			std::string data_dir = "";
			bool test_config = false;
			std::string test_data = "";
			std::string gps_target = "";
		} args;

		static SDR_RECORD* m_pInstance;
		volatile bool program_on = true;

		std::queue<std::complex<double>*> sdr_queue;
		std::mutex sdr_queue_mutex;
		std::condition_variable sdr_var;

		std::queue<PingPtr> ping_queue;
		std::mutex ping_queue_mutex;
		std::condition_variable ping_var;

		RTT::AbstractSDR* sdr;
		RTT::DSP* dsp;
		RTT::PingLocalizer* localizer;
		RTT::GPS* gps;

		std::condition_variable run_var;
		std::mutex run_mutex;

		std::ofstream* _estimate_str;
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