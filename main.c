
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
/**   The Exif file format specifcations are available at                **/
/**                                                                      **/
/**   http://www.exiv2.org/Exif2-2.PDF                                   **/
/**   http://www.cipa.jp/std/documents/e/DC-008-2012_E.pdf               **/
/**                                                                      **/


#define MAIN

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "tiff_metadata.h"

/**                                                                      **/
/**   Function: main.                                                    **/
/**                                                                      **/
/**   Program main function.                                             **/
/**                                                                      **/
/**   Usage:                                                             **/
/**   tiff_metadata tiffFile                                             **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   argc       -- argument count                                       **/
/**   argv       -- argument vector                                      **/
/**                                                                      **/

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "usage: %s tiffFile\n", argv[0]);

		return 1;
	}

	return tiffMetadataPrint(argv[1]);
}

