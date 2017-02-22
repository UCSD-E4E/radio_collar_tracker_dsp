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
