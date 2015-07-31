CFLAGS=-c -std=gnu99 -Wall -g
CPPFLAGS=-c -std=c++11 -Wall -g
LFLAGS=`pkg-config --cflags --libs librtlsdr`
.PHONY: clean

sdr_record: sdr_record.o
	g++ sdr_record.o $(LFLAGS) -o sdr_record

clean:
	-rm -rf *.o
	-rm -rf sdr_record

sdr_record.o: sdr_record.cpp
	g++ sdr_record.cpp $(CPPFLAGS) 


