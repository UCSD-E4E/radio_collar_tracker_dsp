.PHONY: all clean
addNoiseCh: addNoiseCh.c
	gcc addNoiseCh.c -o addNoiseCh -std=gnu99

all: addNoiseCh spectraCollarID

clean:
	rm addNoiseCh

spectraCollarID: spectraCollarID.cpp
	g++ spectraCollarID.cpp -o spectraCollarID -std=gnu++11