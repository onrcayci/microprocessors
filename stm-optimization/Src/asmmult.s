/*
 * asmmult.s
 *
 *  Created on: Sep. 11, 2020
 *      Author: ocayci
 */
.syntax unified

.global asmMultiplication

.section .text.rodata

/**
* void asmMult(float *array1, float *array2, uint32_t size, float *result);
* R0 = pointer to array1
* R1 = pointer to array2
* R2 = size
* R3 = pointer to result
*/

asmMultiplication:
	PUSH		{R4, R5, R6}	// saving R4, R5, R6 according to the calling convention
	MOV			R4, R0			// register R4 points to the beginning of array1
	MOV			R5, R1			// register R5 points to the beginning of array2
	MOV			R6, R3			// register R6 points to the beginning of array result

loop:
	SUBS		R2, R2, #1	// size = size - 1
	BLT			done		// loop finishes when R2 < 0
	VLDR.F32	S0, [R4]	// fp register S0 holds the next element in array1
	VLDR.F32	S1, [R5]	// fp register S1 holds the next element in array2
	VMUL.F32	S2, S0, S1	// multiply elements of array1 and array2
	VSTR.F32	S2, [R6]	// store multiplied result to result array
	ADD			R4, R4, #4	// calculate base address (in R4) for next array element in array1
	ADD			R5, R5, #4	// calculate base address (in R5) for next array element in array2
	ADD			R6, R6, #4	// calculate base address (in R3) for next aray index in result array
	B			loop		// next iteration

done:
	POP			{R4, R5}			// restore context
	BX LR							// return
