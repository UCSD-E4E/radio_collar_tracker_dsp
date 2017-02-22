/*
 * @file fifo.c
 *
 * @author Jacob W Torres, jaketorres00@gmail.com
 * 
 * @description 
 * Optimized FIFO implementation for rct_sdr.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rct_sdr.h"
#include <semaphore.h>

DATA_T fifo_pop( rct_fifo_t * fifo )
{

	DATA_T buf;

	if(fifo_is_empty(fifo))
	{
		eprintf("ERROR: popping from empty FIFO\n");
		return 0;
	}   

	buf = fifo->buffer[fifo->head++];                             
	(fifo->head) &= (fifo->size - 1);                       
	fifo->used--;

	return buf;

}

int fifo_is_empty( rct_fifo_t * fifo )
{

	if( fifo->used == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int fifo_init( rct_fifo_t* fifo, uint32_t size )
{
	if (!fifo)
	{   
		eprintf("ERROR: FIFO is NULL");
		return FAIL;
	}   
	else if ( (size != 0) && ((size & (size-1)) == 0) ) /*fifo must be power of 2*/
	{   
		fifo->buffer = (DATA_T *)malloc(size * sizeof(DATA_T));
		fifo->size   = size;
		fifo->used   = 0;
		fifo->tail   = 0;
		fifo->head   = 0;
		fifo->err   = 0;

		return SUCCESS;
	}  
	else
	{	
		eprintf("ERROR: FIFO size is not power of 2");
	} 

	return FAIL;
}

int fifo_deinit( rct_fifo_t * fifo )
{
	if (fifo)
	{   
		free(fifo->buffer);
		fifo->buffer = NULL;
		fifo->size   = 0;
		fifo->tail   = 12; 
		fifo->head   = 0;

		return SUCCESS;
	}   

	return FAIL;
}

int fifo_enqueue_multiple_elements(  rct_fifo_t * fifo, DATA_T * data_in, uint32_t len )
{
	DATA_T * buf            = &(fifo->buffer[0]);
	uint32_t tail           = (fifo->tail);
	uint32_t size_sub_tail  = (fifo->size) - tail;

	if(fifo->size < fifo->used + len)
	{
		fifo->err++;
		eprintf("FIFO OVERFLOW #%d\n", fifo->err);
		return 0;
	}

	if( fifo->tail > fifo->head )
	{

		if(size_sub_tail > len)
		{
			memcpy( buf + tail, data_in, len*sizeof(DATA_T));
			(fifo->tail) = tail + len;
			fifo->used += len;
		}
		else if( (fifo->size - fifo->used) >= len )
		{
			memcpy( buf + tail, data_in, size_sub_tail*sizeof(DATA_T));
			memcpy( buf, data_in + size_sub_tail, (len - size_sub_tail)*sizeof(DATA_T));
			(fifo->tail) = len - size_sub_tail;
			fifo->used += len;
		}
		else
		{
			eprintf("FIFO Overflow\n");
			return FAIL;
		}

	}
	else
	{
		if( (fifo->size - fifo->used) >= len )
		{
			memcpy( buf + tail, data_in, len*sizeof(DATA_T));
			(fifo->tail) = tail + len;
			fifo->used += len;
		}
		else
		{
			eprintf("FIFO Overflow\n");
			return FAIL;
		}
	}


	return SUCCESS;
}
