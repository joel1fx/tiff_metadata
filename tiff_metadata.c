
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


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "tiff_metadata.h"

/* field types */
typedef enum {
	FT_UNKNOWN = 0,
	FT_BYTE,
	FT_ASCII,
	FT_SHORT,
	FT_LONG,
	FT_RATIONAL,
	FT_SBYTE,
	FT_UNDEFINED,
	FT_SSHORT,
	FT_SLONG,
	FT_SRATIONAL,
	FT_FLOAT,
	FT_DOUBLE,
	FT_MIN = FT_BYTE,
	FT_MAX = FT_DOUBLE,
} fieldType_t;

/* field type details lookup table entry*/
typedef struct {
	fieldType_t type;
	const char *desc;
	size_t numBytes;
} fieldTypeData_t;

/* field type details lookup table */
static const fieldTypeData_t fieldTypeLookup[] = {
	{ FT_BYTE, "BYTE", 1, },
	{ FT_ASCII, "ASCII", 1, },
	{ FT_SHORT, "SHORT", 2, },
	{ FT_LONG, "LONG", 4, },
	{ FT_RATIONAL, "RATIONAL", 8, },
	{ FT_SBYTE, "SBYTE", 1, },
	{ FT_UNDEFINED, "UNDEFINED", 1, },
	{ FT_SSHORT, "SSHORT", 2, },
	{ FT_SLONG, "SLONG", 4, },
	{ FT_SRATIONAL, "SRATIONAL", 8, },
	{ FT_FLOAT, "FLOAT", 4, },
	{ FT_DOUBLE, "DOUBLE", 8, },
};

/* tag numbers */
typedef enum {
	NewSubfileType = 254,
	SubfileType = 255,
	ImageWidth = 256,
	ImageLength = 257,
	BitsPerSample = 258,
	Compression = 259,
	PhotometricInterpretation = 262,
	Threshholding = 263,
	CellWidth = 264,
	CellLength = 265,
	FillOrder = 266,
	DocumentName = 269,
	ImageDescription = 270,
	Make = 271,
	Model = 272,
	StripOffsets = 273,
	Orientation = 274,
	SamplesPerPixel = 277,
	RowsPerStrip = 278,
	StripByteCounts = 279,
	MinSampleValue = 280,
	MaxSampleValue = 281,
	XResolution = 282,
	YResolution = 283,
	PlanarConfiguration = 284,
	XPosition = 286,
	YPosition = 287,
	FreeOffsets = 288,
	FreeByteCounts = 289,
	GrayResponseUnit = 290,
	GrayResponseCurve = 291,
	T4Options = 292,
	T6Options = 293,
	ResolutionUnit = 296,
	PageNumber = 297,
	TransferFunction = 301,
	Software = 305,
	DateTime = 306,
	Artist = 315,
	HostComputer = 316,
	Predictor = 317,
	WhitePoint = 318,
	PrimaryChromaticities = 319,
	ColorMap = 320,
	HalftoneHints = 321,
	TileWidth = 322,
	TileHeight = 323,
	TileOffsets = 324,
	TileByteCounts = 325,
	InkSet = 332,
	InkNames = 333,
	NumberOfInks = 334,
	DotRange = 336,
	TargetPrinter = 337,
	ExtraSamples = 338,
	SampleFormat = 339,
	SMinSampleValue = 340,
	SMaxSampleValue = 341,
	TransferRange = 342,
	JPEGProc = 512,
	JPEGInterchangeFormat = 513,
	JPEGInterchangeFormatLength = 514,
	JPEGRestartInterval = 515,
	JPEGLosslessPredictors = 517,
	JPEGPointTransforms = 518,
	JPEGQTables = 519,
	JPEGDCTables = 520,
	JPEGACTables = 521,
	YCbCrCoefficients = 529,
	YCbCrSubSampling = 530,
	YCbCrPositioning = 531,
	ReferenceBlackWhite = 532,
	ExposureTime = 33434,
	FNumber = 33437,
	ExifIFDPointer = 34665,
	ExposureTime2 = 34434,
	ExposureProgram = 34850,
	SpectralSensitivity = 34852,
	ISOSpeedRatings = 34855,
	OECF = 34856,
	ExifVersion = 36864,
	DateTimeOriginal = 36867,
	DateTimeDigitized = 36868,
	ComponentsConfiguration = 37121,
	CompressedBitsPerPixel = 37122,
	ShutterSpeedValue = 37377,
	ApertureValue = 37378,
	BrightnessValue = 37379,
	ExposureBiasValue = 37380,
	MaxApertureValue = 37381,
	SubjectDistance = 37382,
	MeteringMode = 37383,
	LightSource = 37384,
	Flash = 37385,
	FocalLength = 37386,
	SubjectArea = 37396,
	MakerNote = 37500,
	UserComment = 37510,
	SubSecTime = 37520,
	SubSecTimeOriginal = 37521,
	SubSecTimeDigitized = 37522,
	FlashpixVersion = 40960,
	ColorSpace = 40961,
	PixelXDimension = 40962,
	PixelYDimension = 40963,
	RelatedSoundFile = 40964,
	FlashEnergy = 41483,
	SpatialFrequencyResponse = 41484,
	FocalPlaneXResolution = 41486,
	FocalPlaneYResolution = 41487,
	FocalPlaneResolutionUnit = 41488,
	SubjectLocation = 41492,
	ExposureIndex = 41493,
	SensingMethod = 41495,
	FileSource = 41728,
	SceneType = 41729,
	CFAPattern = 41730,
	CustomRendered = 41985,
	ExposureMode = 41986,
	WhiteBalance = 41987,
	DigitalZoomRatio = 41988,
	FocalLengthIn35mmFilm = 41989,
	SceneCaptureType = 41990,
	GainControl = 41991,
	Contrast = 41992,
	Saturation = 41993,
	Sharpness = 41994,
	DeviceSettingDescription = 41995,
	SubjectDistanceRange = 41996,
	ImageUniqueID = 42016,
	CameraOwnerName = 42032,
	BodySerialNumber = 42033,
	LensSpecification = 42034,
	LensMake = 42035,
	LensModel = 42036,
	LensSerialNumber = 42037,
} tagNum_t;


/**                                                                      **/
/**   macro which initializes a structure entry containing an enum and   **/
/**   a constant string pointing to its name.                            **/
/**                                                                      **/

#define INIT_ENUM_STR(e)	{ e, #e, }


/**                                                                      **/
/**   macro returns the number of elements in an array.                  **/
/**                                                                      **/

#define	N_ELEMENTS(a)	(sizeof(a) / sizeof(*a))


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

unsigned short cSwapUShort(unsigned short a, const internalStruct *internal)
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

unsigned int cSwapUInt(unsigned int a, const internalStruct *internal)
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

int cSwapInt(int a, const internalStruct *internal)
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

float cSwapFloat(float a, const internalStruct *internal)
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
/**   Return a pointer to a constant string description of the supplied  **/
/**   Tag descriptor.                                                    **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   tag     -- tag number                                              **/
/**                                                                      **/
/**                                                                      **/

const char *getTagDescriptor(unsigned short tag)
{
	unsigned int i;
	const char *str = "unknown";
	static const tagString desc[] = {
		INIT_ENUM_STR(NewSubfileType),
		INIT_ENUM_STR(SubfileType),
		INIT_ENUM_STR(ImageWidth),
		INIT_ENUM_STR(ImageLength),
		INIT_ENUM_STR(BitsPerSample),
		INIT_ENUM_STR(Compression),
		INIT_ENUM_STR(PhotometricInterpretation),
		INIT_ENUM_STR(Threshholding),
		INIT_ENUM_STR(CellWidth),
		INIT_ENUM_STR(CellLength),
		INIT_ENUM_STR(FillOrder),
		INIT_ENUM_STR(DocumentName),
		INIT_ENUM_STR(ImageDescription),
		INIT_ENUM_STR(Make),
		INIT_ENUM_STR(Model),
		INIT_ENUM_STR(StripOffsets),
		INIT_ENUM_STR(Orientation),
		INIT_ENUM_STR(SamplesPerPixel),
		INIT_ENUM_STR(RowsPerStrip),
		INIT_ENUM_STR(StripByteCounts),
		INIT_ENUM_STR(MinSampleValue),
		INIT_ENUM_STR(MaxSampleValue),
		INIT_ENUM_STR(XResolution),
		INIT_ENUM_STR(YResolution),
		INIT_ENUM_STR(PlanarConfiguration),
		INIT_ENUM_STR(XPosition),
		INIT_ENUM_STR(YPosition),
		INIT_ENUM_STR(FreeOffsets),
		INIT_ENUM_STR(FreeByteCounts),
		INIT_ENUM_STR(GrayResponseUnit),
		INIT_ENUM_STR(GrayResponseCurve),
		INIT_ENUM_STR(T4Options),
		INIT_ENUM_STR(T6Options),
		INIT_ENUM_STR(ResolutionUnit),
		INIT_ENUM_STR(PageNumber),
		INIT_ENUM_STR(TransferFunction),
		INIT_ENUM_STR(Software),
		INIT_ENUM_STR(DateTime),
		INIT_ENUM_STR(Artist),
		INIT_ENUM_STR(HostComputer),
		INIT_ENUM_STR(Predictor),
		INIT_ENUM_STR(WhitePoint),
		INIT_ENUM_STR(PrimaryChromaticities),
		INIT_ENUM_STR(ColorMap),
		INIT_ENUM_STR(HalftoneHints),
		INIT_ENUM_STR(TileWidth),
		INIT_ENUM_STR(TileHeight),
		INIT_ENUM_STR(TileOffsets),
		INIT_ENUM_STR(TileByteCounts),
		INIT_ENUM_STR(InkSet),
		INIT_ENUM_STR(InkNames),
		INIT_ENUM_STR(NumberOfInks),
		INIT_ENUM_STR(DotRange),
		INIT_ENUM_STR(TargetPrinter),
		INIT_ENUM_STR(ExtraSamples),
		INIT_ENUM_STR(SampleFormat),
		INIT_ENUM_STR(SMinSampleValue),
		INIT_ENUM_STR(SMaxSampleValue),
		INIT_ENUM_STR(TransferRange),
		INIT_ENUM_STR(JPEGProc),
		INIT_ENUM_STR(JPEGInterchangeFormat),
		INIT_ENUM_STR(JPEGInterchangeFormatLength),
		INIT_ENUM_STR(JPEGRestartInterval),
		INIT_ENUM_STR(JPEGLosslessPredictors),
		INIT_ENUM_STR(JPEGPointTransforms),
		INIT_ENUM_STR(JPEGQTables),
		INIT_ENUM_STR(JPEGDCTables),
		INIT_ENUM_STR(JPEGACTables),
		INIT_ENUM_STR(YCbCrCoefficients),
		INIT_ENUM_STR(YCbCrSubSampling),
		INIT_ENUM_STR(YCbCrPositioning),
		INIT_ENUM_STR(ReferenceBlackWhite),
		INIT_ENUM_STR(ExposureTime),
		INIT_ENUM_STR(FNumber),
		INIT_ENUM_STR(ExifIFDPointer),
		INIT_ENUM_STR(ExposureTime2),
		INIT_ENUM_STR(ExposureProgram),
		INIT_ENUM_STR(SpectralSensitivity),
		INIT_ENUM_STR(ISOSpeedRatings),
		INIT_ENUM_STR(OECF),
		INIT_ENUM_STR(ExifVersion),
		INIT_ENUM_STR(DateTimeOriginal),
		INIT_ENUM_STR(DateTimeDigitized),
		INIT_ENUM_STR(ComponentsConfiguration),
		INIT_ENUM_STR(CompressedBitsPerPixel),
		INIT_ENUM_STR(ShutterSpeedValue),
		INIT_ENUM_STR(ApertureValue),
		INIT_ENUM_STR(BrightnessValue),
		INIT_ENUM_STR(ExposureBiasValue),
		INIT_ENUM_STR(MaxApertureValue),
		INIT_ENUM_STR(SubjectDistance),
		INIT_ENUM_STR(MeteringMode),
		INIT_ENUM_STR(LightSource),
		INIT_ENUM_STR(Flash),
		INIT_ENUM_STR(FocalLength),
		INIT_ENUM_STR(SubjectArea),
		INIT_ENUM_STR(MakerNote),
		INIT_ENUM_STR(UserComment),
		INIT_ENUM_STR(SubSecTime),
		INIT_ENUM_STR(SubSecTimeOriginal),
		INIT_ENUM_STR(SubSecTimeDigitized),
		INIT_ENUM_STR(FlashpixVersion),
		INIT_ENUM_STR(ColorSpace),
		INIT_ENUM_STR(PixelXDimension),
		INIT_ENUM_STR(PixelYDimension),
		INIT_ENUM_STR(RelatedSoundFile),
		INIT_ENUM_STR(FlashEnergy),
		INIT_ENUM_STR(SpatialFrequencyResponse),
		INIT_ENUM_STR(FocalPlaneXResolution),
		INIT_ENUM_STR(FocalPlaneYResolution),
		INIT_ENUM_STR(FocalPlaneResolutionUnit),
		INIT_ENUM_STR(SubjectLocation),
		INIT_ENUM_STR(ExposureIndex),
		INIT_ENUM_STR(SensingMethod),
		INIT_ENUM_STR(FileSource),
		INIT_ENUM_STR(SceneType),
		INIT_ENUM_STR(CFAPattern),
		INIT_ENUM_STR(CustomRendered),
		INIT_ENUM_STR(ExposureMode),
		INIT_ENUM_STR(WhiteBalance),
		INIT_ENUM_STR(DigitalZoomRatio),
		INIT_ENUM_STR(FocalLengthIn35mmFilm),
		INIT_ENUM_STR(SceneCaptureType),
		INIT_ENUM_STR(GainControl),
		INIT_ENUM_STR(Contrast),
		INIT_ENUM_STR(Saturation),
		INIT_ENUM_STR(Sharpness),
		INIT_ENUM_STR(DeviceSettingDescription),
		INIT_ENUM_STR(SubjectDistanceRange),
		INIT_ENUM_STR(ImageUniqueID),
		INIT_ENUM_STR(CameraOwnerName),
		INIT_ENUM_STR(BodySerialNumber),
		INIT_ENUM_STR(LensSpecification),
		INIT_ENUM_STR(LensMake),
		INIT_ENUM_STR(LensModel),
		INIT_ENUM_STR(LensSerialNumber),
	};

	/* TODO: Add more efficient search algorithm. */

	for(i = 0; i < N_ELEMENTS(desc); i++)
	{
		if(desc[i].tag == (int)tag)
		{
			str = desc[i].string;
			break;
		}
	}

	return str;
}


/**                                                                      **/
/**   Function: getTIFFValueDesc                                         **/
/**                                                                      **/
/**   Return a pointer to a string describing the Value of the given tag **/
/**   descriptor and possibly value numbers, or "unknown" if unknown Tag **/
/**   descriptor.                                                        **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   tag     -- tag number                                              **/
/**   value   -- value number                                            **/
/**                                                                      **/

const char *getTIFFValueDesc(unsigned short tag, unsigned int value)
{
	const char *str = "unknown";

	/* TODO: Organize the buffer lookup in a less naive way. Note  */
	/* that any improvement would need to account both for buffers */
	/* affected only by the tag and buffers affected by both the   */
	/* tag and value.                                              */

	typedef struct {
		unsigned short tag;
		int checkValue;
		unsigned int value;
		const char *desc;
	} lookup_t;

	static const lookup_t lookup[] = {
		{ NewSubfileType, 0, 0, "", },
		{ ImageWidth, 0, 0, "pixels", },
		{ ImageLength, 0, 0, "pixels", },
		{ BitsPerSample, 0, 0, "", },
		{ Compression, 1, 1, "No compression", },
		{ Compression, 1, 2, "CCITT Group 3 compression", },
		{ Compression, 1, 5, "LZW compression", },
		{ Compression, 1, 32773, "PackBits compression", },
		{ Compression, 0, 0, "", },
		{ PhotometricInterpretation, 1, 0, "WhiteIsZero", },
		{ PhotometricInterpretation, 1, 1, "BlackIsZero", },
		{ PhotometricInterpretation, 1, 2, "RGB", },
		{ PhotometricInterpretation, 1, 3, "Palette color", },
		{ PhotometricInterpretation, 1, 4, "Transparency mask", },
		{ PhotometricInterpretation, 0, 0, "", },
		{ StripOffsets, 0, 0, "", },
		{ Orientation, 1, 1, "Row0:top,Col0:left", },
		{ Orientation, 1, 2, "Row0:top,Col0:right", },
		{ Orientation, 1, 3, "Row0:bottom,Col0:right", },
		{ Orientation, 1, 4, "Row0:bottom,Col0:left", },
		{ Orientation, 1, 5, "Row0:left,Col0:top", },
		{ Orientation, 1, 6, "Row0:right,Col0:top", },
		{ Orientation, 1, 7, "Row0:right,Col0:bottom", },
		{ Orientation, 1, 8, "Row0:left,Col0:bottom", },
		{ Orientation, 0, 0, "", },
		{ SamplesPerPixel, 0, 0, "", },
		{ RowsPerStrip, 0, 0, "", },
		{ StripByteCounts, 0, 0, "", },
		{ PlanarConfiguration, 1, 1, "Chunky", },
		{ PlanarConfiguration, 1, 2, "Planar", },
		{ PlanarConfiguration, 0, 0, "", },
		{ ResolutionUnit, 1, 1, "No absolute unit", },
		{ ResolutionUnit, 1, 2, "Inch", },
		{ ResolutionUnit, 1, 3, "Centimeter", },
		{ ResolutionUnit, 0, 0, "", },
		{ Software, 0, 0, "", },
		{ Predictor, 1, 1, "No prediction scheme", },
		{ Predictor, 1, 2, "Horizontal differencing", },
		{ Predictor, 0, 0, "", },
		{ SampleFormat, 1, 1, "Unsigned integer data", },
		{ SampleFormat, 1, 2, "Two's compliment signed integer data", },
		{ SampleFormat, 1, 3, "IEEE floating point data", },
		{ SampleFormat, 1, 4, "Undefined data format", },
		{ SampleFormat, 0, 0, "", },
		{ ExposureTime, 0, 0, "seconds", },
		{ ExposureProgram, 1, 0, "Not defined", },
		{ ExposureProgram, 1, 1, "Manual", },
		{ ExposureProgram, 1, 2, "Normal program", },
		{ ExposureProgram, 1, 3, "Aperture priority", },
		{ ExposureProgram, 1, 4, "Shutter priority", },
		{ ExposureProgram, 1, 5, "Creative program", },
		{ ExposureProgram, 1, 6, "Action program", },
		{ ExposureProgram, 1, 7, "Portrait mode", },
		{ ExposureProgram, 1, 8, "Landscape mode", },
		{ ExposureProgram, 0, 0, "", },
		{ FocalPlaneResolutionUnit, 1, 1, "No absolute unit", },
		{ FocalPlaneResolutionUnit, 1, 2, "Inch", },
		{ FocalPlaneResolutionUnit, 1, 3, "Centimeter", },
		{ FocalPlaneResolutionUnit, 0, 0, "", },
	};
	const lookup_t *p;
	unsigned int i;

	for (i = 0, p = lookup; i < N_ELEMENTS(lookup); i++, p++)
	{
		if (tag == p->tag && (p->checkValue == 0 || value == p->value))
		{
			str = p->desc;
			break;
		}
	}

	return str;
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

size_t getFieldTypeNumBytes(fieldType_t fieldType)
{
	size_t num = 0;
	unsigned int i;
	const fieldTypeData_t *p;

	for (i = 0, p = fieldTypeLookup; i < N_ELEMENTS(fieldTypeLookup); i++, p++)
	{
		if (fieldType == p->type)
		{
			num = p->numBytes;
			break;
		}
	}

	return num;
}


/**                                                                      **/
/**   Function: getTIFFTypeDesc                                          **/
/**                                                                      **/
/**   Return a pointer to a string describing the given Type of an Image **/
/**   File Directory (IFD) entry.                                        **/
/**                                                                      **/
/**   Input parameters:                                                  **/
/**   fieldType  -- the Type number of an Image File Directory (IFD)     **/
/**                 entry.                                               **/
/**                                                                      **/

const char *getTIFFTypeDesc(fieldType_t fieldType)
{
	const char *str = "unknown";
	unsigned int i;
	const fieldTypeData_t *p;

	for (i = 0, p = fieldTypeLookup; i < N_ELEMENTS(fieldTypeLookup); i++, p++)
	{
		if (fieldType == p->type)
		{
			str = p->desc;
			break;
		}
	}

	return str;
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

void printEntry(const unsigned char *buffer, unsigned short tag,
	fieldType_t fieldType, const internalStruct *internal)
{
	byte4 tmp;
	const char *desc;
	unsigned int numerator, denominator;
	int snumerator, sdenominator;
	double result;

	size_t numBytes = getFieldTypeNumBytes(fieldType);

	switch(fieldType)
	{
		case FT_ASCII:
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
		case FT_SHORT:
		{
			memcpy(tmp.b, buffer, numBytes);

			tmp.s = cSwapUShort(tmp.s, internal);

			desc = getTIFFValueDesc(tag, (unsigned int)tmp.s);
			printf("Value %d %s\n", tmp.s, desc);

			break;
		}
		case FT_LONG:
		{
			memcpy(tmp.b, buffer, numBytes);

			tmp.u = cSwapUInt(tmp.u, internal);

			desc = getTIFFValueDesc(tag, tmp.u);
			printf("Value %d %s\n", tmp.u, desc);

			break;
		}
		case FT_RATIONAL:
		{
			memcpy(tmp.b, buffer, numBytes / 2);
			numerator = cSwapUInt(tmp.u, internal);
			memcpy(tmp.b, buffer + numBytes / 2, numBytes / 2);
			denominator = cSwapUInt(tmp.u, internal);

			result = (double)numerator/(double)denominator;

			printf("Value (%d/%d) %lf\n", numerator, denominator,
				result);

			break;
		}
		case FT_SRATIONAL:
		{
			memcpy(tmp.b, buffer, numBytes / 2);
			snumerator = cSwapInt(tmp.u, internal);
			memcpy(tmp.b, buffer + numBytes / 2, numBytes / 2);
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

void printDump(const unsigned char *buffer, int count)
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
	fieldType_t fieldType, unsigned int count,
	unsigned int valueOffset, const internalStruct *internal)
{
	int i;
	int cur_pos;
	int entry_bytes;
	unsigned char *buffer;

	cur_pos = ftell(file);

	fseek(file, (int)(valueOffset + internal->tiffOffset), SEEK_SET);

	if(fieldType == FT_ASCII)
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
	else if(fieldType == FT_UNDEFINED)
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

void tiffIFDPrint(const char *filename, FILE *file, internalStruct *internal)
{
	int i;
	unsigned short tiffIFDEntries;
	struct tiffIFDEntry ifd_entry;
	int total_bytes;
	const char *desc;
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
			desc = getTagDescriptor(ifd_entry.tag);
			printf("\tTag %d  (%04X.H)   %s\n", ifd_entry.tag,
				ifd_entry.tag, desc);
			ifd_entry.fieldType = cSwapUShort(ifd_entry.fieldType,
				internal);

			desc = getTIFFTypeDesc(ifd_entry.fieldType);
			printf("\tType %d %s\n", ifd_entry.fieldType, desc);

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
				if(ifd_entry.fieldType == FT_SHORT)
				{
					value = (unsigned int)
						cSwapUShort(tmp.s, internal);
				}
				else if(ifd_entry.fieldType == FT_LONG)
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
				else if(ifd_entry.fieldType == FT_UNDEFINED)
				{
					printDump( (unsigned char *)tmp.b,
						total_bytes);
				}
				else
				{
					value = cSwapUInt(tmp.u, internal);
				}

				if(ifd_entry.fieldType != FT_UNDEFINED)
				{
					desc = getTIFFValueDesc(
						ifd_entry.tag,
						value);
					printf("\tValue %d %s\n", value,
						desc);
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

int tiffMetadataPrint(const char *filename)
{
	FILE *file;
	struct tiffImageFileHeader tiff_hdr;
	unsigned char buffer[1024];
	internalStruct internal;

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
