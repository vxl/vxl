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
** FILE:   	NCSUtil\CNCSBase64Coder.h
** CREATED:	16Feb00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Base64 conversion routines (class definition)
** EDITS:
** [01]	30Oct00	 ny	Merge CE/Palm modification in preparation for Macintosh port
** [02] 03Nov00	 ny	Mac port changes
 *******************************************************/
#ifndef CNCSBASE64CODER_H
#define CNCSBASE64CODER_H

#ifdef WIN32
#include <windows.h>
#endif
#include <NCSUtil.h>	/**[02]**/
#include <string.h>

/*!
 * \class CNCSBase64Coder
 * \brief A Base64 text encoding/decoding class
 * \version ny 16Feb00 Created
 * \version ny 09Mar00 Fixed missing #include <windows.h>
 *
 * The CNCSBase64Coder class encapsulates the methods
 * implementing the base64 text encoding and decoding
 * scheme.
 *
 */
class CNCSBase64Coder  
{
	/*!
	 * \class TempBucket
	 * \brief Internal bucket class.
	 *
	 * The TempBucket class is only used internally to CNCSBase64Coder
	 *
	*/
	class TempBucket
	{
	public:
		UINT8		nData[4];	/**[02]**/
		UINT8		nSize;		/**[02]**/
		void		Clear() { NCSMemSet(nData, 0, 4); nSize = 0; }; /**[01]**/
	};

	UINT8 *					m_pDBuffer;		/**[02]**/
	UINT8 *					m_pEBuffer;		/**[02]**/
	UINT32					m_nDBufLen;		/**[02]**/
	UINT32					m_nEBufLen;		/**[02]**/
	UINT32					m_nDDataLen;	/**[02]**/
	UINT32					m_nEDataLen;	/**[02]**/

public:
	CNCSBase64Coder();
	virtual ~CNCSBase64Coder();

public:
	virtual void		Encode(const UINT8 *, UINT32);	/**[02]**/
	virtual void		Decode(const UINT8 *, UINT32);	/**[02]**/
	virtual void		Encode(UINT8 * sMessage);	/**[02]**/
	virtual void		Decode(UINT8 * sMessage);	/**[02]**/

	virtual UINT8 *		DecodedMessage() const;	/**[02]**/
	virtual UINT8 *		EncodedMessage() const;	/**[02]**/

	virtual void		AllocEncode(UINT32);	/**[02]**/
	virtual void		AllocDecode(UINT32);	/**[02]**/
	virtual void		SetEncodeBuffer(const UINT8 * pBuffer, UINT32 nBufLen);	/**[02]**/
	virtual void		SetDecodeBuffer(const UINT8 * pBuffer, UINT32 nBufLen);	/**[02]**/

protected:
	virtual void		_EncodeToBuffer(const TempBucket &Decode, UINT8 * pBuffer);	/**[02]**/
	virtual UINT32		_DecodeToBuffer(const TempBucket &Decode, UINT8 * pBuffer);	/**[02]**/
	virtual void		_EncodeRaw(TempBucket &, const TempBucket &);
	virtual void		_DecodeRaw(TempBucket &, const TempBucket &);
	virtual BOOLEAN		_IsBadMimeChar(UINT8);	/**[02]**/

	static  char		m_DecodeTable[256];
	static  BOOLEAN		m_Init;
	void				_Init();
};
#endif
