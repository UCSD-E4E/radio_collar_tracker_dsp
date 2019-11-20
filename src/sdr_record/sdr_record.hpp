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
#include "gps.hpp"
#include <condition_variable>
#include "localization.hpp"
#include <boost/program_options.hpp>

namespace RTT{
	/**
	 * Initial entry class.  This class is responsible for configuring and
	 * owning the component classes.
	 *
	 * This program will record and analyze RF signals from a software defined
	 * radio for pings.  These pings will be written out to a specified location.
	 */
	class SDR_RECORD{
		/**
		 * Constructor.  This configures the signal handler and system logger.
		 */
		SDR_RECORD();

		/**
		 * Destructor.  This will close out any output streams and deinitialize
		 * allocated resources.
		 */
		~SDR_RECORD();

		/**
		 * Prints metadata about this configuration to the output directory's
		 * meta file.  This will always go in [output_dir]/META_[run_num].
		 * This information shall include start time in seconds since Unix
		 * epoch, center frequency in Hz, sampling frequency in Hz, LNA gain in 
		 * dB, data format (bytes per real sample), target frequencies, ping
		 * width in ms, ping width multipliers, GPS configuration, and test
		 * configuration.
		 */
		void print_meta_data();

		/**
		 * Parses the argument vector provided by the system as well as the
		 * configuration file.  Arguments shall adhere to the 
		 * boost::program_options conventions.
		 * @param argc Argument Count
		 * @param argv Argument Vector
		 */
		void process_args(int argc, char* const *argv);

		/**
		 * Struct to contain all configuration parameters.
		 */
		struct cmd_args{
			/**
			 * LNA gain in dB
			 */
			double gain = -1;
			/**
			 * SDR sampling rate in Hz
			 */
			std::size_t rate = 0;
			/**
			 * SDR center frequency in Hz
			 */
			std::size_t rx_freq = 0;
			/**
			 * Run identifier.  This must be unique!
			 */
			std::size_t run_num = 0;
			/**
			 * Output directory.  Output files will be stored directly in this
			 * path (i.e. data_dir/META_[run_num])
			 */
			std::string data_dir = "";
			/**
			 * Test configuration flag.  If set, this will configure the system
			 * to run as a SIL testbench.  FIXME.DOC!!!
			 */
			bool test_config = false;
			/**
			 * Path to pre-recorded data when used as a SIL testbench.  This
			 * must be generated with the same version of sdr_record, or the
			 * metadata may not be parsed correctly!
			 */
			std::string test_data = "";
			/**
			 * GPS target path.  For the serial GPS module, specify the Unix
			 * device handle.  For the TEST_GPS module, specify the datafile
			 * to use as a datasource.
			 */
			std::string gps_target = "";
			/**
			 * GPS baud rate.  Only used with the serial GPS module.
			 */
			std::size_t gps_baud = 9600;
			/**
			 * Nominal ping width in ms.
			 */
			std::size_t ping_width_ms = 36;
			/**
			 * Minimum ping SNR in dB above noise floor.
			 */
			double ping_min_snr = 4;
			/**
			 * Upper threshold multiplier for ping width
			 */
			double ping_max_len_mult = 1.5;
			/**
			 * Lower threshold multiplier for ping width
			 */
			double ping_min_len_mult = 0.75;
			/**
			 * Test GPS mode - this forces the TEST_NULL gps module if the
			 * test_config flag is not set.
			 */
			bool gps_mode = false;
			/**
			 * Target frequencies in Hz
			 */
			std::vector<int> frequencies;
		} args;

		/**
		 * Singleton instance pointer
		 */
		static SDR_RECORD* m_pInstance;

		/**
		 * Internal state variable
		 */
		volatile bool program_on = true;

		/**
		 * SDR data queue.  This connects the RTT::SDR and RTT::DSP_V3 classes.
		 */
		std::queue<std::complex<double>*> sdr_queue;
		/**
		 * RTT::SDR_RECORD::sdr_queue mutex
		 */
		std::mutex sdr_queue_mutex;
		/**
		 * sRTT::SDR_RECORD::dr_queue condition variable
		 */
		std::condition_variable sdr_var;

		/**
		 * Ping queue.  This connects the RTT::DSP_V3 and RTT::PingLocalizer 
		 * classes.
		 */
		std::queue<PingPtr> ping_queue;
		/**
		 * RTT::SDR_RECORD::ping_queue mutex
		 */
		std::mutex ping_queue_mutex;
		/**
		 * RTT::SDR_RECORD::ping_queue condition variable
		 */
		std::condition_variable ping_var;

		/**
		 * Software Defined Radio handle
		 */
		RTT::AbstractSDR* sdr;
		/**
		 * Digital Signal Processing stage
		 */
		RTT::DSP* dsp;
		/**
		 * Ping Localizer stage
		 */
		RTT::PingLocalizer* localizer;
		/**
		 * GPS Module
		 */
		RTT::GPS* gps;

		/**
		 * Synchronization and wake variable - this signals the main run loop
		 * when to start shutting down the component modules.
		 */
		std::condition_variable run_var;
		/**
		 * Synchronization and wake mutex for run loop
		 */
		std::mutex run_mutex;

		/**
		 * Output stream location - this is where PingLocalizer will output to.
		 */
		std::ofstream* _estimate_str;

		/**
		 * Variables map for program options.
		 */
		boost::program_options::variables_map vm;
	public:
		/**
		 * Signal handler for SIGINT. Responsible for notifying the program
		 * to begin terminating.
		 * @param sig Signal ID
		 */
		static void sig_handler(int sig);

		/**
		 * Singleton instance.
		 * @return Singleton instance of SDR_RECORD
		 */
		static SDR_RECORD* instance();
		/**
		 * Initializes this singleton with the provided command line arguments.
		 * @param argc Argument count
		 * @param argv Argument vector
		 */
		void init(int argc, char * const *argv);
		/**
		 * Start the processing.  This will not exit until all threads have
		 * completed, which will only happen after SIGINT is raised.
		 */
		void run();
	};
}

#endif