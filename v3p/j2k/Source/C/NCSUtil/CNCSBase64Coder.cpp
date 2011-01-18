/********************************************************
** Copyright 2000 Earth Resource Mapping Ltd.
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
** NOTE:    This code is obtained from MSDN July 1999
**
** FILE:   	NCSUtil\CNCSBase64Coder.cpp
** CREATED:	08Mar00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Base64 conversion routines
** EDITS:
** [01] 30Oct00  ny	Merge CE/Palm modification in
					preparation for Macintosh port
** [02]	03Nov00	 ny Mac port changes
 *******************************************************/

// CNCSBase64Coder.cpp: implementation of the Base64Coder class.
//
//////////////////////////////////////////////////////////////////////

#include <memory.h>
#include "CNCSBase64Coder.h"
#include "NCSUtil.h"	/**[01]**/

// Digits...
static char	Base64Digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

BOOLEAN CNCSBase64Coder::m_Init		= FALSE;
char CNCSBase64Coder::m_DecodeTable[256];

#ifndef PAGESIZE
#define PAGESIZE					4096
#endif

#ifndef ROUNDTOPAGE
#define ROUNDTOPAGE(a)			(((a/4096)+1)*4096)
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSBase64Coder::CNCSBase64Coder()
:	m_pDBuffer(NULL),
	m_pEBuffer(NULL),
	m_nDBufLen(0),
	m_nEBufLen(0)
{

}

CNCSBase64Coder::~CNCSBase64Coder()
{
	if(m_pDBuffer != NULL)
		delete [] m_pDBuffer;

	if(m_pEBuffer != NULL)
		delete [] m_pEBuffer;
}

UINT8 * CNCSBase64Coder::DecodedMessage() const /**[02]**/
{ 
	return (UINT8 *) m_pDBuffer;	/**[02]**/
}

UINT8 * CNCSBase64Coder::EncodedMessage() const	/**[02]**/
{ 
	return (UINT8 *) m_pEBuffer;	/**[02]**/
}

void CNCSBase64Coder::AllocEncode(UINT32 nSize)	/**[02]**/
{
	if(m_nEBufLen < nSize)
	{
		if(m_pEBuffer != NULL)
			delete [] m_pEBuffer;

		m_nEBufLen = ROUNDTOPAGE(nSize);
		m_pEBuffer = new UINT8[m_nEBufLen];	/**[02]**/
	}

	NCSMemSet(m_pEBuffer, 0, m_nEBufLen); /**[01]**/ //::ZeroMemory(m_pEBuffer, m_nEBufLen);
	m_nEDataLen = 0;
}

void CNCSBase64Coder::AllocDecode(UINT32 nSize)	/**[02]**/
{
	if(m_nDBufLen < nSize)
	{
		if(m_pDBuffer != NULL)
			delete [] m_pDBuffer;

		m_nDBufLen = ROUNDTOPAGE(nSize);
		m_pDBuffer = new UINT8[m_nDBufLen];	/**[02]**/
	}

	NCSMemSet(m_pDBuffer, 0, m_nDBufLen); /**[01]**/ //::ZeroMemory(m_pDBuffer, m_nDBufLen);
	m_nDDataLen = 0;
}

void CNCSBase64Coder::SetEncodeBuffer(const UINT8 * pBuffer, UINT32 nBufLen) /**[02]**/
{
	UINT32	i = 0;	/**[02]**/

	AllocEncode(nBufLen);
	while(i < nBufLen)
	{
		if(!_IsBadMimeChar(pBuffer[i]))
		{
			m_pEBuffer[m_nEDataLen] = pBuffer[i];
			m_nEDataLen++;
		}

		i++;
	}
}

void CNCSBase64Coder::SetDecodeBuffer(const UINT8 * pBuffer, UINT32 nBufLen)	/**[02]**/
{
	AllocDecode(nBufLen);
	memcpy(m_pDBuffer, pBuffer, nBufLen); /**[01]**/ //::CopyMemory(m_pDBuffer, pBuffer, nBufLen);
	m_nDDataLen = nBufLen;
}

void CNCSBase64Coder::Encode(const UINT8 * pBuffer, UINT32 nBufLen)	/**[02]**/
{
	SetDecodeBuffer(pBuffer, nBufLen);
	AllocEncode(nBufLen * 2);

	TempBucket			Raw;
	UINT32					nIndex	= 0;	/**[02]**/

	while((nIndex + 3) <= nBufLen)
	{
		Raw.Clear();
		memcpy(&Raw, m_pDBuffer + nIndex, 3); /**[01]**/ //::CopyMemory(&Raw, m_pDBuffer + nIndex, 3);
		Raw.nSize = 3;
		_EncodeToBuffer(Raw, m_pEBuffer + m_nEDataLen);
		nIndex		+= 3;
		m_nEDataLen	+= 4;
	}

	if(nBufLen > nIndex)
	{
		Raw.Clear();
		Raw.nSize = (UINT8) (nBufLen - nIndex);	/**[02]**/
		memcpy(&Raw, m_pDBuffer + nIndex, nBufLen - nIndex); /**[01]**/ //::CopyMemory(&Raw, m_pDBuffer + nIndex, nBufLen - nIndex);
		_EncodeToBuffer(Raw, m_pEBuffer + m_nEDataLen);
		m_nEDataLen += 4;
	}
}

void CNCSBase64Coder::Encode(UINT8 * szMessage)	/**[02]**/
{
	if(szMessage != NULL)
		CNCSBase64Coder::Encode( (const UINT8 *)szMessage, (UINT32)strlen((const char *)szMessage) );
}

void CNCSBase64Coder::Decode(const UINT8 * pBuffer, UINT32 dwBufLen)	/**[02]**/
{
	if(!CNCSBase64Coder::m_Init)
		_Init();

	SetEncodeBuffer(pBuffer, dwBufLen);

	AllocDecode(dwBufLen);

	TempBucket			Raw;

	UINT32		nIndex = 0;	/**[02]**/

	while((nIndex + 4) <= m_nEDataLen)
	{
		Raw.Clear();
		Raw.nData[0] = CNCSBase64Coder::m_DecodeTable[m_pEBuffer[nIndex]];
		Raw.nData[1] = CNCSBase64Coder::m_DecodeTable[m_pEBuffer[nIndex + 1]];
		Raw.nData[2] = CNCSBase64Coder::m_DecodeTable[m_pEBuffer[nIndex + 2]];
		Raw.nData[3] = CNCSBase64Coder::m_DecodeTable[m_pEBuffer[nIndex + 3]];

		if(Raw.nData[2] == 255)
			Raw.nData[2] = 0;
		if(Raw.nData[3] == 255)
			Raw.nData[3] = 0;
		
		Raw.nSize = 4;
		_DecodeToBuffer(Raw, m_pDBuffer + m_nDDataLen);
		nIndex += 4;
		m_nDDataLen += 3;
	}
	
   // If nIndex < m_nEDataLen, then we got a decode message without padding.
   // We may want to throw some kind of warning here, but we are still required
   // to handle the decoding as if it was properly padded.
	if(nIndex < m_nEDataLen)
	{
		Raw.Clear();
		for(UINT32 i = nIndex; i < m_nEDataLen; i++)	/**[02]**/
		{
			Raw.nData[i - nIndex] = CNCSBase64Coder::m_DecodeTable[m_pEBuffer[i]];
			Raw.nSize++;
			if(Raw.nData[i - nIndex] == 255)
				Raw.nData[i - nIndex] = 0;
		}

		_DecodeToBuffer(Raw, m_pDBuffer + m_nDDataLen);
		m_nDDataLen += (m_nEDataLen - nIndex);
	}
}

void CNCSBase64Coder::Decode(UINT8 * szMessage) /**[02]**/
{
	if(szMessage != NULL)
		CNCSBase64Coder::Decode((const UINT8 *)szMessage/**[02]**/, (UINT32)strlen((const char *)szMessage)); /**[01]**/
}

UINT32 CNCSBase64Coder::_DecodeToBuffer(const TempBucket &Decode, UINT8 * pBuffer)	/**[02]**/
{
	TempBucket	Data;
	UINT32			nCount = 0;	/**[02]**/

	_DecodeRaw(Data, Decode);

	for(int i = 0; i < 3; i++)
	{
		pBuffer[i] = Data.nData[i];
		if(pBuffer[i] != 255)
			nCount++;
	}

	return nCount;
}


void CNCSBase64Coder::_EncodeToBuffer(const TempBucket &Decode, UINT8 * pBuffer)	/**[02]**/
{
	TempBucket	Data;

	_EncodeRaw(Data, Decode);

	for(int i = 0; i < 4; i++)
		pBuffer[i] = Base64Digits[Data.nData[i]];

	switch(Decode.nSize)
	{
	case 1:
		pBuffer[2] = '=';
	case 2:
		pBuffer[3] = '=';
	}
}

void CNCSBase64Coder::_DecodeRaw(TempBucket &Data, const TempBucket &Decode)
{
	UINT8		nTemp;	/**[02]**/

	Data.nData[0] = Decode.nData[0];
	Data.nData[0] <<= 2;

	nTemp = Decode.nData[1];
	nTemp >>= 4;
	nTemp &= 0x03;
	Data.nData[0] |= nTemp;

	Data.nData[1] = Decode.nData[1];
	Data.nData[1] <<= 4;

	nTemp = Decode.nData[2];
	nTemp >>= 2;
	nTemp &= 0x0F;
	Data.nData[1] |= nTemp;

	Data.nData[2] = Decode.nData[2];
	Data.nData[2] <<= 6;
	nTemp = Decode.nData[3];
	nTemp &= 0x3F;
	Data.nData[2] |= nTemp;
}

void CNCSBase64Coder::_EncodeRaw(TempBucket &Data, const TempBucket &Decode)
{
	UINT8		nTemp;	/**[02]**/

	Data.nData[0] = Decode.nData[0];
	Data.nData[0] >>= 2;
	
	Data.nData[1] = Decode.nData[0];
	Data.nData[1] <<= 4;
	nTemp = Decode.nData[1];
	nTemp >>= 4;
	Data.nData[1] |= nTemp;
	Data.nData[1] &= 0x3F;

	Data.nData[2] = Decode.nData[1];
	Data.nData[2] <<= 2;

	nTemp = Decode.nData[2];
	nTemp >>= 6;

	Data.nData[2] |= nTemp;
	Data.nData[2] &= 0x3F;

	Data.nData[3] = Decode.nData[2];
	Data.nData[3] &= 0x3F;
}

BOOLEAN CNCSBase64Coder::_IsBadMimeChar(UINT8 nData)	/**[02]**/
{
	switch(nData)
	{
		case '\r': case '\n': case '\t': case ' ' :
		case '\b': case '\a': case '\f': case '\v':
			return TRUE;
		default:
			return FALSE;
	}
}

void CNCSBase64Coder::_Init()
{  // Initialize Decoding table.

	int	i;

	for(i = 0; i < 256; i++)
		CNCSBase64Coder::m_DecodeTable[i] = -2;

	for(i = 0; i < 64; i++)
	{
		CNCSBase64Coder::m_DecodeTable[Base64Digits[i]]			= i;
		CNCSBase64Coder::m_DecodeTable[Base64Digits[i]|0x80]	= i;
	}

	CNCSBase64Coder::m_DecodeTable['=']				= -1;
	CNCSBase64Coder::m_DecodeTable['='|0x80]		= -1;

	CNCSBase64Coder::m_Init = TRUE;
}

