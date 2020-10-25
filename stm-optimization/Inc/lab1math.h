/*
 * lab1math.h
 *
 *  Created on: Sep. 8, 2020
 *      Author: ocayci
 */
#include "main.h"

#ifndef INC_LAB1MATH_H_
#define INC_LAB1MATH_H_

void cMax(float *array, uint32_t size, float *max, uint32_t *maxIndex);

void cMultiplication(float *array1, float *array2, uint32_t size, float *result);

void cStd(float *array, uint32_t size, float *result);

extern void asmMax(float *array, uint32_t size, float *max, uint32_t *maxIndex);

extern void asmMultiplication(float *array1, float *array2, uint32_t size, float *result);

extern void asmStd(float *array, uint32_t size, float *result);

#endif /* INC_LAB1MATH_H_ */
