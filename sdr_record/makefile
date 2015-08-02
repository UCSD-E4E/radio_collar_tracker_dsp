CFLAGS=-c -std=gnu99 -Wall -g
CPPFLAGS=-c -std=c++11 -Wall -g
LFLAGS=`pkg-config --cflags --libs librtlsdr` -pthread -lrt
.PHONY: clean test

sdr_record: sdr_record.o queue.o
	gcc sdr_record.o queue.o $(LFLAGS) -o sdr_record

clean:
	-rm -rf *.o
	-rm -rf sdr_record
	-rm -rf rtl_sdr
	-rm -rf test

sdr_record.o: sdr_record.c queue.h
	gcc sdr_record.c $(CFLAGS) 

queue.o: queue.c
	gcc queue.c $(CFLAGS)

test: sdr_record
	./sdr_record -r 1

raw_data_to_complex: raw_data_to_complex.c
	gcc raw_data_to_complex.c $(CFLAGS)
	gcc raw_data_to_complex.o $(LFLAGS) -o raw_data_to_complex

switch_iq: switch_iq.c
	gcc switch_iq.c $(CFLAGS)
	gcc switch_iq.o $(LFLAGS) -o switch_iq

fopentest: fopentest.c
	gcc fopentest.c $(CFLAGS)
	gcc fopentest.o $(LFLAGS) -o fopentest
