#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void siginthandler(int sig);

int main(int argc, char const *argv[]) {
	signal(SIGINT, siginthandler);

	printf("Trying to connect to /dev/ttyUSB0.. success.\r\n");
	fflush(stdout);
	sleep(1);
	printf("Trying to configure /dev/ttyUSB0.. success\r\n");

	printf("GPS and SDR operating!\r\n");
	fflush(stdout);
	int file = 0;
	while (1) {
		for (int i = 0; i < 4; i++) {
			sleep(1);
			printf("Current Frame: %03d Gain: 297\r\n", i);
			fflush(stdout);
		}
		printf("FILE: %06d\n", file++);
		fflush(stdout);
	}
	return 0;
}

void siginthandler(int sig) {
	printf("Got SIGINT!!!\r\n");
	fflush(stdout);
	exit(-1);
}
