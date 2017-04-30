/*
 * @file rct_sdr.c
 *
 * @author Jacob W Torres, jaketorres00@gmail.com
 * 
 * @description 
 *
 * This app provides a software interface to the
 * USRP B200mini software defined radio. Interfaces to USRP
 * Hardware Driver (UHD) library. Through the UHD layer, this
 * application pulls from the USRP over USB 3.0 and pushes onto
 * a custom FIFO optimized for this app. A second thread pulls
 * from the FIFO and writes the raw data the disk at any specifed
 * file location. 
 *
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <uhd.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sdr_record.h"
#include "queue.h"
#include <syslog.h>
#include <time.h>

/////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////


#define TIMEOUT_SEC		        3.0
#define META_PREFIX             "META_"

#define DEFAULT_RATE            -1
#define DEFAULT_FREQ            -1
#define DEFAULT_GAIN            -1
#define DEFAULT_RUN             -1

#define FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS	20
#define FRAMES_PER_FILE 					1024
#define QUEUE_WARNING_LENGTH				15
#define QUEUE_CRITICAL_LENGTH				63


/////////////////////////////////////////////////////////
// Function Declarations
/////////////////////////////////////////////////////////
void    radio_deinit        (void);
int     radio_init          (void);
void    print_help          (void);
void    print_meta_data     (void);
void    sig_handler         (int sig);
void *  stream_push_thread  (void* args);
void *  queue_pop_thread    (void* args);
int     is_rx_error         (uhd_rx_metadata_error_code_t error_code);

/////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////

double freq = DEFAULT_FREQ;
double rate = DEFAULT_RATE;
double gain = DEFAULT_GAIN;
int run_num = DEFAULT_RUN;
volatile int waiting_for_kick = 0;
volatile int program_on = 1;

char error_string[512];
char DATA_DIR[100];
size_t samps_per_buff;
size_t channel = 0;

float *buff = NULL;
char *device_args = "";
void **buffs_ptr = NULL;
FILE *fp = NULL;

queue_t data_queue;

uhd_usrp_handle usrp;
uhd_rx_metadata_handle md;
uhd_rx_streamer_handle rx_streamer;
uhd_tune_result_t tune_result;

uhd_stream_cmd_t stream_cmd = {
	.stream_mode = UHD_STREAM_MODE_START_CONTINUOUS,
	.stream_now = true
};

uhd_tune_request_t tune_request = {
	.target_freq = 500e6,
	.rf_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
	.dsp_freq_policy = UHD_TUNE_REQUEST_POLICY_AUTO,
};

uhd_stream_args_t stream_args = {
	.cpu_format = "fc32",
	.otw_format = "sc16",
	.args = "",
	.channel_list = &channel,
	.n_channels = 1
};

uint8_t push_complete = 0;
pthread_mutex_t thread_compete_mutex;
pthread_cond_t thread_complete;


/////////////////////////////////////////////////////////
// Function Definitions
/////////////////////////////////////////////////////////

void print_help(void){
	printf("sdr_record - Radio Collar Tracker drone application to pull IQ samples from USRP and dump to disk\n\n"
			"Options:\n"
			"    -r (run_number)\n"
			"    -f (center frequency in Hz)\n"
			"    -s (sample rate in Hz)\n"
			"    -g (gain)\n"
			"    -o (output directory)\n"
			"    -h (print this help message)\n");

	exit(0);

}

int radio_init(void){
	double temp_param;

	/* Init USRP object */
	syslog(LOG_DEBUG, "Creating USRP with args \"%s\"...\n", device_args);
	uhd_usrp_make(&usrp, device_args);
	if(!usrp){
		syslog(LOG_ERR, "Unable to create device!");
		return 1;
	}

	/* Create RX streamer */
	uhd_rx_streamer_make(&rx_streamer);

	/* Create RX metadata */
	uhd_rx_metadata_make(&md);

	/* Set rate */
	temp_param = rate;
	syslog(LOG_DEBUG, "Setting RX Rate: %f...\n", rate);
	uhd_usrp_set_rx_rate(usrp, rate, channel);

	/* See what rate actually is */
	uhd_usrp_get_rx_rate(usrp, channel, &rate);
	syslog(LOG_INFO, "Actual RX Rate: %f...\n", rate);

	if (temp_param != rate){
		syslog(LOG_WARNING, "WARNING: RX rate not correctly set\n");
	}


	/* Set gain */
	temp_param = gain;
	syslog(LOG_DEBUG, "Setting RX Gain: %f dB...\n", gain);
	uhd_usrp_set_rx_gain(usrp, gain, channel, ""); 

	/* See what gain actually is */
	uhd_usrp_get_rx_gain(usrp, channel, "", &gain);
	syslog(LOG_INFO, "Actual RX Gain: %f...\n", gain);

	if (temp_param != gain){
		syslog(LOG_WARNING, "WARNING: RX gain not correctly set\n");
	}

	/*Set frequency*/
	temp_param = tune_request.target_freq;
	syslog(LOG_DEBUG, "Setting RX frequency: %f MHz...\n", tune_request.target_freq / 1e6);
	uhd_usrp_set_rx_freq(usrp, &tune_request, channel, &tune_result);

	/*See what frequency actually is*/
	uhd_usrp_get_rx_freq(usrp, channel, &freq);
	syslog(LOG_INFO, "Actual RX frequency: %f MHz...\n", freq / 1e6);

	if ((freq - temp_param) / temp_param * 1e9 > 5){
		syslog(LOG_WARNING, "%f \t %f\n", temp_param, freq);
		syslog(LOG_WARNING, "WARNING: RX freq not correctly set, %.2f ppb offset\n", (freq - temp_param) / temp_param * 1e9);
	}


	/*Set up streamer*/
	stream_args.channel_list = &channel;
	uhd_usrp_get_rx_stream(usrp, &stream_args, rx_streamer);

	/*Set up buffer*/
	uhd_rx_streamer_max_num_samps(rx_streamer, &samps_per_buff);
	syslog(LOG_INFO, "Buffer size in samples: %zu\n", samps_per_buff);

	return 0;
}

void print_meta_data(void){

	struct timespec start_time;
	char buf[256];
	FILE* timing_stream;

	clock_gettime(CLOCK_REALTIME, &start_time);

	snprintf(buf, sizeof(buf), "%s/%s%06d", DATA_DIR, META_PREFIX, run_num);
	timing_stream = fopen(buf, "w");

	fprintf(timing_stream, "start_time: %f\n", start_time.tv_sec + (float)start_time.tv_nsec / 1.e9);
	fprintf(timing_stream, "center_freq: %lf\n", tune_request.target_freq);
	fprintf(timing_stream, "sampling_freq: %lf\n", rate);
	fprintf(timing_stream, "gain: %f\n", gain / 10.0);

	fclose(timing_stream);
}

void radio_deinit(void){

	syslog(LOG_DEBUG, "Cleaning up RX streamer.\n");
	uhd_rx_streamer_free(&rx_streamer);

	syslog(LOG_DEBUG, "Cleaning up RX metadata.\n");
	uhd_rx_metadata_free(&md);

	syslog(LOG_DEBUG, "Cleaning up USRP.\n");
	if(usrp != NULL){
		uhd_usrp_last_error(usrp, error_string, 512);
		syslog(LOG_NOTICE, "USRP reported the following error(s): %s\n", error_string);
	}
	uhd_usrp_free(&usrp);

	if(strcmp(device_args,"")){
		free(device_args);
	}

}

int is_rx_error(uhd_rx_metadata_error_code_t error_code){

	switch (error_code){
		case UHD_RX_METADATA_ERROR_CODE_NONE:
			return 0; 
			break;
		case UHD_RX_METADATA_ERROR_CODE_TIMEOUT:  
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_TIMEOUT\n");
			break;
		case UHD_RX_METADATA_ERROR_CODE_LATE_COMMAND:     
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_LATE_COMMAND\n");
			break;
		case UHD_RX_METADATA_ERROR_CODE_BROKEN_CHAIN:     
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_BROKEN_CHAIN\n");
			break;
		case UHD_RX_METADATA_ERROR_CODE_OVERFLOW:   
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_OVERFLOW\n");
			break;
		case UHD_RX_METADATA_ERROR_CODE_ALIGNMENT:  
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_ALIGNMENT\n");
			break;
		case UHD_RX_METADATA_ERROR_CODE_BAD_PACKET:
			syslog(LOG_ERR, "ERROR: UHD_RX_METADATA_ERROR_CODE_BAD_PACKET\n");
			break;
		default:
			syslog(LOG_ERR, "Unidentified error occured: 0x%x\n", error_code);
	}

	return 1;

}


void * queue_pop_thread(void* args){
	syslog(LOG_INFO, "wx: starting thread");
	int frame_len = 2044 * 2;
	FILE* data_stream = NULL;
	char buf[256];
	int frame_num = 0;
	uint64_t num_samples = 0;
	uint64_t num_bytes = 0;
	int file_num = 0;

	float data_buf[frame_len];

	bool empty = true;
	syslog(LOG_DEBUG, "wx: Initializing loop variables");
	empty = queue_isEmpty(&data_queue);

	syslog(LOG_INFO, "wx: starting loop");
	while(program_on || !empty){
		syslog(LOG_DEBUG, "wx: checking for frame");
		empty = queue_isEmpty(&data_queue);
		syslog(LOG_DEBUG, "wx: queue has %d remaining", data_queue.length);
		if(data_queue.length > QUEUE_WARNING_LENGTH){
			syslog(LOG_NOTICE, "wx: queue has more than %d frames!", QUEUE_WARNING_LENGTH);
		}
		if(data_queue.length > QUEUE_CRITICAL_LENGTH){
			syslog(LOG_WARNING, "wx: queue has more than %d frames!", QUEUE_CRITICAL_LENGTH);
		}

		if(!empty){
			syslog(LOG_DEBUG, "wx: data frame exists");
			if(frame_num / FRAMES_PER_FILE + 1 != file_num){
				syslog(LOG_INFO, "wx: needs new file");
				if(data_stream){
					fclose(data_stream);
				}
				snprintf(buf, sizeof(buf), "%s/RAW_DATA_%06d_%06d", DATA_DIR, run_num,
					frame_num / FRAMES_PER_FILE + 1);
				file_num++;
				data_stream = fopen(buf, "wb");
				syslog(LOG_DEBUG, "wx: New file: %s", buf);
			}
			float* data_ptr = NULL;
			syslog(LOG_DEBUG, "wx: popping frame");
			data_ptr = (float*) queue_pop(&data_queue);
			syslog(LOG_DEBUG, "wx: got data frame %p", data_ptr);

			for(int i = 0; i < frame_len; i++){
				data_buf[i] = (float) data_ptr[i];
			}
			syslog(LOG_DEBUG, "wx: buffered data");
			int bytes_written = fwrite(data_buf, sizeof(float), frame_len, data_stream) * sizeof(float);
			num_bytes += bytes_written;
			syslog(LOG_DEBUG, "wx: Wrote %d bytes", bytes_written);
			free(data_ptr);
			syslog(LOG_DEBUG, "wx: Freed data pointer");
			frame_num++;
			num_samples += frame_len / 2;
		}else{
			syslog(LOG_NOTICE, "wx: no data, sleeping for %d us", FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS * 1000);

			usleep(FILE_CAPTURE_DAEMON_SLEEP_PERIOD_MS * 1000);
		}
	}
	syslog(LOG_DEBUG, "wx: ended loop");

	pthread_mutex_lock(&thread_compete_mutex);
	while(!push_complete){
		pthread_cond_wait(&thread_complete, &thread_compete_mutex);
	}
	pthread_mutex_unlock(&thread_compete_mutex);

	while(!queue_isEmpty(&data_queue)){
		if(frame_num / FRAMES_PER_FILE + 1 != file_num){
			if(data_stream){
				fclose(data_stream);
			}
			snprintf(buf, sizeof(buf), "%s/RAW_DATA_%06d_%06d", DATA_DIR, run_num,
				frame_num / FRAMES_PER_FILE + 1);
			file_num++;
			data_stream = fopen(buf, "wb");
		}
		float* data_ptr = NULL;
		data_ptr = (float*) queue_pop(&data_queue);

		for(int i = 0; i < frame_len; i++){
			data_buf[i] = (float) data_ptr[i];
		}
		num_bytes += fwrite(data_buf, sizeof(float), frame_len, data_stream) * sizeof(float);
		free(data_ptr);
		frame_num++;
		num_samples += frame_len / 2;
	}

	fclose(data_stream);
	syslog(LOG_NOTICE, "wx: Recorded %f seconds of data to disk\n", num_samples / 2048000.0);
	syslog(LOG_NOTICE, "wx: Recorded %ld bytes of data to disk\n", num_bytes);
	syslog(LOG_INFO, "wx: Queue length at end: %d.\n", data_queue.length);
	return NULL;
}

void * stream_push_thread(void* args){

	uhd_rx_metadata_error_code_t error_code;
	size_t num_rx_samps = 0;

	syslog(LOG_INFO, "rx: Starting USRP stream");
	uhd_rx_streamer_issue_stream_cmd(rx_streamer, &stream_cmd);

	syslog(LOG_DEBUG, "rx: allocating buffers");
	buff = malloc(samps_per_buff * 2 * sizeof(float));
	buffs_ptr = (void**)&buff;
	uint64_t sample_counter = 0;

	syslog(LOG_INFO, "rx: Starting loop");
	while(program_on){
		num_rx_samps = 0;
		syslog(LOG_DEBUG, "rx: Receiving data...");
		uhd_rx_streamer_recv(rx_streamer, buffs_ptr, samps_per_buff, &md, TIMEOUT_SEC, false, &num_rx_samps);
		uhd_rx_metadata_error_code(md, &error_code);
		syslog(LOG_DEBUG, "rx: Data received");
		if(is_rx_error(error_code)){
			syslog(LOG_WARNING, "rx: USRP issued warning");
		}else{
			syslog(LOG_DEBUG, "rx: USRP no warnings");
		}

		if(num_rx_samps > 0){
			syslog(LOG_DEBUG, "rx: enqueing data");
			syslog(LOG_DEBUG, "rx: allocating new frame of size %lu", samps_per_buff * 2 * sizeof(float));
			sample_counter += num_rx_samps;

			if(samps_per_buff != num_rx_samps){
				syslog(LOG_WARNING, "rx: incomplete buffer!");
			}

			float* newframe = malloc(samps_per_buff * 2 * sizeof(float));
			if(newframe == NULL){
				syslog(LOG_CRIT, "rx: Failed to allocate new frame!");
				return NULL;
			}
			syslog(LOG_DEBUG, "rx: copying data");
			for(int i = 0; i < num_rx_samps * 2; i++){
				newframe[i] = buff[i];
			}
			syslog(LOG_DEBUG, "rx: pushing frame");
			queue_push(&data_queue, (void*) newframe);
		}
		syslog(LOG_DEBUG, "rx: looping");
	}

	syslog(LOG_NOTICE, "rx: Recorded %lu samples, expect %lu bytes\n", sample_counter, sample_counter * sizeof(float) * 2);

	syslog(LOG_INFO, "rx: stopping USRP");
	stream_cmd.stream_mode = UHD_STREAM_MODE_STOP_CONTINUOUS;
	uhd_rx_streamer_issue_stream_cmd(rx_streamer, &stream_cmd);

	syslog(LOG_INFO, "rx: cleaning resources");
	free(buff);

	pthread_mutex_lock(&thread_compete_mutex);
	push_complete = 1;
	pthread_cond_signal(&thread_complete);
	pthread_mutex_unlock(&thread_compete_mutex);

	syslog(LOG_DEBUG, "rx: exiting");

	return NULL;

}

void sig_handler(int sig){
	program_on = 0;
	syslog(LOG_WARNING, "Caught termination signal");

}

int main(int argc, char* argv[]){
	pthread_t push, pop;

	pthread_mutex_init(&thread_compete_mutex, NULL);
	pthread_cond_init(&thread_complete, NULL);

	openlog("sdr_record", LOG_PERROR, LOG_USER);
	int mask = 0;
	for(int i = 0; i <= 4; i++){
		mask |= 1 << i;
	}
	setlogmask(mask);

	syslog(LOG_DEBUG, "Setting UHD thread priority to default");

	if(uhd_set_thread_priority(uhd_default_thread_priority, true)){
		syslog(LOG_WARNING, "Unable to set thread priority. Continuing anyway.");
	}

	/*Process options*/
	syslog(LOG_INFO, "Getting command line options");
	int option = 0;
	while((option = getopt(argc, argv, "hg:s:f:r:o:v:")) != -1){
		switch(option)
		{
			case 'h':
				syslog(LOG_INFO, "Got help flag");
				print_help();
				break;
			case 'g':
				gain = (int)(atof(optarg));
				syslog(LOG_INFO, "Got gain setting of %.2f", gain);
				break;
			case 's':
				rate = (int)(atof(optarg));
				syslog(LOG_INFO, "Got sampling rate setting of %lf", rate);
				break;
			case 'f':
				tune_request.target_freq = atof(optarg);
				syslog(LOG_INFO, "Got center frequency target of %lf", tune_request.target_freq);
				break;
			case 'r':
				run_num = atoi(optarg);
				syslog(LOG_INFO, "Got run number of %d", run_num);
				break;
			case 'o':
				strcat(DATA_DIR, optarg);
				syslog(LOG_INFO, "Got an output directory of %s", DATA_DIR);
				break;
			case 'v':
				syslog(LOG_INFO, "Setting log output to %d", atoi(optarg));
				mask = 0;
				for(int i = 0; i <= atoi(optarg); i++){
					mask |= 1 << i;
				}
				setlogmask(mask);
				break;
		}
	}

	syslog(LOG_INFO, "Sanity checking args");

	if (run_num == DEFAULT_RUN) {
		syslog(LOG_ERR, "Must set run number\n");
		print_help();
	}

	if (gain == DEFAULT_GAIN){
		syslog(LOG_ERR, "Must set gain\n");
		print_help();
	}

	if (strlen(DATA_DIR) == 0){
		syslog(LOG_ERR, "Must set directory\n");
		print_help();
	}

	if (tune_request.target_freq == DEFAULT_FREQ){
		syslog(LOG_ERR, "Must set freq\n");
		print_help();
	}

	if (rate == DEFAULT_RATE){
		syslog(LOG_ERR, "Must set rate\n");
		print_help();
	}

	syslog(LOG_INFO, "Setting signal handler");
	signal(SIGINT, sig_handler);

	syslog(LOG_INFO, "Initializing data queue");
	queue_init(&data_queue);

	printf("\n\n========================= Initializing Radio... =========================\n");
	syslog(LOG_INFO, "Initializing Radio");
	if(radio_init()){
		syslog(LOG_CRIT, "Failed to initialize radio! Exiting...");
		exit(1);
	}
	syslog(LOG_DEBUG, "Printing metadata to file");
	print_meta_data();

	printf("\n\n========================== Getting Samples... ===========================\n");
	syslog(LOG_INFO, "Starting threads");
	syslog(LOG_DEBUG, "Starting receiver thread");
	pthread_create(&push, NULL, stream_push_thread, NULL);
	syslog(LOG_DEBUG, "Starting writer thread");
	pthread_create(&pop, NULL, queue_pop_thread, NULL);

	syslog(LOG_INFO, "main thread sleeping");
	pthread_join(pop, NULL);
	syslog(LOG_DEBUG, "writer thread joined with main");
	pthread_join(push, NULL);
	syslog(LOG_DEBUG, "receiver thread joined with main");

	syslog(LOG_INFO, "main thread awoke");

	if(!queue_isEmpty(&data_queue)){
		syslog(LOG_ERR, "Program exit without writing FIFO to disk\n");
	}

	syslog(LOG_INFO, "cleaning up resources");
	syslog(LOG_DEBUG, "deinitializing radio");
	radio_deinit();

	syslog(LOG_DEBUG, "deinitializing queues");
	queue_destroy(&data_queue);

	syslog(LOG_DEBUG, "deinitializing semaphores");
	pthread_mutex_destroy(&thread_compete_mutex);
	pthread_cond_destroy(&thread_complete);
	syslog(LOG_WARNING, "Exiting");
	return 0;
}


