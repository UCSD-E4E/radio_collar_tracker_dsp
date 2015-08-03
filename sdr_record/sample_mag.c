#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

int main(int argc, char** argv){
	FILE* stream = fopen("test_data/foo.raw", "rb");
	//FILE* stream = fopen("test_data/RAW_DATA_000001_000001", "rb");
	//FILE* stream = fopen("test.raw", "rb");
	float max = FLT_MIN;
	float min = FLT_MAX;

	uint8_t next[2];
	while(fread(&next, sizeof(uint8_t) * 2, 1, stream)){
		float magnitude = sqrt(next[0] * next[0] + next[1] * next[1]);
		if(magnitude > max){
			max = magnitude;
		}
		if(magnitude < min){
			min = magnitude;
		}
	}
	printf("Max: %f\nMin: %f\n", max, min);
}
