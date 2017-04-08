/*
 * @file rct_sdr.h
 *
 * @author Jacob W Torres, jaketorres00@gmail.com 
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
#include <semaphore.h>

//Uncomment to enable verbose. Alternative: make verbose
//#define RCT_VERBOSE           		1

/////////////////////////////////////////////////////////////////////////
//Semaphore Locking
/////////////////////////////////////////////////////////////////////////

sem_t sem, mutex;
#define WAIT_FOR_FIFO_TO_FILL()         sem_wait(&sem)
#define SET_FIFO_NONEMPTY()             sem_post(&sem)

#define LOCK()                          sem_wait(&mutex)
#define UNLOCK()                        sem_post(&mutex)

#define SEMAPHORE_BETWEEN_THREADS       0
#define SEMAPHORE_BETWEEN_PROCESSES     1
#define SEMAPHORE_INIT                  0   /*init semaphore to block initially*/
#define MUTEX_INIT                      1   /*init mutex - initially available*/

#define THREAD_SYNC(x)                  x

/////////////////////////////////////////////////////////////////////////
//FIFO Header
/////////////////////////////////////////////////////////////////////////


#define data_type float

typedef data_type DATA_T;
typedef unsigned int uint32_t;

typedef struct
{
    data_type * buffer;
    uint32_t size;
    uint32_t used;
    volatile unsigned int tail;
    volatile unsigned int head;
    volatile unsigned int err;
} rct_fifo_t;

#define SUCCESS     0
#define FAIL        1

int fifo_enqueue_multiple_elements(  rct_fifo_t * fifo, DATA_T * data_in, uint32_t len );
DATA_T fifo_pop( rct_fifo_t * fifo );
int fifo_is_empty( rct_fifo_t * fifo );
int fifo_deinit( rct_fifo_t * fifo );
int fifo_init( rct_fifo_t* fifo, uint32_t size );

/////////////////////////////////////////////////////////////////////////
//RCT Header
/////////////////////////////////////////////////////////////////////////

#ifdef RCT_VERBOSE
	#define vprintf(...)        fprintf(stderr, __VA_ARGS__)
	#define eprintf(...)        fprintf(stderr, __VA_ARGS__)
#else
	FILE * fp_err;
	#define vprintf(...)    
	#define eprintf(...)        fprintf(fp_err, __VA_ARGS__)
#endif

