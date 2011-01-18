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
** CREATED:  28/01/2005 3:27:34 PM
** AUTHOR:   Tom Lynch
** PURPOSE:  NCSJP2WorldBox class implementation
** EDITS:    [xx]	ddMmmyy	NAME	COMMENTS
 *******************************************************/

#include "NCSJP2File.h"
#include "NCSAffineTransform.h"
#include "NCSBuildNumber.h"

//These macros manage any OS endianness and pointer alignment issues
#define READ_UINT16(Stream, b, v) (b ? (Stream.ReadUINT16(v), NCSByteSwap16(v)) : (Stream.ReadUINT16(v), v))
#define READ_UINT32(Stream, b, v) (b ? (Stream.ReadUINT32(v), NCSByteSwap32(v)) : (Stream.ReadUINT32(v), v)) 
#define READ_DOUBLE(Stream, b, v) (b ? (Stream.ReadIEEE8(v), NCSByteSwapRange64((UINT64*)&v, (UINT64*)&v, 1), v) : (Stream.ReadIEEE8(v), v)) 
#ifdef NCSBO_LSBFIRST
#define WRITE_UINT16(v,pByte) { \
		UINT16 w = (UINT16)(v);\
		UINT16 *pUINT16 = (UINT16 *)pByte; \
		memcpy(pUINT16, &w, sizeof(w)); \
		pByte += sizeof(UINT16);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_UINT16(v,pByte) { \
		UINT16 w = (UINT16)(v);\
		w = NCSByteSwap16(w);\
		UINT16 *pUINT16 = (UINT16 *)pByte; \
		memcpy(pUINT16, &w, sizeof(w)); \
		pByte += sizeof(UINT16);\
	}
#endif
#ifdef NCSBO_LSBFIRST
#define WRITE_UINT32(v,pByte) { \
		UINT32 w = (UINT32)(v);\
		UINT32 *pUINT32 = (UINT32 *)pByte; \
		memcpy(pUINT32, &w, sizeof(w)); \
		pByte += sizeof(UINT32);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_UINT32(v,pByte) { \
		UINT32 w = (UINT32)(v);\
		w = NCSByteSwap32(w);\
		UINT32 *pUINT32 = (UINT32 *)pByte; \
		memcpy(pUINT32, &w, sizeof(w)); \
		pByte += sizeof(UINT32);\
	}
#endif
#ifdef NCSBO_LSBFIRST
#define WRITE_IEEE8(v,pByte) { \
		IEEE8 w = (IEEE8)(v);\
		IEEE8 *pDouble = (IEEE8 *)pByte; \
		memcpy(pDouble, &w, sizeof(w)); \
		pByte += sizeof(IEEE8);\
	}
#elif defined(NCSBO_MSBFIRST)
#define WRITE_IEEE8(v,pByte) { \
		IEEE8 w = (IEEE8)(v);\
		NCSByteSwapRange64((UINT64 *)&w,(UINT64 *)&w,1);\
		IEEE8 *pDouble = (IEEE8 *)pByte; \
		memcpy(pDouble, &w, sizeof(w)); \
		pByte += sizeof(IEEE8);\
	}
#endif

//NOTE: the default UUID box data that follows is derived from the provisional
//specification for the world chunk box, and the available sample data.  The 
//content of the tags may not be accurate according to any official specification,
//if such exists.  The goal of the implementation is just to interoperate with the
//support for the world chunk box provided by applications such as ArcMap.

const static UINT8 WORLD_UUID[UUID_LEN] =
{
	 0x96, 0xa9, 0xf1, 0xf1,
	 0xdc, 0x98, 0x40, 0x2d,
	 0xa7, 0xae, 0xd6, 0x8e,
	 0x34, 0x45, 0x18, 0x09
};

const static UINT8 WorldBoxHeader[] = {
	0x4d, 0x53, 0x49, 0x47,							//"MSIG"
	0x2f, 0x10										//major and minor versions
};

const static UINT8 WorldBoxMiddleData[] = {
	0x01,											//number of chunks, always 1
	0x00,											//next box?

	//World chunk header
	0x00,											//chunk index, always 0
	0x01,											//chunk properties?
	0x30, 0x00, 0x00, 0x00							//chunk length, little-endian UINT32
	//End of world chunk header
};

const static UINT8 WorldBoxFooter[] = {
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00 //world chunk footer, probably constant
};


NCSUUID CNCSJP2File::CNCSJP2WorldBox::sm_UUID(WORLD_UUID);
IEEE4 CNCSJP2File::CNCSJP2WorldBox::sm_fGeodataPrecisionEpsilon = DEFAULT_GEODATA_PRECISION;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJP2File::CNCSJP2WorldBox::CNCSJP2WorldBox()
{
	NCSInitFileInfoEx(&m_WorldFileInfo); 
	memcpy(&m_UUID, &sm_UUID, sizeof(sm_UUID));
	m_nWorldBoxLength = 0;
	for (int i = 0; i < sizeof(m_pFeatureFlags); i++)
		m_pFeatureFlags[i] = 0x00;
}

// Destructor
CNCSJP2File::CNCSJP2WorldBox::~CNCSJP2WorldBox()
{
	NCSFreeFileInfoEx(&m_WorldFileInfo);
}

void CNCSJP2File::CNCSJP2WorldBox::UpdateXLBox(void)
{
	UINT8 buf[1024];
	if (FormatWorldInfo((UINT8 *)buf) == NCS_SUCCESS)
	{
		CNCSJP2Box::UpdateXLBox();
		m_nXLBox += m_nWorldBoxLength;
		m_bValid = true;
	}
	else //Processing error or file not georeferenced
	{
		m_nXLBox = 0;
		m_bValid = false;
	}
}


// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2WorldBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr,"Parsing World box information\n");
#endif

	CNCSError Error;
	CNCSAffineTransform Affine;
	m_bValid = false;	

	//See NCSJPCIOStream.h to get an understanding of these macros
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(Read(m_UUID.m_UUID, sizeof(m_UUID.m_UUID)));


		if(m_UUID == sm_UUID) {
			Stream.Seek(m_nDBoxOffset + sizeof(m_UUID.m_UUID) + sizeof(WorldBoxHeader), 
						CNCSJPCIOStream::START);
			NCSJP2_CHECKIO(Read(m_pFeatureFlags, sizeof(m_pFeatureFlags)));
			Stream.Seek(m_nDBoxOffset + sizeof(m_UUID.m_UUID) + sizeof(WorldBoxHeader)
						+ sizeof(m_pFeatureFlags) + sizeof(WorldBoxMiddleData), CNCSJPCIOStream::START);
			READ_DOUBLE(Stream,TRUE,Affine.fScaleX);
			READ_DOUBLE(Stream,TRUE,Affine.fRotX);
			READ_DOUBLE(Stream,TRUE,Affine.fRotY);
			READ_DOUBLE(Stream,TRUE,Affine.fScaleY);
			READ_DOUBLE(Stream,TRUE,Affine.fTransX);
			READ_DOUBLE(Stream,TRUE,Affine.fTransY);
		}
	NCSJP2_CHECKIO_END();

	//Process the data into member NCSFileViewFileInfoEx

	if(Affine.PopulateFileInfoEx(m_WorldFileInfo, TRUE)) {
		m_bValid = true;
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"ProcessWorldInfo-->observed rotation: %lg\n", m_WorldFileInfo.fCWRotationDegrees);
#endif
	}
	return(Error);
}

CNCSError CNCSJP2File::CNCSJP2WorldBox::UnParse(CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr, "UnParsing World box information\n");
#endif
	UINT8 buf[1024];
	CNCSError Error = FormatWorldInfo((UINT8 *)buf);

	if(Error == NCS_SUCCESS) 
	{
		Error = CNCSJP2Box::UnParse(JP2File, Stream);
		NCSJP2_CHECKIO_BEGIN(Error, Stream);
			Stream.Write(buf, m_nWorldBoxLength);
		NCSJP2_CHECKIO_END();
	}
	return(Error);
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2WorldBox::FormatWorldInfo(UINT8 *pBuf)
{
	if (NCSIsFileInfoExGeoreferenced(&m_WorldFileInfo)) 
	{
		UINT8 *pBufIndex = pBuf;
		//Construct output buffer
		CNCSAffineTransform Affine(sm_fGeodataPrecisionEpsilon);
		if (Affine.GetFromFileInfoEx(m_WorldFileInfo,TRUE))
		{
			m_nWorldBoxLength = UUID_LEN + sizeof(WorldBoxHeader);
			memcpy((void *)pBufIndex, (void *)m_UUID.m_UUID, UUID_LEN);
			pBufIndex += UUID_LEN;

			memcpy((void *)pBufIndex, (void *)WorldBoxHeader, sizeof(WorldBoxHeader));
			pBufIndex += sizeof(WorldBoxHeader);

			m_nWorldBoxLength += sizeof(m_pFeatureFlags);
			memcpy((void *)pBufIndex, (void *)m_pFeatureFlags, sizeof(m_pFeatureFlags));
			pBufIndex += sizeof(m_pFeatureFlags);

			m_nWorldBoxLength += sizeof(WorldBoxMiddleData);
			memcpy((void *)pBufIndex, (void *)WorldBoxMiddleData, sizeof(WorldBoxMiddleData));
			pBufIndex += sizeof(WorldBoxMiddleData);

			m_nWorldBoxLength += 6 * sizeof(IEEE8);
			WRITE_IEEE8(Affine.fScaleX,pBufIndex);
			WRITE_IEEE8(Affine.fRotX,pBufIndex);
			WRITE_IEEE8(Affine.fRotY,pBufIndex);
			WRITE_IEEE8(Affine.fScaleY,pBufIndex);
			WRITE_IEEE8(Affine.fTransX,pBufIndex);
			WRITE_IEEE8(Affine.fTransY,pBufIndex);

			m_nWorldBoxLength += sizeof(WorldBoxFooter);
			memcpy((void *)pBufIndex, (void *)WorldBoxFooter, sizeof(WorldBoxFooter));
		}
		else
		{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
			fprintf(stderr,"Error obtaining affine coefficients from file info: no World box created\r\n");
#endif
			return(NCS_JP2_GEODATA_WRITE_ERROR);
		}
		return(NCS_SUCCESS);
	}
	else 
	{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"File not georeferenced: no World box created\r\n");
#endif
		m_nWorldBoxLength = 0;
		return(CNCSError(NCS_JP2_GEODATA_NOT_GEOREFERENCED));
	}
}

CNCSError CNCSJP2File::CNCSJP2WorldBox::GetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSFree(FileInfo.szDatum);
	NCSFree(FileInfo.szProjection);
	FileInfo.szDatum = NCSStrDup(m_WorldFileInfo.szDatum);
	FileInfo.szProjection = NCSStrDup(m_WorldFileInfo.szProjection);
	FileInfo.eCellSizeUnits = m_WorldFileInfo.eCellSizeUnits;
	FileInfo.fCellIncrementX = m_WorldFileInfo.fCellIncrementX;
	FileInfo.fCellIncrementY = m_WorldFileInfo.fCellIncrementY;
	FileInfo.fOriginX = m_WorldFileInfo.fOriginX;
	FileInfo.fOriginY = m_WorldFileInfo.fOriginY;
	FileInfo.fCWRotationDegrees = m_WorldFileInfo.fCWRotationDegrees;
	//NCSCopyFileInfoEx(&FileInfo, &m_WorldFileInfo);
	return(NCS_SUCCESS);
}

CNCSError CNCSJP2File::CNCSJP2WorldBox::SetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSCopyFileInfoEx(&m_WorldFileInfo, &FileInfo);
	return(NCS_SUCCESS);
}

void CNCSJP2File::CNCSJP2WorldBox::SetPrecision(IEEE4 fPrecision)
{
	sm_fGeodataPrecisionEpsilon = fPrecision;
}

