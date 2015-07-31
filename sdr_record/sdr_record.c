/**
 * This program records broadband signal data from an RTL2832 based SDR to
 * assist detection of radio collars.
 * Copyright (C) 2015 by Nathan Hui <nthui@eng.ucsd.edu>
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtl-sdr.h>
#include <signal.h>
#include <pthread.h>
#include <sys/queue.h>
#include <time.h>
#include <stdbool.h>
#include "queue.h"

// Global constants
#define FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS	500

// Typedefs
struct proc_queue_args{
	int run_num;
	int frame_len;
};

// Global variables
rtlsdr_dev_t *dev = NULL;
int run = 1;
pthread_mutex_t lock;
queue data_queue;
int counter = 0;

// Function Prototypes
void sighandler(int signal);
int main(int argc, char** argv);
void* proc_queue(void* args);
static void rtlsdr_callback(unsigned char* buf, uint32_t len, void *ctx);

/**
 * Main function of this program.
 */
int main(int argc, char** argv){
	// Set up varaibles
	// Options variable
	int opt;
	// SDR Gain
	int gain = 0;
	// Sampling Frequency for SDR in samples per second
	int samp_freq = 2048000;
	// Center Frequency for SDR in Hz
	int center_freq = 172464000;
	// Nominal pulse length in ms
	int pulse_length = 0;
	// Nominal pulse period in m
	int pulse_per = 1500;
	// Run number
	int run_num = -1;
	// Block size
	int block_size = 0;
	// Start Time
	struct timespec start_time;
	// End Time
	struct timespec end_time;
	// Thread attributes
	pthread_attr_t attr;
	// Thread ID
	pthread_t thread_id;
	// proc_queue args
	struct proc_queue_args pargs;

	// Get command line options
	printf("Getting command line options\n");
	while ((opt = getopt(argc, argv, "g:s:f:p:r:P:")) != -1){
		switch(opt){
			case 'g':
				gain = (int)(atof(optarg) * 10);
				break;
			case 's':
				samp_freq = (int)(atof(optarg));
				break;
			case 'f':
				center_freq = (int)(atof(optarg));
				break;
			case 'p':
				pulse_length = (int)(atof(optarg));
				break;
			case 'r':
				run_num = (int)(atof(optarg));
				break;
			case 'P':
				pulse_per = (int)(atof(pulse_per));
				break;
		}
	}
	if(run_num == -1){
		// TODO: add usage notification here!
		fprintf(stderr, "Bad RUN number!\n");
		exit(-1);
	}
	printf("done\n");

	// Initialize environment
	printf("Configuring environment\n");
	block_size = (int)(pulse_per / 1000.0 * 2 * samp_freq);
	block_size = 262144;
	queue_init(&data_queue);
	printf("Done configuring environment\n");

	// Open SDR
	printf("Opening SDR\n");
	if(rtlsdr_open(&dev, 0)){
		fprintf(stderr, "Failed to open rtlsdr device\n");
		exit(1);
	}
	printf("SDR Opened\n");

	// Configure SDR
	printf("Configuring SDR\n");
	if(gain == 0){
		if(rtlsdr_set_tuner_gain_mode(dev, 0)){
			fprintf(stderr, "ERROR: Failed to set tuner gain.\n");
			exit(1);
		}
	}else{
		if(rtlsdr_set_tuner_gain_mode(dev, 1)){
			fprintf(stderr, "ERROR: Failed to enable manual gain.\n");
			exit(1);
		}
		if(rtlsdr_set_tuner_gain(dev, gain)){
			fprintf(stderr, "ERROR: Failed to set tuner gain.\n");
			exit(1);
		}
	}
	if(rtlsdr_set_center_freq(dev, center_freq)){
		fprintf(stderr, "ERROR: Failed to set center frequency.\n");
		exit(1);
	}
	if(rtlsdr_set_sample_rate(dev, samp_freq)){
		fprintf(stderr, "ERROR: Failed to set sampling frequency.\n");
		exit(1);
	}
	if(rtlsdr_reset_buffer(dev)){
		fprintf(stderr, "ERROR: Failed to reset buffers.\n");
		exit(1);
	}
	printf("SDR Configured.\n");

	// Configure signal handlers
	printf("Configuring signal handlers.\n");
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	printf("Signal handlers configured\n");

	// Configure worker thread
	printf("Configuring worker thread\n");
	if(pthread_attr_init(&attr)){
		fprintf(stderr, "ERROR: Failed to create default thread attributes.\n");
		exit(1);
	}
	if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)){
		fprintf(stderr, "ERROR: Failed to create detached thread.\n");
		exit(1);
	}
	pargs.run_num = run_num;
	pargs.frame_len = block_size;
	if(pthread_create(&thread_id, &attr, proc_queue, (void*) &pargs)){
		fprintf(stderr, "ERROR: Failed to create detached thread.\n");
		exit(1);
	}
	printf("Worker thread configured\n");

	// Get timestamp
	printf("Starting record\n");
	clock_gettime(CLOCK_REALTIME, &start_time);
	// begin recording
	rtlsdr_read_async(dev, rtlsdr_callback, (void*) &data_queue, 0, block_size);
	// clean up
	clock_gettime(CLOCK_REALTIME, &end_time);
	printf("Stopping record\n");
	rtlsdr_close(dev);
}

/**
 * Signal handler for this program.  Cancels the asynchronous read for the SDR
 * and notifies all threads to finish execution.
 */
void sighandler(int signal){
	printf("Signal caught, exiting\n");
	run = 0;
	rtlsdr_cancel_async(dev);
}

/**
 * Worker thread function.  Reads information from the queue and writes it to
 * the disk.
 */
void* proc_queue(void* args){
	struct proc_queue_args* pargs = (struct proc_queue_args*) args;
	int run_num = pargs->run_num;
	int frame_len = pargs->frame_len;
	FILE* data_stream;
	char buff[256];
	int frame_num;

	frame_num = 0;
	// Lock mutex
	pthread_mutex_lock(&lock);
	bool empty = true;
	empty = queue_isEmpty(&data_queue);
	pthread_mutex_unlock(&lock);

	printf("Running thread\n");

	while(run || !empty){
		printf("Checking queue\n");
		pthread_mutex_lock(&lock);
		empty = queue_isEmpty(&data_queue);
		pthread_mutex_unlock(&lock);

		if(!empty){
			// Process queue
			printf("Got frame\n");
			snprintf(buff, sizeof(buff),
					"RAW_DATA_%06d_%06d", run_num,
					frame_num %4 + 1);
			data_stream = fopen(buff, "ab");
			pthread_mutex_lock(&lock);
			char* data_ptr = NULL;
			data_ptr = (char*) queue_pop(&data_queue);
			pthread_mutex_unlock(&lock);

			printf("Writing frame\n");
			fwrite(data_ptr, sizeof(char), frame_len, data_stream);
			fclose(data_stream);

			free(data_ptr);
			frame_num++;
			printf("done\n");
		}else{
			printf("No frame\n");
			usleep(FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS * 1000);
		}
	}
	return NULL;
}

static void rtlsdr_callback(unsigned char* buf, uint32_t len, void *ctx){
	counter++;
	if(counter > 100){
		sighandler(0);
	}
	if(!run){
		return;
	}
	char* newframe = malloc(len * sizeof(char));
	memcpy(newframe, buf, len);
	pthread_mutex_lock(&lock);
	queue_push((queue*)ctx, (void*) newframe);
	pthread_mutex_unlock(&lock);
}
