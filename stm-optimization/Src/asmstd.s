/*
 * asmstd.s
 *
 *  Created on: Sep. 11, 2020
 *      Author: ocayci
 */
.syntax unified

.global asmStd

.section .text.rodata

/**
* void asmStd(&array, uint32_t size, float *result);
*
* R0 = pointer to array
* R1 = size
* R2 = pointer to result
*/

asmStd:
	PUSH			{R3, R4}	// saving R3 and R4 according to calling convention
	MOV				R3, R0		// register R3 points to the start of the array
	MOV				R4, R1		// register R4 holds size
	VSUB.F32		S0, S0, S0	// clear fp register S0

total:
	SUBS			R4, R4, #1	// size = size - 1
	BLT				mean		// loop finishes when R4 < 0
	VLDR.F32		S1, [R3]	// get the next element from the array
	VADD.F32		S0, S0, S1	// sum = sum + array[i]
	ADD				R3, R3, #4	// get the address of the next element in th array
	B				total		// next iteration

mean:
	VMOV			S1, R1		// move integer size to fp register S1
	VCVT.F32.U32	S1, S1		// convert integer size to fp
	VDIV.F32		S0, S0, S1	// mean = sum / size
	MOV				R3, R0		// reset register R3 to the start of the array
	MOV				R4, R1		// reset register R4 to size
	VSUB.F32		S1, S1, S1	// clear fp register S1

sum:
	SUBS			R4, R4, #1	// size = size - 1
	BLT				std			// loop finishes when R4 < 0
	VLDR.F32		S2, [R3]	// fp register S2 hodls the next element in the array
	VSUB.F32		S2, S2, S0	// S2 = array[i] - mean
	VMUL.F32		S2, S2, S2	// S2 = S2 * S2
	VADD.F32		S1, S1, S2	// sum of differences squared, S1 = S1 + S2
	ADD				R3, R3, #4	// get the address of the next element in the array
	B				sum			// next iteration

std:
	SUBS			R4, R1, #1	// R4 = size - 1
	VMOV			S2, R4		// fp register S2 holds integer size - 1
	VCVT.F32.U32	S2, S2		// convert integer size - 1 into fp
	VDIV.F32		S1, S1, S2	// divide sum of differences squared to size - 1
	VSQRT.F32		S1, S1		// take the square root to calculate std

done:
	VSTR.F32		S1, [R2]	// save result into the result pointer
	POP				{R3, R4}	// restore context
	BX LR						// return
