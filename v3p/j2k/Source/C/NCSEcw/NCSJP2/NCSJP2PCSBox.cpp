/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
**
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2PCSBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2PCSBox class implementation
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
**			 [01]	08Jul04	tfl		Changes to write GeoTIFF UUID box correctly	 
**			 [02]   03Aug04 tfl	    Completed first draft of full GeoTIFF Box functionality
**			 [03]	18Aug04 tfl		Changes to unparsing code (macro, STL bugs), cleanup
**			 [04]   11Nov04 tfl		Change to fix ModelPixelScale values bug
 *******************************************************/

#include "NCSJP2File.h"
#include "NCSGeoTIFFBoxUtil.h" //Contains relevant typedefs and constants
#include "NCSAffineTransform.h"
#include "NCSWorldFile.h"
#include "NCSBuildNumber.h"
#include "../../NCSGDT2/NCSGDTEpsg.h"

//These macros manage any OS endianness and pointer alignment issues
#define READ_UINT16(Stream, b, v) (b ? (Stream.ReadUINT16(v), NCSByteSwap16(v)) : (Stream.ReadUINT16(v), v))
#define READ_UINT32(Stream, b, v) (b ? (Stream.ReadUINT32(v), NCSByteSwap32(v)) : (Stream.ReadUINT32(v), v))
#define READ_DOUBLE(Stream, b, v) (b ? (Stream.ReadIEEE8(v), NCSByteSwapRange64((UINT64*)&v, (UINT64*)&v, 1), v) : (Stream.ReadIEEE8(v), v))
#ifdef NCSBO_LSBFIRST
#define WRITE_UINT16(v,pByte) { \
		UINT16 w = (UINT16)(v);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(UINT16);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_UINT16(v,pByte) { \
		UINT16 w = (UINT16)(v);\
		w = NCSByteSwap16(w);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(UINT16);\
	}
#endif
#ifdef NCSBO_LSBFIRST
#define WRITE_UINT32(v,pByte) { \
		UINT32 w = (UINT32)(v);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(UINT32);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_UINT32(v,pByte) { \
		UINT32 w = (UINT32)(v);\
		w = NCSByteSwap32(w);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(UINT32);\
	}
#endif
#ifdef NCSBO_LSBFIRST
#define WRITE_IEEE8(v,pByte) { \
		IEEE8 w = (IEEE8)(v);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(IEEE8);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_IEEE8(v,pByte) { \
		IEEE8 w = (IEEE8)(v);\
		NCSByteSwapRange64((UINT64 *)&w,(UINT64 *)&w,1);\
		memcpy(pByte, &w, sizeof(w)); \
		pByte += sizeof(IEEE8);\
	}
#endif

#define USE_MODEL_TRANSFORMATION_TAG
#define MIN_GCS_CODE	4000
#define MAX_GCS_CODE	4999
#define MIN_PCS_CODE	20000
#define MAX_PCS_CODE	32760

const UINT8 PCS_UUID[16] = {
						0xb1,0x4b,0xf8,0xbd,0x08,0x3d,0x4b,0x43,
						0xa5,0xae,0x8c,0xd7,0xd5,0xa6,0xce,0x03};
NCSUUID CNCSJP2File::CNCSJP2PCSBox::sm_UUID(PCS_UUID);
IEEE4 CNCSJP2File::CNCSJP2PCSBox::sm_fGeodataPrecisionEpsilon = DEFAULT_GEODATA_PRECISION;

//A few utility functions
static bool GetCoordSysGeoKeys(NCSFileViewFileInfoEx &FileInfo, 
								  CNCSJP2File::CNCSJP2PCSBox::GeoKey *pCoordSysKey, 
								  CNCSJP2File::CNCSJP2PCSBox::GeoKey *pUnitsKey);
static bool GetDatumProjection(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo);
static void GetUnitsFromGeoKey(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo);
static void GetUnitsFromFileInfo(NCSFileViewFileInfoEx &FileInfo, CNCSJP2File::CNCSJP2PCSBox::GeoKey *p);


//These enum types have value correspondence with the GeoTIFF specification
// - so don't change them
typedef enum {
	ModelType			= 1024, //0x0400
	RasterType			= 1025, //0x0401
	CitationType        = 1026, //0x0402
	GeogType			= 2048, //0x0800
	GeogLinearUnits		= 2052, //0x0804
	GeogAngularUnits	= 2054, //0x0806
	PCSType				= 3072, //0x0c00
	ProjLinearUnits		= 3076, //0x0c04
} GeoKeyNames;

typedef enum {
	Projected			= 1,
	Geographic			= 2,
	Geocentric			= 3
} ModelTypeNames;

typedef enum {
	PixelIsArea			= 1,
	PixelIsPoint		= 2,
} RasterTypeNames;

typedef enum {
	Meter				= 9001,
	Foot				= 9002,
	Foot_US_Survey		= 9003,
	Foot_Modified_American	= 9004,
	Foot_Clarke			= 9005,
	Foot_Indian			= 9006,
	Link				= 9007,
	Link_Benoit			= 9008,
	Link_Sears			= 9009,
	Chain_Benoit		= 9010,
	Chain_Sears			= 9011,
	Yard_Sears			= 9012,
	Yard_Indian			= 9013,
	Fathom				= 9014,
	Mile_International_Nautical = 9015,
} LinearUnitNames;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJP2File::CNCSJP2PCSBox::CNCSJP2PCSBox()
{
	NCSInitFileInfoEx(&m_PCSFileInfo); 
	memcpy(&m_UUID, &sm_UUID, sizeof(sm_UUID));
	m_nGeoTIFFBoxLength = 0;
}

// Destructor
CNCSJP2File::CNCSJP2PCSBox::~CNCSJP2PCSBox()
{
	/*for(UINT32 i = 0; i < m_GeoKeys.size(); i++) {
		if(m_GeoKeys[i].Type == GeoASCIIParams) {
			NCSFree(m_GeoKeys[i].v.pValue);
		}
	}*/
	NCSFreeFileInfoEx(&m_PCSFileInfo);
}

void CNCSJP2File::CNCSJP2PCSBox::UpdateXLBox(void)
{
	UINT8 buf[1024];
	if (FormatGeoTIFFInfo((UINT8 *)buf, (char *)NULL) == NCS_SUCCESS)
	{
		CNCSJP2Box::UpdateXLBox();
		m_nXLBox += m_nGeoTIFFBoxLength;
		m_bValid = true;
	}
	else //Processing error or file not georeferenced
	{
		m_nXLBox = 0;
		m_bValid = false;
	}
}


// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2PCSBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr,"Parsing PCS box information\n");
#endif

	CNCSError Error;
	std::vector<double> GeoKeyDoubleParams;
	//std::vector<char *> GeoKeyASCIIParams;

	m_bValid = false;	//This is only set to true after all GeoTIFF information has been 
						//correctly parsed

	//See NCSJPCIOStream.h to get an understanding of these macros
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(Read(m_UUID.m_UUID, sizeof(m_UUID.m_UUID)));

		if(m_UUID == sm_UUID) {
			UINT16 i16;
			bool bSwap;
#ifdef NCSBO_LSBFIRST
			bSwap = (Stream.ReadUINT16(i16) && i16 == TIFF_BIGENDIAN_CODE);
#elif defined(NCSBO_MSBFIRST)
			bSwap = !(Stream.ReadUINT16(i16) && i16 == TIFF_LITTLEENDIAN_CODE);
#endif
			//Check for presence of TIFF identification code in degenerate GeoTIFF
			if(READ_UINT16(Stream, bSwap, i16) == TIFF_IDENTITY_CODE) {
				UINT32 i32;
				//A TIFF file header consists of a sequence of IFDs revealing file
				//information (IFD == 'Image File Directory').  The size of these 
				//IFDs is indicated by a four byte offset value.  The PCS UUID boxes we 
				//write out will only contain one IFD, but more are possible.

				//First scan through all the IFD's, parsing out any relevant GeoTIFF Tags
				UINT32 iIFD = READ_UINT32(Stream, bSwap, i32); //read first IFD offset

				while(iIFD != 0) {
					//seek to start of IFD information
					Stream.Seek(m_nDBoxOffset + sizeof(m_UUID.m_UUID) + iIFD, CNCSJPCIOStream::START);
					//check how many metadata entries there are in this IFD and iterate through them
					UINT16 nDirEntries = READ_UINT16(Stream, bSwap, i16);
					for(UINT32 i = 0; i < nDirEntries; i++) {
						TIFFTag Tag;
						Tag.Tag = READ_UINT16(Stream, bSwap, i16);
						Tag.Type = READ_UINT16(Stream, bSwap, i16);
						Tag.Count = READ_UINT32(Stream, bSwap, i32);
						Tag.Offset = READ_UINT32(Stream, bSwap, i32);
						
						//Only take note if these parsed tags have the correct data types
						switch(Tag.Tag) {
							case ModelTiePoint:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing ModelTiePoint\n");
#endif
									if(Tag.Type == 12) {//DOUBLE
										m_TIFFTags.push_back(Tag);
									}
								break;
							case ModelPixelScale:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing ModelPixelScale\n");
#endif
									if(Tag.Type == 12 && Tag.Count == 3) {//DOUBLE, one 3-point
										m_TIFFTags.push_back(Tag);
									}
								break;
							case ModelTransformation:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing ModelTransformation\n");
#endif
									if (Tag.Type == 12 && Tag.Count == 16) {//DOUBLE, 16 matrix values
										m_TIFFTags.push_back(Tag);
									}
								break;
							case GeoKeyDirectory:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing GeoKeyDirectory\n");
#endif
								if(Tag.Type == 3) {//SHORT
										m_TIFFTags.push_back(Tag);
									}
								break;
							case GeoDoubleParams:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing GeoDoubleParams\n");
#endif	
								if(Tag.Type == 12) {//DOUBLE
										m_TIFFTags.push_back(Tag);
									}
								break;
							case GeoASCIIParams:
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
								fprintf(stderr,"Parsing GeoASCIIParams\n");
#endif
								if(Tag.Type == 2) {//ASCII
										m_TIFFTags.push_back(Tag);										
									}
								break;
							default:break;
						}
					}
					iIFD = READ_UINT32(Stream, bSwap, i32); //read offset of next IFD
				}											//this is 0 for the last IFD

				//Now scan through the GeoTIFF Tags, parsing the GeoTIFF Keys
				for(UINT32 t = 0; t < m_TIFFTags.size(); t++) {

					//The offset of the tag values is contained within the tags, seek to it
					Stream.Seek(m_nDBoxOffset + sizeof(sm_UUID.m_UUID) + m_TIFFTags[t].Offset, CNCSJPCIOStream::START);
					switch(m_TIFFTags[t].Tag) {
						case ModelTiePoint:
						case ModelPixelScale:
						case ModelTransformation:
							{ //'Count' here will be either 3, 6, or 16
									for(UINT32 i = 0; i < m_TIFFTags[t].Count; i++) {
										double d64 = READ_DOUBLE(Stream, bSwap, d64);
										m_TIFFTags[t].Values.push_back(d64);
									}
								}
							break;
						case GeoKeyDirectory:
								{
									READ_UINT16(Stream, bSwap, i16);
									READ_UINT16(Stream, bSwap, i16);
									READ_UINT16(Stream, bSwap, i16);
									UINT16 nKeys = READ_UINT16(Stream, bSwap, i16);
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
									fprintf(stderr,"Parsing GeoKeys ...\n");
#endif
									for(int k = 0; k < nKeys; k++) {
										GeoKey key;
										key.KeyID = READ_UINT16(Stream, bSwap, i16);
										key.Type = (GeoKeyType)READ_UINT16(Stream, bSwap, i16);
										key.Count = READ_UINT16(Stream, bSwap, i16);
										key.v.nValue = READ_UINT16(Stream, bSwap, i16);
										m_GeoKeys.push_back(key);
									}
								}
							break;
						case GeoDoubleParams:
								{//DOUBLE
									double d64;
									for(UINT32 p = 0; p < m_TIFFTags[t].Count; p++) {
										d64 = READ_DOUBLE(Stream, bSwap, d64);
										GeoKeyDoubleParams.push_back(d64);
									}
								}
							break;
						case GeoASCIIParams:
								{//ASCII - this data is not currently processed
									/*char *p = (char*)NCSMalloc(m_TIFFTags[t].Count, TRUE);
									Stream.Read(p, m_TIFFTags[t].Count);
									GeoKeyASCIIParams.push_back(p);*/									
								}
							break;
						default:break;
					}
				}
				//Determine the values for the keys
				for(UINT32 k = 0; k < m_GeoKeys.size(); k++) {
					switch(m_GeoKeys[k].Type) {
						case 0:	
							break;
						case GeoDoubleParams: 
								m_GeoKeys[k].v.dValue = GeoKeyDoubleParams[m_GeoKeys[k].v.nValue];
							break;
						case GeoASCIIParams:	
								{
									/*char *p = GeoKeyASCIIParams[0] + m_GeoKeys[k].v.nValue;
									int nLen = (int)(strstr(p, "|") ? (strstr(p, "|") - p) : strlen(p)); 
									m_GeoKeys[k].v.pValue = (char*)NCSMalloc(nLen + 1, TRUE);
									strncpy(m_GeoKeys[k].v.pValue, p, nLen);*/
								}
							break;
					}
				}

				//m_bValid = true;
			}
		}
	NCSJP2_CHECKIO_END();

	//Process the data into member NCSFileViewFileInfoEx
	Error = ProcessGeoTIFFInfo();

	/*for(UINT32 i = 0; i < GeoKeyASCIIParams.size(); i++) {
		NCSFree(GeoKeyASCIIParams[i]);
	}*/

	if( Error == NCS_SUCCESS ) {
		m_bValid = true;
	}
	return(Error);
}

CNCSError CNCSJP2File::CNCSJP2PCSBox::UnParse(CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr, "UnParsing PCS box information\n");
#endif
	UINT8 buf[1024];
	CNCSError Error = FormatGeoTIFFInfo((UINT8 *)buf, CHAR_STRING(Stream.GetName()));

	if(Error == NCS_SUCCESS) 
	{
		Error = CNCSJP2Box::UnParse(JP2File, Stream);
		NCSJP2_CHECKIO_BEGIN(Error, Stream);
			Stream.Write(buf, m_nGeoTIFFBoxLength);
		NCSJP2_CHECKIO_END();
	}
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2PCSBox::FormatGeoTIFFInfo(UINT8 *pBuf, char *szFilename)
{
	if (NCSIsFileInfoExGeoreferenced(&m_PCSFileInfo)) 
	{
		CNCSError Error;
		std::vector<IEEE8> GeoTIFFDoubleValues; 

		//Empty out old GeoKey/TIFFTag vectors
		/*fprintf(stderr,"Size of m_GeoKeys: %d\r\n",m_GeoKeys.size());
		for(int nGeoKeyCount = 0; nGeoKeyCount < m_GeoKeys.size(); nGeoKeyCount++) {
			if(m_GeoKeys[nGeoKeyCount].Type == GeoASCIIParams) //&& 
			   m_GeoKeys[nGeoKeyCount].v.pValue != NULL)
			{
				NCSFree(m_GeoKeys[nGeoKeyCount].v.pValue);
			}
		}*/
		m_GeoKeys.clear();
		m_TIFFTags.clear();

   		//Build citation
		char szCitation[1024];
		sprintf((char *)szCitation,
				 "This file was created with the ECW JPEG 2000 SDK build %s "
                 "copyright 1998-2005 by ER Mapper.  This GeoJP2 header was translated "
				 "from the following ER Mapper style registration information:\r\n"
                 "\tfOriginX:\t\t\t%f\r\n"
                 "\tfOriginY:\t\t\t%f\r\n"
                 "\tfCellIncrementX:\t\t%f\r\n"
                 "\tfCellIncrementY:\t\t%f\r\n"
                 "\tfCWRotationDegrees:\t%f\r\n"
                 "\tszProjection:\t\t%s\r\n"
                 "\tszDatum:\t\t\t%s\r\n",
                 NCS_VERSION_STRING_DOT_DEL,
                 m_PCSFileInfo.fOriginX,
                 m_PCSFileInfo.fOriginY,
                 m_PCSFileInfo.fCellIncrementX,
                 m_PCSFileInfo.fCellIncrementY,
                 m_PCSFileInfo.fCWRotationDegrees,
                 m_PCSFileInfo.szProjection,
                 m_PCSFileInfo.szDatum);

		//Create TIFF tags (set offset values later)
		UINT8 nTags = 0;
		TIFFTag TiePoint, CoordTransform, KeyDirectory, ASCIIParams;
		TiePoint.Tag = ModelTiePoint;
		TiePoint.Type = 12; CoordTransform.Type = 12;//DOUBLE;
		TiePoint.Count = 6; //Two 3-points
		KeyDirectory.Tag = GeoKeyDirectory;
		KeyDirectory.Type = 3; //SHORT
		KeyDirectory.Count = 5; //RasterType, ModelType, GeogType/PCSType, GeogLinearUnits/ProjLinearUnits, CitationType
		ASCIIParams.Tag = GeoASCIIParams;
		ASCIIParams.Type = 2; //ASCII
		ASCIIParams.Count = (UINT32)strlen(szCitation)+1; //only one citation string required!
		
		//If we have a nonzero rotation we try using a ModelTransformationTag
		//If we are using a Transformation tag it comes after the TiePoint (ID = 0x85d8)
		//If we are using a PixelScale tag it comes before the TiePoint (ID = 0x830e)
		//This bit of code can run in one of two modes.  In one it uses a ModelTransformation, in the other
		//it writes a world file, because ArcGIS does not support rotated affine values in the GeoJP2 box
		//(or the "world chunk" box for that matter).

		//Eventual decision was to run in the mode where the ModelTransformationTag is used, and handle 
		//any alternative use of world files at the application level (e.g. for the 
		//free compressor).

#ifdef	USE_MODEL_TRANSFORMATION_TAG
		CNCSAffineTransform Affine(sm_fGeodataPrecisionEpsilon);
		if ((m_PCSFileInfo.fCWRotationDegrees != 0.0) && Affine.GetFromFileInfoEx(m_PCSFileInfo, TRUE)
			&& (fabs(Affine.fRotX) > sm_fGeodataPrecisionEpsilon || 
				fabs(Affine.fRotY) > sm_fGeodataPrecisionEpsilon))
		{
			CoordTransform.Tag = ModelTransformation;
			CoordTransform.Count = 16;
			m_TIFFTags.push_back(CoordTransform);
			nTags = 13;

			//Transformation matrix has a lot of known, fixed values
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fScaleX);
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fRotX);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fTransX);
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fRotY);
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fScaleY);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)Affine.fTransY);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);
			GeoTIFFDoubleValues.push_back((IEEE8)1.0);
		}
#else //!USE_MODEL_TRANSFORMATION_TAG
		CNCSWorldFile WorldFile(sm_fGeodataPrecisionEpsilon);
		if ((m_PCSFileInfo.fCWRotationDegrees != 0.0) && WorldFile.GetFromFileInfoEx(m_PCSFileInfo, TRUE)
			&& (fabs(WorldFile.fRotX) > sm_fGeodataPrecisionEpsilon || 
				fabs(WorldFile.fRotY) > sm_fGeodataPrecisionEpsilon))
		{
			nTags = 12;
			//Write world file
			if (szFilename != NULL)
			{
				char *szWorldFilename;
				if (WorldFile.ConstructWorldFilename(szFilename,&szWorldFilename))
				{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
					fprintf(stderr,"World file name: %s\n",szWorldFilename);
#endif
					if (WorldFile.Write(szWorldFilename))
					{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
						fprintf(stderr,"Writing information to world file %s\n", pWorldFilename);
#endif	
					}
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
					else fprintf(stderr,"Unable to process information from world file %s\n",pWorldFilename);
#endif			
				}
				NCSFree(szWorldFilename);
			}
		}
#endif //USE_MODEL_TRANSFORMATION_TAG
		else 
		{	//no rotation or (unlikely) something invalid about situation, use ModelPixelScale
			CoordTransform.Tag = ModelPixelScale;
			CoordTransform.Count = 3; //3 scale values
			//PixelScale.Offset - set this later
			m_TIFFTags.push_back(CoordTransform);
			m_TIFFTags.push_back(TiePoint);
			nTags = 14;

			//PixelScale values
			GeoTIFFDoubleValues.push_back((IEEE8)m_PCSFileInfo.fCellIncrementX);
			//GeoTIFFDoubleValues.push_back((IEEE8)m_PCSFileInfo.fCellIncrementY);
			GeoTIFFDoubleValues.push_back((IEEE8)-m_PCSFileInfo.fCellIncrementY);//[04]
			GeoTIFFDoubleValues.push_back((IEEE8)0.0);

			//Tiepoint values
			//Push back dataset origin
			GeoTIFFDoubleValues.push_back((IEEE8)0.0L);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0L);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0L);
			//Push back world origin
			GeoTIFFDoubleValues.push_back((IEEE8)m_PCSFileInfo.fOriginX);
			GeoTIFFDoubleValues.push_back((IEEE8)m_PCSFileInfo.fOriginY);
			GeoTIFFDoubleValues.push_back((IEEE8)0.0L);
		}

		m_TIFFTags.push_back(KeyDirectory);
		m_TIFFTags.push_back(ASCIIParams);

		GeoKey RasterTypeKey, ModelTypeKey, CoordTypeKey, UnitsKey, CitationKey;	
		RasterTypeKey.KeyID = RasterType;
		RasterTypeKey.Type = GKT_SHORT; RasterTypeKey.Count = 1;
		RasterTypeKey.v.nValue = PixelIsArea;//Always use RasterPixelIsArea

		ModelTypeKey.KeyID = ModelType;
		ModelTypeKey.Type = GKT_SHORT; ModelTypeKey.Count = 1; 	
		CoordTypeKey.Type = GKT_SHORT; CoordTypeKey.Count = 1;
		CitationKey.KeyID = CitationType;
		CitationKey.Type = GKT_ASCII; CitationKey.Count = (UINT16)strlen(szCitation)+1;
		CitationKey.v.nValue = 0;
		GetUnitsFromFileInfo(m_PCSFileInfo, &UnitsKey);

		bool bUseCoordSys = false;
		//If we can get an EPSG code use a PCS or GCS GeoKey when writing out
		if (GetCoordSysGeoKeys(m_PCSFileInfo, &CoordTypeKey, &UnitsKey)) //adds EPSG code
		{
			bUseCoordSys = true;
			if (CoordTypeKey.KeyID == GeogType)
				ModelTypeKey.v.nValue = Geographic;
			else ModelTypeKey.v.nValue = Projected;
		}
		//If we have an EPSG, but it is unknown
		else if (strnicmp(m_PCSFileInfo.szProjection,"epsg:",5) == 0) //we have an unknown EPSG
		{
			bUseCoordSys = true;
			ModelTypeKey.v.nValue = Projected;
			CoordTypeKey.KeyID = PCSType;
			char *pColon = strchr(m_PCSFileInfo.szProjection,':');
			pColon++;
			CoordTypeKey.v.nValue = atoi(pColon); //grab EPSG code
			UnitsKey.KeyID = ProjLinearUnits;
			UnitsKey.Type = GKT_SHORT; UnitsKey.Count = 1;
			UnitsKey.v.nValue = Meter;
		}
		if (bUseCoordSys)
			m_GeoKeys.push_back(ModelTypeKey);
		m_GeoKeys.push_back(RasterTypeKey);
		m_GeoKeys.push_back(CitationKey);
		if (bUseCoordSys)
		{
			m_GeoKeys.push_back(CoordTypeKey);
			m_GeoKeys.push_back(UnitsKey);
		}

		//Copy across initial data
		m_nGeoTIFFBoxLength = sizeof(GeoTIFFInitialData);
		memcpy((void *)pBuf, (void *)GeoTIFFInitialData, m_nGeoTIFFBoxLength);
		pBuf[NUM_IFD_ENTRIES_OFFSET] = (UINT8)nTags;
		pBuf[STRIP_BYTE_COUNTS_TAG_OFFSET_OFFSET] = (UINT8) (GEOTIFF_HEADER_LENGTH_BYTES + 
													nTags*TIFF_TAG_LENGTH_BYTES + 
													IFD_TERMINATOR_LENGTH_BYTES);
		pBuf[XRESOLUTION_TAG_OFFSET_OFFSET] = (UINT8) (GEOTIFF_HEADER_LENGTH_BYTES +
												nTags*TIFF_TAG_LENGTH_BYTES + 
												IFD_TERMINATOR_LENGTH_BYTES + 
												STRIP_BYTE_COUNTS_VALUE_LENGTH_BYTES);
		pBuf[YRESOLUTION_TAG_OFFSET_OFFSET] = (UINT8) (GEOTIFF_HEADER_LENGTH_BYTES +
												nTags*TIFF_TAG_LENGTH_BYTES +
												IFD_TERMINATOR_LENGTH_BYTES +
												STRIP_BYTE_COUNTS_VALUE_LENGTH_BYTES + 
												RESOLUTION_VALUE_LENGTH_BYTES);											
				

		//Calculate offsets to tag values and GeoKeys
		m_nGeoTIFFBoxLength += (TIFF_TAG_LENGTH_BYTES*(UINT32)m_TIFFTags.size() + sizeof(GeoTIFFMiddleData)); 

		TIFFTag *t;
		TIFFTag *u;
		//If we are using a PixelScale/TiePoint PixelScale comes before the TiePoint (ID = 0x830e)
		if ((t = GetTag(ModelTransformation)) != NULL)
		{
			t->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
			m_nGeoTIFFBoxLength += t->Count * sizeof(IEEE8); //48
			//u->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
			//m_nGeoTIFFBoxLength += u->Count * sizeof(IEEE8); //128 or 24
		}
		else if ((t = GetTag(ModelPixelScale)) != NULL)
		{
			u = GetTag(ModelTiePoint);
			t->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
			m_nGeoTIFFBoxLength += t->Count * sizeof(IEEE8);
			u->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
			m_nGeoTIFFBoxLength += u->Count * sizeof(IEEE8);
		}

		t = GetTag(GeoKeyDirectory);
		t->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
		t->Count = (GEOKEY_LENGTH_BYTES/2)*((UINT32)m_GeoKeys.size() + 1); //FIXME
   		m_nGeoTIFFBoxLength += GEOKEY_LENGTH_BYTES*(UINT32)m_GeoKeys.size() + GEOKEY_LENGTH_BYTES; //add 8 for GeoKey version info

		t = GetTag(GeoASCIIParams);
		t->Offset = (UINT32)(m_nGeoTIFFBoxLength - UUID_LEN);
		m_nGeoTIFFBoxLength += t->Count * sizeof(char);


		//Write out GeoTIFF data
		UINT8 *pBufIndex = pBuf + sizeof(GeoTIFFInitialData);
		std::vector<TIFFTag>::iterator iTag = m_TIFFTags.begin();
		std::vector<TIFFTag>::iterator iTagEnd = m_TIFFTags.end();
		for (iTag; iTag != iTagEnd; iTag++)
		{
			WRITE_UINT16(iTag->Tag,pBufIndex);
			WRITE_UINT16(iTag->Type,pBufIndex);
			WRITE_UINT32(iTag->Count,pBufIndex);
			WRITE_UINT32(iTag->Offset,pBufIndex);
		}

        //Write out default data for the middle of the file
        memcpy((void *)pBufIndex,(void *)GeoTIFFMiddleData,sizeof(GeoTIFFMiddleData));
        pBufIndex += sizeof(GeoTIFFMiddleData);

		std::vector<IEEE8>::iterator iDouble = GeoTIFFDoubleValues.begin();
		//for(...) needs {} because of macro definition
		for (iDouble; iDouble != GeoTIFFDoubleValues.end(); iDouble++)	
		{
			WRITE_IEEE8(*iDouble,pBufIndex);
		}

		WRITE_UINT16(0x01,pBufIndex);//Write out GeoKeys - start by writing out version info using 1.1.0
		WRITE_UINT16(0x01,pBufIndex);
		WRITE_UINT16(0x00,pBufIndex);
		WRITE_UINT16(m_GeoKeys.size(),pBufIndex);
		std::vector<GeoKey>::iterator iKey = m_GeoKeys.begin();
		std::vector<GeoKey>::iterator iKeyEnd = m_GeoKeys.end();
		for (iKey; iKey != iKeyEnd; iKey++)
		{
			WRITE_UINT16(iKey->KeyID,pBufIndex);
			WRITE_UINT16(iKey->Type,pBufIndex)
			WRITE_UINT16(iKey->Count,pBufIndex);
			WRITE_UINT16(iKey->v.nValue,pBufIndex);
		}
		//Write out citation
		memcpy((void *)pBufIndex,(void *)szCitation, strlen(szCitation)+1);
		
		return(NCS_SUCCESS);
	}
	else 
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"File not georeferenced: no PCS box created\n");
#endif
		m_nGeoTIFFBoxLength = 0;
		return(CNCSError(NCS_JP2_GEODATA_NOT_GEOREFERENCED));
	}
}

CNCSError CNCSJP2File::CNCSJP2PCSBox::GetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSFree(FileInfo.szDatum);
	NCSFree(FileInfo.szProjection);
	FileInfo.szDatum = NCSStrDup(m_PCSFileInfo.szDatum);
	FileInfo.szProjection = NCSStrDup(m_PCSFileInfo.szProjection);
	FileInfo.eCellSizeUnits = m_PCSFileInfo.eCellSizeUnits;
	FileInfo.fCellIncrementX = m_PCSFileInfo.fCellIncrementX;
	FileInfo.fCellIncrementY = m_PCSFileInfo.fCellIncrementY;
	FileInfo.fOriginX = m_PCSFileInfo.fOriginX;
	FileInfo.fOriginY = m_PCSFileInfo.fOriginY;
	FileInfo.fCWRotationDegrees = m_PCSFileInfo.fCWRotationDegrees;
	//NCSCopyFileInfoEx(&FileInfo, &m_PCSFileInfo);
	return(NCS_SUCCESS);
}

CNCSError CNCSJP2File::CNCSJP2PCSBox::SetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSCopyFileInfoEx(&m_PCSFileInfo, &FileInfo);
	return(NCS_SUCCESS);
}

CNCSJP2File::CNCSJP2PCSBox::TIFFTag *CNCSJP2File::CNCSJP2PCSBox::GetTag(TIFFTagNames eTag)
{
	UINT32 nSize = (UINT32)m_TIFFTags.size();
	for(UINT32 k = 0; k < nSize; k++) {
		if(m_TIFFTags[k].Tag == eTag) {
			return(&m_TIFFTags[k]);
		}
	}
	return(NULL);
}

CNCSJP2File::CNCSJP2PCSBox::GeoKey *CNCSJP2File::CNCSJP2PCSBox::GetKey(UINT16 Key)
{
	UINT32 nSize = (UINT32)m_GeoKeys.size();
	for(UINT32 k = 0; k < nSize; k++) {
		if(m_GeoKeys[k].KeyID == Key) {
			return(&m_GeoKeys[k]);
		}
	}
	return(NULL);
}

CNCSError CNCSJP2File::CNCSJP2PCSBox::ProcessGeoTIFFInfo()
{
	if(m_TIFFTags.size() == 0) {
		return(CNCSError(NCS_JP2_GEODATA_READ_ERROR));
	}

	TIFFTag *pT;
	
	m_PCSFileInfo.fCWRotationDegrees = 0.0; //no rotation is the default
	NCSFree(m_PCSFileInfo.szProjection);
	NCSFree(m_PCSFileInfo.szDatum);
	m_PCSFileInfo.szDatum = NCSStrDup("RAW"); //changed default back to RAW/RAW (was WGS84/LOCAL)												  
	m_PCSFileInfo.szProjection = NCSStrDup("RAW");	 
	
	if((pT = GetTag(ModelPixelScale)) != NULL) 
	{
		m_PCSFileInfo.fCellIncrementX = pT->Values[0];
		m_PCSFileInfo.fCellIncrementY = -pT->Values[1];
	}
	if((pT = GetTag(ModelTiePoint)) != NULL) 
	{
		TIFFTag *pU;
		//ModelTiePoint AND ModelPixelScale means we use an unrotated coord system
		if(pT->Values[0] == 0.0 && pT->Values[1] == 0.0) 
		{
			//Degenerate case
			m_PCSFileInfo.fOriginX = pT->Values[3];
			m_PCSFileInfo.fOriginY = pT->Values[4];
		}
		else if ((pU = GetTag(ModelPixelScale)) != NULL) 
		{
			m_PCSFileInfo.fOriginX = pT->Values[3] - pT->Values[0]*pU->Values[0];
			m_PCSFileInfo.fOriginY = pT->Values[4] + pT->Values[1]*pU->Values[1];
		}
	}
	
	//If the ModelTransformation matrix is present we only consider it if one
	//of the ModelTiePoint and ModelPixelScale tags is missing, according 
	//to the recommendations of the GeoTIFF specification (part 2.6.1)
	if ((pT = GetTag(ModelTransformation)) != NULL)
	{
		if (!GetTag(ModelTiePoint) || !GetTag(ModelPixelScale))
		{
			CNCSAffineTransform Affine(sm_fGeodataPrecisionEpsilon);
			//We don't deal with shearing so check validity here
			if (Affine.GetFromGeoTIFFModelTransformationTag(pT))
				Affine.PopulateFileInfoEx(m_PCSFileInfo, TRUE);	
		}
	}
	

	GeoKey *p;
	if((p = GetKey(RasterType)) != NULL) {
		if(p->v.nValue == PixelIsPoint) {
			m_PCSFileInfo.fOriginX -= 0.5 * m_PCSFileInfo.fCellIncrementX;                      
			m_PCSFileInfo.fOriginY -= 0.5 * m_PCSFileInfo.fCellIncrementY;                      
		}
	}

	//Free existing projection/datum


	if((p = GetKey(ModelType)) != NULL) {
		switch(p->v.nValue) {
			case Projected:
					if((p = GetKey(PCSType)) != NULL) {
						if (!GetDatumProjection(p, m_PCSFileInfo))
						//If we have a PCSType key with an EPSG code but we didn't get anything 
						//from the GDT database create a default value "epsg:<code>" - this logic should 
						//be rolled back into NCSGDTEpsg
						{
							char szEPSG[ECW_MAX_PROJECTION_LEN];
							sprintf(szEPSG,"epsg:%ld", p->v.nValue);
							m_PCSFileInfo.szProjection = NCSStrDup(szEPSG);
							m_PCSFileInfo.szDatum = NCSStrDup(szEPSG);
						}
					}
					if((p = GetKey(ProjLinearUnits)) != NULL) {
						GetUnitsFromGeoKey(p, m_PCSFileInfo);
					}
				break;
			case Geographic:
			case Geocentric:
					if((p = GetKey(GeogType)) != NULL) {
						GetDatumProjection(p, m_PCSFileInfo);
					}
					if((p = GetKey(GeogLinearUnits)) != NULL) {
						GetUnitsFromGeoKey(p, m_PCSFileInfo);
					}
				break;
		}
	}
	else
	{
		if((p = GetKey(GeogLinearUnits)) != NULL) GetUnitsFromGeoKey(p,m_PCSFileInfo);
		else if ((p = GetKey(ProjLinearUnits)) != NULL) GetUnitsFromGeoKey(p,m_PCSFileInfo);
	}
	//sensible defaults if anything broken	
	if (m_PCSFileInfo.szProjection == NULL || m_PCSFileInfo.szDatum == NULL)
	{
		m_PCSFileInfo.szDatum = NCSStrDup("RAW");
		m_PCSFileInfo.szProjection = NCSStrDup("RAW");
	}
	
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr,"ProcessGeoTIFFInfo-->observed rotation: %lg\n", m_PCSFileInfo.fCWRotationDegrees);
#endif

	m_bValid = true;


	return(NCS_SUCCESS);
}

void CNCSJP2File::CNCSJP2PCSBox::SetPrecision(IEEE4 fPrecision)
{
	sm_fGeodataPrecisionEpsilon = fPrecision;
}

//Returns true if the GeoKey argument is actually a PCSType Key with an EPSG code
bool GetDatumProjection(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo)
{
	CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
	char *pProjection = NULL;
	char *pDatum = NULL;

	if((Epsg.GetProjectionAndDatum(p->v.nValue, &pProjection, &pDatum)) == NCS_SUCCESS) {
		if(pProjection) {
			NCSFree(FileInfo.szProjection);
			FileInfo.szProjection = NCSStrDup(pProjection);
			NCSFree(pProjection);
		}
		if(pDatum) {
			NCSFree(FileInfo.szDatum);
			FileInfo.szDatum = NCSStrDup(pDatum);
			NCSFree(pDatum);
		}
	} 
	return (p->KeyID == PCSType);
}

bool GetCoordSysGeoKeys(NCSFileViewFileInfoEx &FileInfo, 
						   CNCSJP2File::CNCSJP2PCSBox::GeoKey *pCoordSysKey, 
						   CNCSJP2File::CNCSJP2PCSBox::GeoKey *pUnitsKey)
{
	CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
	UINT16 nEpsgCode;
	if ((nEpsgCode = Epsg.GetEPSG(FileInfo.szProjection,FileInfo.szDatum)) != 0)
	{
		//Distinguish between PCS and GCS keys using the code value
		if (MIN_GCS_CODE <= nEpsgCode && nEpsgCode <= MAX_GCS_CODE)
		{
			pCoordSysKey->KeyID = GeogType;
			pCoordSysKey->Type = CNCSJP2File::CNCSJP2PCSBox::GKT_SHORT;
			pCoordSysKey->Count = 1;
			pCoordSysKey->v.nValue = nEpsgCode;
			pUnitsKey->KeyID = GeogLinearUnits;
			pUnitsKey->Type = CNCSJP2File::CNCSJP2PCSBox::GKT_SHORT;
			pUnitsKey->Count = 1;
			GetUnitsFromFileInfo(FileInfo, pUnitsKey);
			return true;
		}
		else 
		//Default to unknown PCS regardless even if code is not in valid range
		//if (MIN_PCS_CODE <= nEpsgCode && nEpsgCode <= MAX_PCS_CODE)
		{
			pCoordSysKey->KeyID = PCSType;
			pCoordSysKey->Type = CNCSJP2File::CNCSJP2PCSBox::GKT_SHORT;
			pCoordSysKey->Count = 1;
			pCoordSysKey->v.nValue = nEpsgCode;
			pUnitsKey->KeyID = ProjLinearUnits;
			pUnitsKey->Type = CNCSJP2File::CNCSJP2PCSBox::GKT_SHORT;
			pUnitsKey->Count = 1;
			GetUnitsFromFileInfo(FileInfo, pUnitsKey);
			return true;
		}
	}
	else return false; //and don't change values in GeoKeys
}

void GetUnitsFromGeoKey(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo)
{
	switch(p->v.nValue) {
		case Meter:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
				break;
		case Foot_US_Survey:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_FEET;
				break;
		case Foot:
		case Foot_Modified_American:
		case Foot_Clarke:
		case Foot_Indian:
		case Link:
		case Link_Benoit:
		case Link_Sears:
		case Chain_Benoit:
		case Chain_Sears:
		case Yard_Sears:
		case Yard_Indian:
		case Fathom:
		case Mile_International_Nautical:
		default:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_UNKNOWN;
				break;
	}
}

void GetUnitsFromFileInfo(NCSFileViewFileInfoEx &FileInfo, CNCSJP2File::CNCSJP2PCSBox::GeoKey *p)
{
	switch (FileInfo.eCellSizeUnits)
	{
		case ECW_CELL_UNITS_METERS:
			p->v.nValue = Meter;
			break;
		case ECW_CELL_UNITS_FEET:
			p->v.nValue = Foot_US_Survey;
			break;
		case ECW_CELL_UNITS_UNKNOWN:
			p->v.nValue = Meter;
			break;
		default:
			p->v.nValue = Meter;
			break;
	}
}
