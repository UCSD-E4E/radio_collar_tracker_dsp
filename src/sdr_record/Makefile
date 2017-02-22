all:
	gcc fifo.c rct_sdr.c -Wall -lpthread -lm -luhd -o sdr_record

verbose:
	gcc fifo.c rct_sdr.c -DRCT_VERBOSE -Wall -lpthread -lm -luhd -o sdr_record

fifo_test:	
	gcc fifo.c fifo_test.c

clean:
	rm -f sdr_record *.out
	rm -rf data
	mkdir data
