/**
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
 * Copyright (C) 2012 by Steve Markgraf <steve@steve-m.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <signal.h>
#include <rtl-sdr.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

#define SAMP_RATE 2048000
#define CENTR_FREQ 172464000

static int do_exit = 0;
static rtlsdr_dev_t *dev = NULL;

static void sighandler(int signum){
	fprintf(stderr, "Signal caught, exiting!\n");
	do_exit = 1;
	rtlsdr_cancel_async(dev);
}

int main(int argc, char **argv){

	// Setting up variables
	struct sigaction sigact;
	int gain = 0;
	int opt;
	FILE* file;

	// Getting command line options
	while ((opt = getopt(argc, argv, "g")) != -1){
		switch(opt){
			case 'g':
				gain = (int)(atof(optarg) * 10);
				break;
		}
	}

	// Opening SDR
	if(rtlsdr_open(&dev, 0)){
		fprintf(stderr, "Failed to open rtlsdr device!");
		exit(1);
	}

	// Setting signal handlers
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);

	// Setting sample rate
	if(rtlsdr_set_sample_rate(dev, SAMP_RATE)){
		fprintf(stderr, "WARNING: Failed to set sample rate.\n");
		exit(1);
	}
	// Setting center frequency
	if(rtlsdr_set_center_freq(dev, CENTR_FREQ)){
		fprintf(stderr, "WARNING: Failed to set center freq.\n");
		exit(1);
	}

	// Setting gain
	if(gain == 0){
		if(rtlsdr_set_tuner_gain_mode(dev, 0)){
			fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
			exit(1);
		}
	}else{
		if(rtlsdr_set_tuner_gain_mode(dev, 1)){
			fprintf(stderr, "WARNING: Failed to enable manual gain.\n");
			exit(1);
		}
		if(rtlsdr_set_tuner_gain(dev, gain)){
			fprintf(stderr, "WARNING: Failed to set tuner gain.\n");
			exit(1);
		}
	}

	// Setting output filename
	// FIXME: Need smarter filename
	file = fopen("tmp", "wb");
	if(!file){
		fprintf(stderr, "Failed to open file!\n");
	}

	// Reset endpoint before we start reading from it (mandatory)
	if(rtlsdr_reset_buffer(dev)){
		fprintf(stderr, "WARNING: Failed to reset buffers\n");
		exit(1);
	}

	// FIXME: Add timestamp here!
	// Read samples in async mode
	int r = rtlsdr_read_async(dev, rtlsdr_callback, (void*) file, 0, out_block_size);
	/*
	 * TODO: Set up callback to copy frame to worker thread's memory space, 
	 * have worker send data to file.
	 */

	fclose(file);
	rtlsdr_close(dev);
}
