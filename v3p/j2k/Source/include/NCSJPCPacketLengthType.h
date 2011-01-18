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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPacketLengthType.h $
** CREATED:  18/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPacketLengthType class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPACKETLENGTHTYPE_H
#define NCSJPCPACKETLENGTHTYPE_H

#include "NCSJPCMarker.h"

	/**
	 * CNCSJPCPacketLengthType class - the JPC Packet Length Type.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.12 $ $Author: simon $ $Date: 2005/07/13 02:51:07 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPacketLengthType {
public:
		/** Packet length - Header usually includes Data, except when using PPM or PPT markers */
	UINT32	m_nHeaderLength;
		/** Packet data length - Only when using PPM or PPT markers */
	UINT32	m_nDataLength;

		/** Default constructor, initialises members */
	CNCSJPCPacketLengthType();

		/** 
		 * Parse the fields from the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      bool		true on success.
		 */
	NCS_INLINE bool Parse(CNCSJPCIOStream &Stream) {
			UINT32 nTmp = 0;
			bool bRet = false;
			UINT8 t8;

			while((bRet = Stream.ReadUINT8(t8)) != FALSE) {
				nTmp = (nTmp << 7) | (t8 & 0x7f);
				if((t8 & 0x80) == 0) {
					break;
				}
			}
			m_nHeaderLength = nTmp;
			return(bRet);
		};
		/** 
		 * Parse the fields from a chunk of memory.
		 * @param		UINT8*		Memory buffer to parse length from.
		 * @param		&i			Index into memory buffer, incremented before return
		 * @return      bool		true on success.
		 */
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
	NCS_INLINE bool Parse(UINT8 *pBuf, INT32 &i) {
			UINT32 nTmp = 0;
			while(true) {
				UINT8 t8 = pBuf[i++];
				nTmp = (nTmp << 7) | (t8 & 0x7f);
				if((t8 & 0x80) == 0) {
					break;
				}
			}
			m_nHeaderLength = nTmp;
			return(true);
		};
#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif
		/** 
		 * UnParse the fields to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @return      bool		true on success.
		 */
	NCS_INLINE bool UnParse(CNCSJPCIOStream &Stream) {
			UINT32 nMask = 0xffffffff;
			INT8 nShift = 0;
			bool bRet = true;

			while(nMask & m_nHeaderLength) {
				nMask <<= 7;
				nShift++;
			}
			if(nShift > 0) {
				nShift--;
				nMask >>= 7;
				nMask = 0x7f << (nShift * 7);
				while(nShift >= 0 && bRet) {
					bRet &= Stream.WriteUINT8((UINT8)(((m_nHeaderLength & nMask) >> (nShift * 7)) | ((nShift > 0) ? 0x80 : 0x00)));
					nShift--;
					nMask >>= 7;
				}
			} else {
				// Zero length
				bRet &= Stream.WriteUINT8((UINT8)0x00);
			}
			return(bRet);

		};
};


#endif // !NCSJPCPACKETLENGTHTYPE_H
