CFLAGS=-c -std=gnu99 -Wall
LFLAGS=`pkg-config --cflags --libs librtlsdr`
.PHONY: clean

sdr_record: sdr_record.o
	gcc sdr_record.o $(LFLAGS) -o sdr_record

clean:
	-rm -rf *.o
	-rm -rf sdr_record

sdr_record.o: sdr_record.c
	gcc $(CFLAGS) sdr_record.c


