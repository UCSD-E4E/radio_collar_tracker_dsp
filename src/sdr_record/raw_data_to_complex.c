#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, char** argv){
	int opt;
	FILE* infile;
	FILE* outfile;
	uint8_t buf[4096];
	float fbuf[4096];
	int nread = 0;
	int totread = 0;

	while((opt = getopt(argc, argv, "o:i:")) != -1){
		switch(opt){
			case 'o':
				printf("Using %s as output.\n", optarg);
				outfile = fopen(optarg, "ab");
				break;
			case 'i':
				printf("Using %s as input.\n", optarg);
				infile = fopen(optarg, "rb");
				break;
			default:
				printf("Usage: \t -o output_file -i input_file\n");
				break;
		}
	}
	if(!outfile || !infile){
		printf("Usage: \t -o output_file -i input_file\n");
		exit(1);
	}

	while((nread = fread((void*)buf, sizeof(uint8_t), 4096, infile)) > 0){
		totread += nread;
		for(int i = 0; i < nread; i++){
			fbuf[i] = (float)((float)buf[i] -128.0)/128.0;
		}
		int retval = fwrite(fbuf, sizeof(float), nread, outfile);
		if(retval != nread){
			fprintf(stderr, "Failed to complete write!\n");
			exit(-1);
		}
	}
	fclose(infile);
	fclose(outfile);
	printf("Read %lu bytes, wrote %lu bytes\n", totread * sizeof(char), totread * sizeof(float));
	
	// get file size
	return 0;
}


