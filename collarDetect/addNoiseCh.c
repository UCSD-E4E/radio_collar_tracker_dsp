/**
 * This program is intended to read a list of collar frequencies and generate a
 * frequency that can be used as a noise baseline.  The assumptions made are
 * that the collar file is plain text, has one collar per line, and has units
 * of Hertz.
 */

//////////////
// Includes //
//////////////
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/////////////////
// Global Vars //
/////////////////
char* progName;

/////////////////////////
// Function Prototypes //
/////////////////////////
void printHelp();

//////////////////////////
// Function Definitions //
//////////////////////////
void printHelp(){
	printf("Usage: %s NUM_COLLARS COLLAR_FILE\n", progName);
	printf("\n");
	printf("%s is a program that generates a noise baseline channel for the radio\n", progName);
	printf("collar tracker spectrumAnalysis tool to use in positively identifying\n");
	printf("radio collars based on a signal to noise ratio.\n");
}

int main(int argc, char** argv){
	progName = "addNoiseCh";
	if(argc != 3){
		printHelp();
		return 1;
	}
	FILE *collarFreqFile = fopen(argv[2], "r");
	if(collarFreqFile == NULL){
		printHelp();
		return 1;
	}

	int numCollars;
	int retVal = sscanf(argv[1], "%d", &numCollars);
	if(retVal != 1){
		printHelp();
		return 1;
	}
	uint32_t collarFreqs[numCollars];

	for(int i = 0; i < numCollars; i++){
		retVal = fscanf(collarFreqFile, "%d", &collarFreqs[i]);
		if(retVal != 1){
			printf("Bad File! Exiting...\n");
			return 1;
		}
	}

	uint32_t meanFreq = 0;
	for(int i = 0; i < numCollars; i++){
		meanFreq += collarFreqs[i];
	}
	meanFreq /= numCollars;
	meanFreq -= meanFreq % 2500;

	int noiseFreq = meanFreq;
	for(int i = 0; i < numCollars; i++){
		if(abs(noiseFreq - collarFreqs[i]) < 2500){
			noiseFreq += 2500;
			i = 0;
		}
	}

	collarFreqFile = freopen(NULL, "a", collarFreqFile);

	fprintf(collarFreqFile, "%d\n", noiseFreq);

	return 0;
}