
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
/**   The TIFF file format specification is available at                 **/
/**                                                                      **/
/**   http://partners.adobe.com/public/developer/en/tiff/TIFF6.pdf       **/
/**                                                                      **/

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


# define TIFF_MAGIC 42


/**                                                                      **/
/**  TIFF Image File Header                                              **/
/**                                                                      **/

struct tiffImageFileHeader
{
	unsigned short byteOrder;
	unsigned short magic;
	unsigned int ifd_offset;
};


/**                                                                      **/
/**  TIFF Image File Directory (IFD) entry (i.e. metadata item)          **/
/**                                                                      **/

struct tiffIFDEntry
{
	unsigned short tag;
	unsigned short fieldType;
	unsigned int count;
	unsigned int valueOffset;
};


/**                                                                      **/
/**  Internal image structure for better parameter passing               **/
/**                                                                      **/
/**  machineEndian                                                       **/
/**      defines machine architecture. 1 == little endian                **/
/**                                    0 == big_endian                   **/
/**      for more info http://en.wikipedia.org/wiki/Endianness           **/
/**  fileEndian                                                          **/
/**      defines file architecture. 1 == little endian                   **/
/**                                 0 == big_endian                      **/
/**  exifHeader                                                          **/
/**      defines whether this IFD is an Exif IFD                         **/
/**  exifIFDOffset                                                       **/
/**      offset of Exif IFD from file start                              **/
/**  tiffOffset                                                          **/
/**      offset of TIFF file header from file start                      **/
/**  tiffIFDOffset                                                       **/
/**      offset of TIFF IFD from file start                              **/
/**                                                                      **/

typedef struct internalStruct
{
	int machineEndian;
	int fileEndian;
	long tiffOffset;
	unsigned int tiffIFDOffset;
	int exifHeader;
	unsigned int exifIFDOffset;
} internalStruct;


/**                                                                      **/
/**  Utility structure for conversion from raw bytes to chars, ints,     **/
/**  floats, etc.                                                        **/
/**                                                                      **/

typedef union byte4
{
	char b[4];
	int i;
	unsigned int u;
	unsigned short s;
	float f;
} byte4;


/**                                                                      **/
/**  TIFF tag and description string structure                           **/
/**                                                                      **/

typedef struct tagString
{
	int tag;
	char *string;
} tagString;


/**                                                                      **/
/**  Library API function declarations                                   **/
/**                                                                      **/

int tiffMetadataPrint(char *filename);
int detectMachineEndian(void);
unsigned short cSwapUShort(unsigned short a, struct internalStruct *internal);
unsigned int cSwapUInt(unsigned int a, internalStruct *internal);
int cSwapInt(int a, internalStruct *internal);
float cSwapFloat(float a, internalStruct *internal);
