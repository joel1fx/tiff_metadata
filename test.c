
/**************************************************************************
The MIT License (MIT)

Copyright (c) 2005-2015 Joel E. Merritt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**************************************************************************/


/**                                                                      **/
/**   Test suite to check conditional swapping functions                 **/
/**                                                                      **/

#include <assert.h>
#include <stdio.h>
#include "tiff_metadata.h"

int main(int argc, char *argv[])
{
	internalStruct test;
	unsigned short us1;
	unsigned short us2;
	unsigned int ui1;
	unsigned int ui2;
	int i1;
	int i2;
	float f1;
	float f2;
	float f3;

	test.machineEndian = detectMachineEndian();

	/* Force no overall swap */
	test.fileEndian = test.machineEndian;

	us1 = 0x1234;
	us2 = cSwapUShort(us1, &test);
	assert(us2 == us1);

	ui1 = 0x12345678;
	ui2 = cSwapUInt(ui1, &test);
	assert(ui2 == ui1);

	i1 = 0x12345678;
	i2 = cSwapInt(i1, &test);
	assert(i2 == i1);

	f1 = 0x12345678;
	f2 = cSwapFloat(f1, &test);
	assert(f2 == f1);

	/* Force overall swap */
	test.fileEndian = test.machineEndian ^ 1;

	us1 = 0x1234;
	us2 = cSwapUShort(us1, &test);
	assert(us2 == 0x3412);

	ui1 = 0x12345678;
	ui2 = cSwapUInt(ui1, &test);
	assert(ui2 == 0x78563412);

	i1 = 0x12345678;
	i2 = cSwapInt(i1, &test);
	assert(i2 == 0x78563412);

	f1 = 0x12345678;
	f2 = cSwapFloat(f1, &test);
	assert(f2 != f1);

	/* Swap the swapped number */
	f3 = cSwapFloat(f2, &test);
	assert(f3 == f1);

	printf("Test completed with no errors.\n");

	return 0;
}

