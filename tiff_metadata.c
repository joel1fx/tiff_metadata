
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
#include "tiff_metadata.h"


/**                                                                      **/
/**   Function: detectMachineEndian                                      **/
/**                                                                      **/
/**   Return the endianness of the current system.                       **/
/**   1 == little endian 0 == big_endian                                 **/
/**                                                                      **/
/**   No input parameters                                                **/
/**                                                                      **/

int detectMachineEndian(void)
{
	byte4 tmp;

	tmp.i = 1;

	if(tmp.b[0] == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/**                                                                      **/
/**   Function: cSwapUShort                                              **/
/**                                                                      **/
/**   Return the value of a 2 byte unsigned short. Swap bytes if         **/
/**   necessary to compensate for endian issues.                         **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   a           -- unsigned short value to possibly be swapped         **/
/**   internal    -- struct containing internal program data, including  **/
/**                  machineEndian and fileEndian fields                 **/
/**                  (1 == little endian 0 == big_endian)                **/
/**                                                                      **/

unsigned short cSwapUShort(unsigned short a, struct internalStruct *internal)
{
	int swapBytes;

	swapBytes = internal->machineEndian ^ internal->fileEndian;

	if(swapBytes)
	{
		return ( (a & 0xff00) >> 8) + ( (a & 0xff) << 8);
	}
	else
	{
		return a;
	}
}


/**                                                                      **/
/**   Function: cSwapUInt                                                **/
/**                                                                      **/
/**   Return the value of a 4 byte unsigned int. Swap bytes if           **/
/**   necessary to compensate for endian issues.                         **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   a         -- unsigned int value to possibly be swapped             **/
/**   internal  -- struct containing internal program data, including    **/
/**                machineEndian and fileEndian fields                   **/
/**                                                                      **/

unsigned int cSwapUInt(unsigned int a, internalStruct *internal)
{
	int swapBytes;

	swapBytes = internal->machineEndian ^ internal->fileEndian;

	if(swapBytes)
	{
		return ( (a & 0xff000000) >> 24) + ( (a & 0xff0000) >> 8) +
			( (a & 0xff00) << 8) + ( (a & 0xff) << 24);;
	}
	else
	{
		return a;
	}
}


/**                                                                      **/
/**   Function: cSwapInt                                                 **/
/**                                                                      **/
/**   Return the value of a 4 byte signed int. Swap bytes if             **/
/**   necessary to compensate for endian issues.                         **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   a         -- signed int value to possibly be swapped               **/
/**   internal  -- struct containing internal program data, including    **/
/**                  machineEndian and fileEndian fields                 **/
/**                                                                      **/

int cSwapInt(int a, internalStruct *internal)
{
	int swapBytes;
	byte4 tmp;

	swapBytes = internal->machineEndian ^ internal->fileEndian;

	if(swapBytes)
	{
		tmp.i = a;
		tmp.u = cSwapUInt(tmp.u, internal);
		a = tmp.i;

		return a;
	}
	else
	{
		return a;
	}
}


/**                                                                      **/
/**   Function: cSwapFloat                                               **/
/**                                                                      **/
/**   Return the value of a 4 byte float. Swap bytes if necessary        **/
/**   to compensate for endian issues.                                   **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   a           -- float value to possibly be swapped                  **/
/**   internal    -- struct containing internal program data, including  **/
/**                  machineEndian and fileEndian fields                 **/
/**                                                                      **/

float cSwapFloat(float a, internalStruct *internal)
{
	int swapBytes;
	byte4 tmp;

	swapBytes = internal->machineEndian ^ internal->fileEndian;

	if(swapBytes)
	{
		tmp.f = a;
		tmp.u = cSwapUInt(tmp.u, internal);
		a = tmp.f;

		return a;
	}
	else
	{
		return a;
	}
}


/**                                                                      **/
/**   Function: getTagDescriptor                                         **/
/**                                                                      **/
/**   Copy the Tag descriptor of the given tag number into the given     **/
/**   buffer, or "unknown" if unknown Tag descriptor.                    **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   buffer  -- buffer to which to copy Tag descriptor                  **/
/**   tag     -- tag number                                              **/
/**                                                                      **/
/**                                                                      **/

void getTagDescriptor(char *buffer, unsigned short tag)
{
	int i;
	int match;
	tagString desc[] = {
		{ 254, "NewSubfileType" },
		{ 255, "SubfileType" },
		{ 256, "ImageWidth" },
		{ 257, "ImageLength" },
		{ 258, "BitsPerSample" },
		{ 259, "Compression" },
		{ 262, "PhotometricInterpretation" },
		{ 263, "Threshholding" },
		{ 264, "CellWidth" },
		{ 265, "CellLength" },
		{ 266, "FillOrder" },
		{ 269, "DocumentName" },
		{ 270, "ImageDescription" },
		{ 271, "Make" },
		{ 272, "Model" },
		{ 273, "StripOffsets" },
		{ 274, "Orientation" },
		{ 277, "SamplesPerPixel" },
		{ 278, "RowsPerStrip" },
		{ 279, "StripByteCounts" },
		{ 280, "MinSampleValue" },
		{ 281, "MaxSampleValue" },
		{ 282, "XResolution" },
		{ 283, "YResolution" },
		{ 284, "PlanarConfiguration" },
		{ 286, "XPosition" },
		{ 287, "YPosition" },
		{ 288, "FreeOffsets" },
		{ 289, "FreeByteCounts" },
		{ 290, "GrayResponseUnit" },
		{ 291, "GrayResponseCurve" },
		{ 292, "T4Options" },
		{ 293, "T6Options" },
		{ 296, "ResolutionUnit" },
		{ 297, "PageNumber" },
		{ 301, "TransferFunction" },
		{ 305, "Software" },
		{ 306, "DateTime" },
		{ 315, "Artist" },
		{ 316, "HostComputer" },
		{ 317, "Predictor" },
		{ 318, "WhitePoint" },
		{ 319, "PrimaryChromaticities" },
		{ 320, "ColorMap" },
		{ 321, "HalftoneHints" },
		{ 322, "TileWidth" },
		{ 323, "TileHeight" },
		{ 324, "TileOffsets" },
		{ 325, "TileByteCounts" },
		{ 332, "InkSet" },
		{ 333, "InkNames" },
		{ 334, "NumberOfInks" },
		{ 336, "DotRange" },
		{ 337, "TargetPrinter" },
		{ 338, "ExtraSamples" },
		{ 339, "SampleFormat" },
		{ 340, "SMinSampleValue" },
		{ 341, "SMaxSampleValue" },
		{ 342, "TransferRange" },
		{ 512, "JPEGProc" },
		{ 513, "JPEGInterchangeFormat" },
		{ 514, "JPEGInterchangeFormatLength" },
		{ 515, "JPEGRestartInterval" },
		{ 517, "JPEGLosslessPredictors" },
		{ 518, "JPEGPointTransforms" },
		{ 519, "JPEGQTables" },
		{ 520, "JPEGDCTables" },
		{ 521, "JPEGACTables" },
		{ 529, "YCbCrCoefficients" },
		{ 530, "YCbCrSubSampling" },
		{ 531, "YCbCrPositioning" },
		{ 532, "ReferenceBlackWhite" },
		{ 33434, "ExposureTime" },
		{ 33437, "FNumber" },
		{ 34665, "ExifIFDPointer" },
		{ 34434, "ExposureTime" },
		{ 34850, "ExposureProgram" },
		{ 34852, "SpectralSensitivity" },
		{ 34855, "ISOSpeedRatings" },
		{ 34856, "OECF" },
		{ 36864, "ExifVersion" },
		{ 36867, "DateTimeOriginal" },
		{ 36868, "DateTimeDigitized" },
		{ 37121, "ComponentsConfiguration" },
		{ 37122, "CompressedBitsPerPixel" },
		{ 37377, "ShutterSpeedValue" },
		{ 37378, "ApertureValue" },
		{ 37379, "BrightnessValue" },
		{ 37380, "ExposureBiasValue" },
		{ 37381, "MaxApertureValue" },
		{ 37382, "SubjectDistance" },
		{ 37383, "MeteringMode" },
		{ 37384, "LightSource" },
		{ 37385, "Flash" },
		{ 37386, "FocalLength" },
		{ 37500, "MakerNote" },
		{ 37510, "UserComment" },
		{ 37520, "SubSecTime" },
		{ 37521, "SubSecTimeOriginal" },
		{ 37522, "SubSecTimeDigitized" },
		{ 40960, "FlashpixVersion" },
		{ 40961, "ColorSpace" },
		{ 40962, "PixelXDimension" },
		{ 40963, "PixelYDimension" },
		{ 40964, "RelatedSoundFile" },
		{ 41483, "FlashEnergy" },
		{ 41484, "SpatialFrequencyResponse" },
		{ 41486, "FocalPlaneXResolution" },
		{ 41487, "FocalPlaneYResolution" },
		{ 41488, "FocalPlaneResolutionUnit" },
		{ 41492, "SubjectLocation" },
		{ 41493, "ExposureIndex" },
		{ 41495, "SensingMethod" },
		{ 41728, "FileSource" },
		{ 41729, "SceneType" },
		{ 41730, "CFAPattern" },
		{ 41985, "CustomRendered" },
		{ 41986, "ExposureMode" },
		{ 41987, "WhiteBalance" },
		{ 41988, "DigitalZoomRatio" },
		{ 41989, "FocalLengthIn35mmFilm" },
		{ 41990, "SceneCaptureType" },
		{ 41991, "GainControl" },
		{ 42016, "ImageUniqueID" },
		{ -1, "" }  /* End of array element.  Do not delete. */
		};

	match = 0;

	/* TODO: Add more efficient search algorithm. */

	for(i = 0;desc[i].tag != -1;i++)
	{
		if(desc[i].tag == (int)tag)
		{
			strcpy(buffer, desc[i].string);
			match = 1;
			break;
		}
	}
	if(match == 0)
	{
		strcpy(buffer, "unknown");
	}

	return;
}


/**                                                                      **/
/**   Function: getTIFFValueDesc                                         **/
/**                                                                      **/
/**   Copy the Value descriptor of the given tag and possibly value      **/
/**   numbers and into the given buffer, or "unknown" if unknown Tag     **/
/**   descriptor.                                                        **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   buffer  -- buffer to which to copy Tag descriptor                  **/
/**   tag     -- tag number                                              **/
/**   value   -- value number                                            **/
/**                                                                      **/

void getTIFFValueDesc(char *buffer, unsigned short tag, unsigned int value)
{
	/* TODO: Organize the buffer lookup in a less naive way. Note  */
	/* that any improvement would need to account both for buffers */
	/* affected only by the tag and buffers affected by both the   */
        /* tag and value.                                              */

	switch(tag)
	{
		case 254: /* NewSubfileType */
		{
			strcpy(buffer, "");
			break;
		}
		case 256: /* ImageWidth */
		{
			strcpy(buffer, "pixels");
			break;
		}
		case 257: /* ImageLength */
		{
			strcpy(buffer, "pixels");
			break;
		}
		case 258: /* BitsPerSample */
		{
			strcpy(buffer, "");
			break;
		}
		case 259: /* Compression */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "No compression");
					break;
				}
				case 2:
				{
					strcpy(buffer,
						"CCITT Group 3 compression");
					break;
				}
				case 5:
				{
					strcpy(buffer, "LZW compression");
					break;
				}
				case 32773:
				{
					strcpy(buffer, "PackBits compression");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 262: /* PhotometricInterpretation */
		{
			switch(value)
			{
				case 0:
				{
					strcpy(buffer, "WhiteIsZero");
					break;
				}
				case 1:
				{
					strcpy(buffer, "BlackIsZero");
					break;
				}
				case 2:
				{
					strcpy(buffer, "RGB");
					break;
				}
				case 3:
				{
					strcpy(buffer, "Palette color");
					break;
				}
				case 4:
				{
					strcpy(buffer, "Transparency mask");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 273: /* StripOffsets */
		{
			strcpy(buffer, "");
			break;
		}
		case 274: /* Orientation */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "Row0:top,Col0:left");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Row0:top,Col0:right");
					break;
				}
				case 3:
				{
					strcpy(buffer,
						"Row0:bottom,Col0:right");
					break;
				}
				case 4:
				{
					strcpy(buffer,
						"Row0:bottom,Col0:left");
					break;
				}
				case 5:
				{
					strcpy(buffer, "Row0:left,Col0:top");
					break;
				}
				case 6:
				{
					strcpy(buffer, "Row0:right,Col0:top");
					break;
				}
				case 7:
				{
					strcpy(buffer,
						"Row0:right,Col0:bottom");
					break;
				}
				case 8:
				{
					strcpy(buffer,
						"Row0:left,Col0:bottom");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 277: /* SamplesPerPixel */
		{
			strcpy(buffer, "");
			break;
		}
		case 278: /* RowsPerStrip */
		{
			strcpy(buffer, "");
			break;
		}
		case 279: /* StripByteCounts */
		{
			strcpy(buffer, "");
			break;
		}
		case 284: /* PlanarConfiguration */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "Chunky");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Planar");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 296: /* ResolutionUnit */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "No absolute unit");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Inch");
					break;
				}
				case 3:
				{
					strcpy(buffer, "Centimeter");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 305: /* Software */
		{
			strcpy(buffer, "");
			break;
		}
		case 317: /* Predictor */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "No prediction scheme");
					break;
				}
				case 2:
				{
					strcpy(buffer,
						"Horizontal differencing");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 339: /* SampleFormat */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer,
						"Unsigned integer data");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Two's compliment "
						"signed integer data");
					break;
				}
				case 3:
				{
					strcpy(buffer, "IEEE floating point "
						"data");
					break;
				}
				case 4:
				{
					strcpy(buffer,
						"Undefined data format");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 33434: /* Exif ExposureTime */
		{
			strcpy(buffer, "seconds");
			break;
		}
		case 34850: /* Exif ExposureProgram */
		{
			switch(value)
			{
				case 0:
				{
					strcpy(buffer, "Not defined");
					break;
				}
				case 1:
				{
					strcpy(buffer, "Manual");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Normal program");
					break;
				}
				case 3:
				{
					strcpy(buffer, "Aperture priority");
					break;
				}
				case 4:
				{
					strcpy(buffer, "Shutter priority");
					break;
				}
				case 5:
				{
					strcpy(buffer, "Creative program");
					break;
				}
				case 6:
				{
					strcpy(buffer, "Action program");
					break;
				}
				case 7:
				{
					strcpy(buffer, "Portrait mode");
					break;
				}
				case 8:
				{
					strcpy(buffer, "Landscape mode");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		case 41488: /* Exif FocalPlaneResolutionUnit */
		{
			switch(value)
			{
				case 1:
				{
					strcpy(buffer, "No absolute unit");
					break;
				}
				case 2:
				{
					strcpy(buffer, "Inch");
					break;
				}
				case 3:
				{
					strcpy(buffer, "Centimeter");
					break;
				}
				default:
				{
					strcpy(buffer, "");
					break;
				}
			}
			break;
		}
		default:
		{
			strcpy(buffer, "unknown");
			break;
		}
	}
}


/**                                                                      **/
/**   Function: getFieldTypeNumBytes                                     **/
/**                                                                      **/
/**   Return the number of bytes contained in the given Type of an       **/
/**   Image File Directory (IFD) entry.                                  **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   fieldType  -- the Type number of an Image File Directory (IFD)     **/
/**                 entry.                                               **/
/**                                                                      **/

int getFieldTypeNumBytes(unsigned short fieldType)
{
	int numBytes[] = { 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8 };

	if( (fieldType >= 1) && (fieldType <= 12) )
	{
		return numBytes[fieldType - 1];
	}
	else
	{
		return 0;
	}
}


/**                                                                      **/
/**   Function: getTIFFTypeDesc                                          **/
/**                                                                      **/
/**   Copy the name of the given Type of an Image File Directory (IFD)   **/
/**   entry into the given buffer.                                       **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   buffer     -- buffer to which to copy name                         **/
/**   fieldType  -- the Type number of an Image File Directory (IFD)     **/
/**                 entry.                                               **/
/**                                                                      **/

void getTIFFTypeDesc(char *buffer, unsigned short fieldType)
{
	char *typeDesc[] = { "BYTE", "ASCII", "SHORT", "LONG", "RATIONAL",
		"SBYTE", "UNDEFINED", "SSHORT", "SLONG", "SRATIONAL",
		"FLOAT", "DOUBLE" };

	if( (fieldType >= 1) && (fieldType <= 12) )
	{
		strcpy(buffer, typeDesc[fieldType - 1]);
	}
	else
	{
		strcpy(buffer, "unknown");
	}

	return;
}


/**                                                                      **/
/**   Function: printEntry                                               **/
/**                                                                      **/
/**   Copy the name of the given Type of an Image File Directory (IFD)   **/
/**   entry into the given buffer.                                       **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   buffer     -- buffer containing entry value                        **/
/**   tag        -- tag number                                           **/
/**   fieldType  -- the Type number of an Image File Directory (IFD)     **/
/**                 entry.                                               **/
/**   internal   -- struct containing internal program data, including   **/
/**                 machineEndian and fileEndian fields                  **/
/**                                                                      **/

void printEntry(unsigned char *buffer, unsigned short tag,
	unsigned short fieldType, internalStruct *internal)
{
	byte4 tmp;
	char buffer2[1024];
	unsigned int numerator, denominator;
	int snumerator, sdenominator;
	double result;

	switch(fieldType)
	{
		case 2: /* ASCII */
		{
			if( (buffer[0] > 31) && (buffer[0] < 128) )
			{
				printf("Value '%c'\n", buffer[0]);
			}
			else
			{
				printf("Value %d\n", buffer[0]);
			}

			break;
		}
		case 3: /* SHORT */
		{
			tmp.b[0] = buffer[0];
			tmp.b[1] = buffer[1];

			tmp.s = cSwapUShort(tmp.s, internal);

			getTIFFValueDesc(buffer2, tag, (unsigned int)tmp.s);
			printf("Value %d %s\n", tmp.s, buffer2);

			break;
		}
		case 4: /* LONG */
		{
			tmp.b[0] = buffer[0];
			tmp.b[1] = buffer[1];
			tmp.b[2] = buffer[2];
			tmp.b[3] = buffer[3];

			tmp.u = cSwapUInt(tmp.u, internal);

			getTIFFValueDesc(buffer2, tag, tmp.u);
			printf("Value %d %s\n", tmp.u, buffer2);

			break;
		}
		case 5: /* RATIONAL */
		{
			tmp.b[0] = buffer[0];
			tmp.b[1] = buffer[1];
			tmp.b[2] = buffer[2];
			tmp.b[3] = buffer[3];
			numerator = cSwapUInt(tmp.u, internal);
			tmp.b[0] = buffer[4];
			tmp.b[1] = buffer[5];
			tmp.b[2] = buffer[6];
			tmp.b[3] = buffer[7];
			denominator = cSwapUInt(tmp.u, internal);

			result = (double)numerator/(double)denominator;

			printf("Value (%d/%d) %lf\n", numerator, denominator,
				result);

			break;
		}
		case 10: /* SRATIONAL */
		{
			tmp.b[0] = buffer[0];
			tmp.b[1] = buffer[1];
			tmp.b[2] = buffer[2];
			tmp.b[3] = buffer[3];
			snumerator = cSwapInt(tmp.u, internal);
			tmp.b[0] = buffer[4];
			tmp.b[1] = buffer[5];
			tmp.b[2] = buffer[6];
			tmp.b[3] = buffer[7];
			sdenominator = cSwapInt(tmp.u, internal);

			result = (double)snumerator/(double)sdenominator;

			printf("Value (%d/%d) %lf\n", snumerator, sdenominator,
				result);

			break;
		}
		default:
		{
			printf("Value 0x%x\n", buffer[0]);
			break;
		}
	}

	return;
}


/**                                                                      **/
/**   Function: printDump                                                **/
/**                                                                      **/
/**   Print a hexadecimal dump of the first count bytes of buffer        **/
/**   buffer in canonical hex+ASCII format.                              **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   buffer  -- unsigned char buffer to be dumped                       **/
/**   count   -- number of bytes to dump                                 **/
/**                                                                      **/

void printDump(unsigned char *buffer, int count)
{
	int i, j;
	int i2;
	int bytesPerLine = 16;

	for(i = 0;i < count;i++)
	{
		if( (i % bytesPerLine) == 0)
		{
			printf("%08x  %02x ", i, buffer[i]);
		}
		else if( (i % bytesPerLine) == (bytesPerLine - 1) )
		{
			printf("%02x  |", buffer[i]);
			for(j = i - (bytesPerLine - 1);j <= i;j++)
			{
				if( (buffer[j] > 31) && (buffer[j] < 128) )
				{
					printf("%c", buffer[j]);
				}
				else
				{
					printf(".");
				}
			}
			printf("|\n");
		}
		else
		{
			printf("%02x ", buffer[i]);
		}
	}
	if( (i % bytesPerLine) != 0)
	{
		i2 = i;
		for(i2 = i;(i2 % bytesPerLine) != (bytesPerLine - 1);i2++)
		{
			printf("   ");
		}
		printf("    |");
		i2 -= (bytesPerLine - 1);
		for(j = i2;j < i;j++)
		{
			if( (buffer[j] > 31) && (buffer[j] < 128) )
			{
				printf("%c", buffer[j]);
			}
			else
			{
				printf(".");
			}
		}
		printf("|\n");
	}
	printf("%08x\n", i);

	return;
}


/**                                                                      **/
/**  Function: getOffsetValues                                           **/
/**                                                                      **/
/**  file         -- FILE pointer                                        **/
/**  tag          -- tag number                                          **/
/**  fieldType    -- the Type number of an Image File Directory (IFD)    **/
/**                  entry.                                              **/
/**  count        -- number of items in this entry                       **/
/**  valueOffset  -- value or offset                                     **/
/**  internal     -- struct containing internal program data, including  **/
/**                  machineEndian, fileEndian, and tiffOffset fields    **/
/**                                                                      **/

void getOffsetValues(FILE *file, unsigned short tag,
	unsigned short fieldType, unsigned int count,
	unsigned int valueOffset, internalStruct *internal)
{
	int i;
	int cur_pos;
	int entry_bytes;
	unsigned char *buffer;

	cur_pos = ftell(file);

	fseek(file, (int)(valueOffset + internal->tiffOffset), SEEK_SET);

	if(fieldType == 2) /* ASCII */
	{
		buffer = (unsigned char *)malloc(count);
		if(buffer == NULL)
		{
			fprintf(stderr, "can't alloc buffer\n");
			exit(1);
		}
		if(fread(buffer, 1, count, file) != count)
		{
			fprintf(stderr, "can't read ASCII entry\n");
			exit(1);
		}
		printf("\t  String \"%s\"\n", buffer);
	}
	else if(fieldType == 7) /* UNDEFINED */
	{
		buffer = (unsigned char *)malloc(count);
		if(buffer == NULL)
		{
			fprintf(stderr, "can't alloc buffer\n");
			exit(1);
		}
		if(fread(buffer, 1, count, file) != count)
		{
			fprintf(stderr, "can't read UNDEFINED entry\n");
			exit(1);
		}
		printDump(buffer, count);
	}
	else /* all other types */
	{
		entry_bytes = getFieldTypeNumBytes(fieldType);
		buffer = (unsigned char *)malloc(entry_bytes);
		if(buffer == NULL)
		{
			fprintf(stderr, "can't alloc buffer\n");
			exit(1);
		}

		for(i = 0;i < count;i++)
		{
			if(fread(buffer, entry_bytes, 1, file) != 1)
			{
				fprintf(stderr, "can't read offset entry\n");
				exit(1);
			}
			printf("\t  %d ", i);
			printEntry(buffer, tag, fieldType, internal);
		}
	}

	/*  Reposition file position indicator to value at start of  */
	/*  function.  */
	fseek(file, (int)cur_pos, SEEK_SET);

	free(buffer);

	return;
}


/**                                                                      **/
/**  Function: tiffIFDPrint                                              **/
/**                                                                      **/
/**  Print the entries of a TIFF or an Exif IFD.                         **/
/**                                                                      **/
/**  Input parameters:                                                   **/
/**  filename  -- file name                                              **/
/**  file      -- FILE pointer                                           **/
/**  internal  -- struct containing internal program data, including     **/
/**               fileEndian field                                       **/
/**                                                                      **/

void tiffIFDPrint(char *filename, FILE *file, internalStruct *internal)
{
	int i;
	unsigned short tiffIFDEntries;
	struct tiffIFDEntry ifd_entry;
	int total_bytes;
	char buffer[1024];
	unsigned int value;
	byte4 tmp;

	while(internal->tiffIFDOffset != 0)
	{
		fseek(file, (internal->tiffIFDOffset + internal->tiffOffset),
			SEEK_SET);

		if(fread(&tiffIFDEntries, sizeof(tiffIFDEntries), 1, file) !=
			1)
		{
			fprintf(stderr,
				"can't read number of IFD entries of %s\n",
				filename);
			exit(1);
		}

		tiffIFDEntries = cSwapUShort(tiffIFDEntries, internal);
		printf("number of IFD entries %d\n", tiffIFDEntries);

		for(i = 0;i < tiffIFDEntries;i++)
		{
			printf("\nIFD entry %d\n", i + 1);
			if(fread(&ifd_entry, sizeof(ifd_entry), 1, file) != 1)
			{
				fprintf(stderr, "can't IFD entry of %s\n",
					filename);
				exit(1);
			}
			ifd_entry.tag = cSwapUShort(ifd_entry.tag, internal);
			getTagDescriptor(buffer, ifd_entry.tag);
			printf("\tTag %d  (%04X.H)   %s\n", ifd_entry.tag,
				ifd_entry.tag, buffer);
			ifd_entry.fieldType = cSwapUShort(ifd_entry.fieldType,
				internal);

			getTIFFTypeDesc(buffer, ifd_entry.fieldType);
			printf("\tType %d %s\n", ifd_entry.fieldType, buffer);

			ifd_entry.count = cSwapUInt(ifd_entry.count, internal);
			printf("\tCount %d\n", ifd_entry.count);

			total_bytes = getFieldTypeNumBytes(ifd_entry.fieldType)
				* ifd_entry.count;
			if(total_bytes > 4)
			{
				ifd_entry.valueOffset =
					cSwapUInt(ifd_entry.valueOffset,
						internal);
				printf("\tOffset %d\n", ifd_entry.valueOffset);
				getOffsetValues(file, ifd_entry.tag,
					ifd_entry.fieldType,
					ifd_entry.count,
					ifd_entry.valueOffset, internal);
			}
			else
			{
				tmp.u = ifd_entry.valueOffset;
				if(ifd_entry.fieldType == 3) /* SHORT */
				{
					value = (unsigned int)
						cSwapUShort(tmp.s, internal);
				}
				else if(ifd_entry.fieldType == 4) /* LONG */
				{
					value = cSwapUInt(
						ifd_entry.valueOffset,
						internal);
					if(ifd_entry.tag == 0x8769)
					{
						internal->exifHeader = 1;
						internal->exifIFDOffset =
							value;
					}
				}
				else if(ifd_entry.fieldType == 7) /* UNDEF */
				{
					printDump( (unsigned char *)tmp.b,
						total_bytes);
				}
				else
				{
					value = cSwapUInt(tmp.u, internal);
				}

				if(ifd_entry.fieldType != 7)
				{
					getTIFFValueDesc(buffer,
						ifd_entry.tag,
						value);
					printf("\tValue %d %s\n", value,
						buffer);
				}
			}


		}
		printf("\n");

		if(fread(&(internal->tiffIFDOffset),
			sizeof(internal->tiffIFDOffset), 1, file) != 1)
		{
			fprintf(stderr, "can't read next IFD offset in %s\n",
				filename);
			exit(1);
		}

		internal->tiffIFDOffset = cSwapUInt(internal->tiffIFDOffset,
			internal);
		if(internal->tiffIFDOffset == 0)
		{
			printf("End of IFD list\n");
		}
		else
		{
			printf("next IFD offset %d\n",
				internal->tiffIFDOffset);
		}
	}

	return;
}


/**                                                                      **/
/**   Function: tiffMetadataPrint                                        **/
/**                                                                      **/
/**   Print the metadata of a TIFF file or a JPEG file with an Exif      **/
/**   header.                                                            **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   filename  -- file name                                             **/
/**                                                                      **/

int tiffMetadataPrint(char *filename)
{
	FILE *file;
	struct tiffImageFileHeader tiff_hdr;
	unsigned char buffer[1024];
	struct internalStruct internal;

	internal.machineEndian = detectMachineEndian();

#if DEBUG
	if(internal.machineEndian == 1)
	{
		printf("This machine has little-endian architecture.\n");
	}
	else
	{
		printf("This machine has big-endian architecture.\n");
	}
#endif

	file = fopen(filename, "r");
	if(file == NULL)
	{
		fprintf(stderr, "can't open %s to read\n", filename);

		return 1;
	}

	if(fread(buffer, 1, 128, file) != 128)
	{
		fprintf(stderr, "can't read header of %s\n", filename);

		return 1;
	}

	internal.tiffOffset = 0;
	if( (buffer[0] == 0xff) && (buffer[1] == 0xd8) )
	{
		printf("JPEG file\n");

		if( (buffer[2] == 0xff) && (buffer[3] == 0xe1) &&
			(buffer[6] == 'E') && (buffer[7] == 'x') &&
			(buffer[8] == 'i') && (buffer[9] == 'f') )
		{
			internal.tiffOffset = 12;
		}
		else
		{
			fprintf(stderr, "can't find Exif header\n");

			return 1;
		}
	}

	if( (buffer[internal.tiffOffset] == 'I') &&
		(buffer[internal.tiffOffset + 1] == 'I') )
	{
		internal.fileEndian = 1;
		printf("Intel (little-endian) byte order\n");
	}
	else if( (buffer[internal.tiffOffset] == 'M') &&
		(buffer[internal.tiffOffset + 1] == 'M') )
	{
		internal.fileEndian = 0;
		printf("Motorola (big-endian) byte order\n");
	}
	else
	{
		fprintf(stderr, "unsupported file type\n");

		return 1;
	}

	fseek(file, internal.tiffOffset, SEEK_SET);

	if(fread(&tiff_hdr, sizeof(struct tiffImageFileHeader), 1, file) != 1)
	{
		fprintf(stderr, "can't read header of %s\n", filename);

		return 1;
	}

	internal.exifHeader = 0;

	tiff_hdr.magic = cSwapUShort(tiff_hdr.magic, &internal);
	if(tiff_hdr.magic != TIFF_MAGIC)
	{
		fprintf(stderr, "bad magic number 0x%x -- exiting\n",
			tiff_hdr.magic);

		return 1;
	}
	printf("Magic %d\n", tiff_hdr.magic);

	tiff_hdr.ifd_offset = cSwapUInt(tiff_hdr.ifd_offset, &internal);
	printf("IFD offset %d\n", tiff_hdr.ifd_offset);

	internal.tiffIFDOffset = tiff_hdr.ifd_offset;
	tiffIFDPrint(filename, file, &internal);

	/* If the first IFD contains an Exif header, print that too */
	if(internal.exifHeader == 1)
	{
		internal.tiffIFDOffset = internal.exifIFDOffset;

		printf("\nExif header\n");
		tiffIFDPrint(filename, file, &internal);
	}

	fclose(file);

	return 0;
}


