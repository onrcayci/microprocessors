/*
 * cmult.c
 *
 *  Created on: Sep. 11, 2020
 *      Author: ocayci
 */
#include "main.h"
#include "lab1math.h"

void cMultiplication(float *array1, float *array2, uint32_t size, float *result) {
	for (uint32_t i=0; i<size; i++) {
		result[i] = array1[i] * array2[i];
	}
}
