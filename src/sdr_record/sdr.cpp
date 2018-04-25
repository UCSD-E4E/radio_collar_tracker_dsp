#include <iostream>
#include "sdr.hpp"
#include <boost/format.hpp>
#include <uhd/types/tune_request.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/thread_priority.hpp>
#include <stdexcept>
#include <syslog.h>
#include <complex>

namespace RTT{
	SDR::SDR(double gain, long int rate, long int freq) : device_args(""), subdev("A:A"), ant("RX2"), ref("internal"), cpu_format("sc16"), wire_format("sc16"), channel("0"), if_gain(gain), rx_rate(rate), rx_freq(freq){
		uhd::set_thread_priority_safe();
		//create a usrp device
		syslog(LOG_DEBUG, "Creating USRP with args \"%s\"...\n", device_args.c_str());
		usrp = uhd::usrp::multi_usrp::make(device_args);

		// Lock mboard clocks
		syslog(LOG_DEBUG, "Lock mboard clocks: %s\n", ref.c_str());
		usrp->set_clock_source(ref);
		
		//always select the subdevice first, the channel mapping affects the other settings
		syslog(LOG_DEBUG, "subdev set to: %s\n", subdev.c_str());
		usrp->set_rx_subdev_spec(subdev);
		syslog(LOG_DEBUG, "Using Device: %s\n", usrp->get_pp_string().c_str());

		//set the sample rate
		if (rate <= 0.0) {
			syslog(LOG_ERR, "Please specify a valid sample rate\n");
			throw std::invalid_argument("invalid sample rate");
		}

		// set sample rate
		syslog(LOG_DEBUG, "Setting RX Rate: %f Msps...\n", (rate / 1e6));
		usrp->set_rx_rate(rate);
		syslog(LOG_DEBUG, "Actual RX Rate: %f Msps...\n", (usrp->get_rx_rate() / 1e6));

		// set freq
		syslog(LOG_DEBUG, "Setting RX Freq: %f MHz...\n", (freq / 1e6));
		uhd::tune_request_t tune_request(freq);
		usrp->set_rx_freq(tune_request);
		syslog(LOG_DEBUG, "Actual RX Freq: %f MHz...\n", (usrp->get_rx_freq() / 1e6));

		// set the rf gain
		syslog(LOG_DEBUG, "Setting RX Gain: %f dB...\n", gain);
		usrp->set_rx_gain(gain);
		syslog(LOG_DEBUG, "Actual RX Gain: %f dB...\n", usrp->get_rx_gain());

		// set the IF filter bandwidth
		syslog(LOG_DEBUG, "Setting RX Bandwidth: %f MHz...\n", (rate / 1e6));
		usrp->set_rx_bandwidth(rate);
		syslog(LOG_DEBUG, "Actual RX Bandwidth: %f MHz...\n", (usrp->get_rx_bandwidth() / 1e6));

		// set the antenna
		syslog(LOG_DEBUG, "Setting RX Antenna: %s\n", ant.c_str());
		usrp->set_rx_antenna(ant);
		syslog(LOG_DEBUG, "Actual RX Antenna: %s\n", usrp->get_rx_antenna().c_str());

	}

	void SDR::startStreaming(std::vector<std::complex<short>>& queue, std::mutex& mutex){
		output_queue = &queue;
		
	}

	void SDR::streamer(){
		uhd::stream_args_t stream_args(cpu_format, wire_format);
		std::vector<size_t> channel_nums;
		channel_nums.push_back(0);
		stream_args.channels = channel_nums;
		uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
		
		uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
		stream_cmd.stream_now = true;
		rx_stream->issue_stream_cmd(stream_cmd);
		uhd::rx_metadata_t md;
		std::vector<std::complex<short>> buff(rx_buffer_size);
	}

	SDR::~SDR(){
	}
}