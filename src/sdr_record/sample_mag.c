/*
RCT Payload Software
Copyright (C) 2016  Hui, Nathan T.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
		float magnitude = sqrt((next[0]-128) * (next[0]-128) + (next[1]-128) * (next[1]-128));
		if(magnitude > max){
			max = magnitude;
		}
		if(magnitude < min){
			min = magnitude;
		}
	}
	printf("Max: %f\nMin: %f\n", max, min);
}
