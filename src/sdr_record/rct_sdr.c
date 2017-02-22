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
#include "getopt.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rct_sdr.h"
#include <semaphore.h>


/////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////


#define TIMEOUT_SEC		        3.0
#define META_PREFIX             "META_"
#define FIFO_SIZE               1024*1024*32
#define SAMPLES_PER_FILE        1024*1024*8

#define DEFAULT_RATE            -1
#define DEFAULT_FREQ            -1
#define DEFAULT_GAIN            -1
#define DEFAULT_RUN             -1


/////////////////////////////////////////////////////////
// Function Declarations
/////////////////////////////////////////////////////////
void    radio_deinit        ( void );
void    radio_init          ( void );
void    print_help          ( void );
void    print_meta_data     ( void );
void    sig_handler         ( int sig );
void *  stream_push_thread  ( void * args );
void *  queue_pop_thread    ( void * args );
int     is_rx_error         ( uhd_rx_metadata_error_code_t error_code );

/////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////

double freq = DEFAULT_FREQ;
double rate = DEFAULT_RATE;
double gain = DEFAULT_GAIN;
int run_num = DEFAULT_RUN;
volatile int still_pushing_into_fifo = 1;
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

rct_fifo_t rct_fifo;

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


/////////////////////////////////////////////////////////
// Function Definitions
/////////////////////////////////////////////////////////

void print_help( void )
{
    vprintf("sdr_record - Radio Collar Tracker drone application to pull IQ samples from USRP and dump to disk\n\n"
            "Options:\n"
            "    -r (run_number)\n"
            "    -f (center frequency in Hz)\n"
            "    -s (sample rate in Hz)\n"
            "    -g (gain)\n"
            "    -o (output directory)\n"
            "    -h (print this help message)\n");

#ifndef RCT_VERBOSE
    fclose(fp_err);
#endif

    exit(0);

}

void radio_init( void )
{
    double temp_param;

    /* Init USRP object */
    vprintf("Creating USRP with args \"%s\"...\n", device_args);
    uhd_usrp_make(&usrp, device_args);

    /* Create RX streamer */
    uhd_rx_streamer_make(&rx_streamer);

    /* Create RX metadata */
    uhd_rx_metadata_make(&md);

    /* Set rate */
    temp_param = rate;
    vprintf("Setting RX Rate: %f...\n", rate);
    uhd_usrp_set_rx_rate(usrp, rate, channel);

    /* See what rate actually is */
    uhd_usrp_get_rx_rate(usrp, channel, &rate);
    vprintf("Actual RX Rate: %f...\n", rate);

    if (temp_param != rate)
    {
        eprintf("WARNING: RX rate not correctly set\n");
    }


    /* Set gain */
    temp_param = gain;
    vprintf("Setting RX Gain: %f dB...\n", gain);
    uhd_usrp_set_rx_gain(usrp, gain, channel, ""); 

    /* See what gain actually is */
    uhd_usrp_get_rx_gain(usrp, channel, "", &gain);
    vprintf("Actual RX Gain: %f...\n", gain);

    if (temp_param != gain)
    {
        eprintf("WARNING: RX gain not correctly set\n");
    }

    /*Set frequency*/
    temp_param = tune_request.target_freq;
    vprintf("Setting RX frequency: %f MHz...\n", tune_request.target_freq / 1e6);
    uhd_usrp_set_rx_freq(usrp, &tune_request, channel, &tune_result);

    /*See what frequency actually is*/
    uhd_usrp_get_rx_freq(usrp, channel, &freq);
    vprintf("Actual RX frequency: %f MHz...\n", freq / 1e6);

    if (temp_param != freq)
    {
        vprintf("%f \t %f\n", temp_param, freq);
        eprintf("WARNING: RX freq not correctly set\n");
    }


    /*Set up streamer*/
    stream_args.channel_list = &channel;
    uhd_usrp_get_rx_stream(usrp, &stream_args, rx_streamer);

    /*Set up buffer*/
    uhd_rx_streamer_max_num_samps(rx_streamer, &samps_per_buff);
    vprintf("Buffer size in samples: %zu\n", samps_per_buff);

}

void print_meta_data(void)
{

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

void radio_deinit( void )
{

    vprintf("Cleaning up RX streamer.\n");
    uhd_rx_streamer_free(&rx_streamer);

    vprintf("Cleaning up RX metadata.\n");
    uhd_rx_metadata_free(&md);

    vprintf("Cleaning up USRP.\n");
    if(usrp != NULL)
    {
        uhd_usrp_last_error(usrp, error_string, 512);
        vprintf("USRP reported the following error: %s\n", error_string);
    }
    uhd_usrp_free(&usrp);

    if(strcmp(device_args,""))
    {
        free(device_args);
    }

}

int is_rx_error( uhd_rx_metadata_error_code_t error_code )
{

    switch ( error_code )
    {
        case UHD_RX_METADATA_ERROR_CODE_NONE:
            return 0; 
            break;
        case UHD_RX_METADATA_ERROR_CODE_TIMEOUT:  
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_TIMEOUT\n");
            break;
        case UHD_RX_METADATA_ERROR_CODE_LATE_COMMAND:     
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_LATE_COMMAND\n");
            break;
        case UHD_RX_METADATA_ERROR_CODE_BROKEN_CHAIN:     
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_BROKEN_CHAIN\n");
            break;
        case UHD_RX_METADATA_ERROR_CODE_OVERFLOW:   
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_OVERFLOW\n");
            break;
        case UHD_RX_METADATA_ERROR_CODE_ALIGNMENT:  
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_ALIGNMENT\n");
            break;
        case UHD_RX_METADATA_ERROR_CODE_BAD_PACKET:
            eprintf("ERROR: UHD_RX_METADATA_ERROR_CODE_BAD_PACKET\n");
            break;
        default:
            eprintf("Unidentified error occured: 0x%x\n", error_code);
    }

    return 1;

}


void * queue_pop_thread( void * args )
{

    int frame_num = 1;
    int num_samples_written = 0;
    char file_name_buff[256];
    FILE* data_stream = NULL;
    float val;
    const int buf_len = 1024*4;
    float data_buffer[buf_len];
    int buffer_i = 0;

    snprintf(file_name_buff, sizeof(file_name_buff), "%s/RAW_DATA_%06d_%06d", DATA_DIR, run_num, frame_num);
    vprintf("File: %s\n", file_name_buff);             
    data_stream = fopen(file_name_buff, "wb");


    while( still_pushing_into_fifo )
    {

        if( fifo_is_empty(&rct_fifo) )
        {
            LOCK();
            waiting_for_kick = 1;
            UNLOCK();
 
            WAIT_FOR_FIFO_TO_FILL();
        }

#ifdef RCT_VERBOSE
        if(fifo_is_empty(&rct_fifo))
        {
            vprintf("WARNING: Unexpected Busy Wait, FIFO should not be empty\n");
        }
#endif

        while( !fifo_is_empty(&rct_fifo) )
        {

            if( num_samples_written == SAMPLES_PER_FILE  )
            {
                frame_num++;
                num_samples_written = 0;
                fclose(data_stream);
                snprintf(file_name_buff, sizeof(file_name_buff), "%s/RAW_DATA_%06d_%06d", DATA_DIR, run_num, frame_num);
                vprintf("File: %s\n", file_name_buff);             
                data_stream = fopen(file_name_buff, "wb");
            }


            if(fifo_is_empty(&rct_fifo))
            {
                vprintf("Incorrectly kicked\n");
                break;
            }      

            val = fifo_pop( &rct_fifo );

            data_buffer[buffer_i++] = val;
            num_samples_written++;

            if( buffer_i == buf_len )
            {
                buffer_i = 0;
                fwrite(data_buffer, sizeof(float), buf_len, data_stream);
            }

        }

    }

    vprintf("Exit Loop\n");

    fclose(data_stream);

    return NULL;
}

void * stream_push_thread( void * args )
{

    uhd_rx_metadata_error_code_t error_code;
    size_t num_rx_samps = 0;


    uhd_rx_streamer_issue_stream_cmd(rx_streamer, &stream_cmd);

    buff = malloc(samps_per_buff * 2 * sizeof(float));
    buffs_ptr = (void**)&buff;

    while(program_on)
    {
        num_rx_samps = 0;
        uhd_rx_streamer_recv(rx_streamer, buffs_ptr, samps_per_buff, &md, TIMEOUT_SEC, false, &num_rx_samps);
        uhd_rx_metadata_error_code(md, &error_code);
        is_rx_error(error_code);

        if(num_rx_samps > 0)
        {    
            fifo_enqueue_multiple_elements( &rct_fifo, buff, num_rx_samps*2  );
            
            LOCK();
            if(waiting_for_kick)
            {    
                SET_FIFO_NONEMPTY();
                waiting_for_kick = 0;
            }
            UNLOCK();
        }
    }

    vprintf("Exit Loop\n");

    stream_cmd.stream_mode = UHD_STREAM_MODE_STOP_CONTINUOUS;
    uhd_rx_streamer_issue_stream_cmd(rx_streamer, &stream_cmd);

    free(buff);

    still_pushing_into_fifo = 0;

    return NULL;

}

void sig_handler( int sig )
{

    program_on = 0;
    vprintf("\nCought Interrupt\n");

}

int main( int argc, char* argv[] )
{

    int option = 0;
#ifdef RCT_VERBOSE
    int err = 0;
#endif
    pthread_t push, pop;

#ifndef RCT_VERBOSE
    fp_err = fopen("./err_log/err.log", "a");
#endif

    if( uhd_set_thread_priority(uhd_default_thread_priority, true) )
    {
        eprintf("Unable to set thread priority. Continuing anyway.\n");
    }

    /*Process options*/
    while((option = getopt(argc, argv, "hg:s:f:r:o:")) != -1)
    {
        switch(option)
        {
            case 'h':
                print_help();
            case 'g':
                gain = (int)(atof(optarg));
                break;
            case 's':
                rate = (int)(atof(optarg));
                break;
            case 'f':
                tune_request.target_freq = atof(optarg);
                break;
            case 'r':
                run_num = atoi(optarg);
                break;
            case 'o':
                strcat(DATA_DIR, optarg );
                break;
        }
    }


    if (run_num == DEFAULT_RUN) 
    {
        eprintf("ERROR: Must set run number\n");
        print_help();
    }

    if (gain == DEFAULT_GAIN)
    {
        eprintf("ERROR: Must set gain\n");
        print_help();
    }

    if ( strlen(DATA_DIR) == 0 )
    {
        eprintf("ERROR: Must set directory\n");
        print_help();
    }

    if (tune_request.target_freq == DEFAULT_FREQ)
    {
        eprintf("ERROR: Must set freq\n");
        print_help();
    }

    if (rate == DEFAULT_RATE)
    {
        eprintf("ERROR: Must set rate\n");
        print_help();
    }

    signal(SIGINT, sig_handler); 
    fifo_init(&rct_fifo, FIFO_SIZE);

    vprintf("\n\n========================= Initializing Radio... =========================\n");
    radio_init();
    print_meta_data();

    THREAD_SYNC(sem_init(&sem, SEMAPHORE_BETWEEN_THREADS, SEMAPHORE_INIT));
    THREAD_SYNC(sem_init(&mutex, SEMAPHORE_BETWEEN_THREADS, MUTEX_INIT));

    vprintf("\n\n========================== Getting Samples... ===========================\n");

    pthread_create(&push, NULL, stream_push_thread, NULL);
    pthread_create(&pop, NULL, queue_pop_thread, NULL);

    pthread_join(pop, NULL);
    pthread_join(push, NULL);

#ifdef RCT_VERBOSE
    err = rct_fifo.err;
#endif

    if(!fifo_is_empty(&rct_fifo)) 
    {
        eprintf("ERROR: Program exit without writing FIFO to disk\n");
    }

    radio_deinit();    
    fifo_deinit(&rct_fifo);
    THREAD_SYNC(sem_destroy(&sem));
    THREAD_SYNC(sem_destroy(&mutex));

#ifndef RCT_VERBOSE
    fclose(fp_err);
#endif

    vprintf("\n\n============ Program Terminated With %d FIFO Overflow Errors ============\n", err);
    return 0;

}


