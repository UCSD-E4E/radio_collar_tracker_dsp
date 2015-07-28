/**
 *
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
#include <rtl_sdr.h>
#include <stdio.h>
#include <unistd.h>

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
	int gain = 0;
	uint8_t* buffer;
	while ((opt = getopt(argc, argv, "g")) != -1){
		switch(opt){
			case 'g':
				gain = (int)(atof(optarg) * 10);
				break;
		}
	}

	buffer = malloc(out_block_size * sizeof(uint8_t));

	if(rtlsdr_open(&dev, 0)){
		fprintf(stderr, "Failed to open rtlsdr device!");
		exit(1);
	}

	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);

	if(rtlsdr_set_sample_rate(dev, SAMP_RATE)){
		fprintf(stderr, "WARNING: Failed to set sample rate.\n");
	}
	if(rtlsdr_set_center_freq(dev, CENTR_FREQ)){
		fprintf(stderr, "WARNING: Failed to set center freq.\n");
	}
}
