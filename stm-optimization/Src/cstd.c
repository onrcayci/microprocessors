/*
 * cstd.c
 *
 *  Created on: Sep. 11, 2020
 *      Author: ocayci
 */
#include <math.h>

#include "main.h"
#include "lab1math.h"

void cStd(float *array, uint32_t size, float *result) {
	float sum = 0;
	float middleSum = 0;
	for (uint32_t i=0; i<size; i++) {
		sum += array[i];
	} // for
	float mean = sum / size;
	for (uint32_t i=0; i<size; i++) {
		middleSum += (array[i] - mean) * (array[i] - mean);
	}
	(*result) = sqrtf(middleSum/(size-1));
} // cStd
