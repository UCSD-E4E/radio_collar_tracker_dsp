#include <stdio.h>
#include <float.h>

int main(int argc, char** argv){
	FILE* stream = fopen("COMP_DATA", "rb");
	float max = FLT_MIN;
	float min = FLT_MAX;

	float next = 0;
	while(fread(&next, sizeof(float), 1, stream)){
		if(next < min){
			min = next;
		}
		if(next > max){
			max = next;
		}
	}
	printf("Max: %f\nMin: %f\n", max, min);
}
