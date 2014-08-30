#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PIN_PATH "/sys/class/gpio/gpio7/value"
#define BUFFER_SIZE 256

FILE *fileStream;
char fileBuffer[BUFFER_SIZE];
int val = 0;

int main(int argc, char *argv[]){
	system("echo 7 > /sys/class/gpio/export");
	for (;;){
		fileStream = fopen(PIN_PATH,"r");
		fgets(fileBuffer, BUFFER_SIZE, fileStream);
		val = atoi(fileBuffer);
		if(!val)
			system("/home/debian/xcode/collarTracker");
		printf("%d\n",val);
		fclose(fileStream);
	}
	return 1;
}
