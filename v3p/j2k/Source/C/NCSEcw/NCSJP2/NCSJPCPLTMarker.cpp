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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCPLTMarker.cpp $
** CREATED:  19/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCPLTMarker class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCPLTMarker.h"
#include "NCSJPC.h"
#include "NCSJPCMemoryIOStream.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNCSJPCNode::CNCSJPCNodeTracker CNCSJPCPLTMarker::sm_Tracker("CNCSJPCPLTMarker", sizeof(CNCSJPCPLTMarker));

// Constructor
CNCSJPCPLTMarker::CNCSJPCPLTMarker()
{
	// Initialise the base marker class members
	m_eMarker = PLT;
	m_nZplt = 0;
	m_nLengths = 0;
	m_pLengths = NULL;
	m_bDynamic = false;
	m_bCumulativeLengths = false;
	m_nFirstPacket = 0;
	m_nHeaderLengthOffset = 0;
	m_nDataLengthOffset = 0;
}

// Destructor
CNCSJPCPLTMarker::~CNCSJPCPLTMarker()
{
	FreeLengths();
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCPLTMarker::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	&JPC;//Keep compiler happy
	CNCSError Error;

	m_bHaveMarker = true;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(ReadUINT16(m_nLength));
		NCSJP2_CHECKIO(ReadUINT8(m_nZplt));

		INT32 nLen = m_nLength - sizeof(UINT16) - sizeof(UINT8);
		INT64 nBegin = Stream.Tell();

		m_nLengths = 0;
		UINT8 *pBuf = (UINT8*)NCSMalloc(nLen, FALSE);
		bool bParseLengths = true;

		if(pBuf) {
			// Read in the PLT data, and count the # of lengths
			if(Stream.Read(pBuf, nLen)) {
				bParseLengths = false;
				UINT32 c = 0;
				INT8 *pBufLocal = (INT8*)pBuf + nLen;
				INT32 i = -nLen;

				for(; i <= -4; i+= 4) {
					c += pBufLocal[i] >= 0;
					c += pBufLocal[i+1] >= 0;
					c += pBufLocal[i+2] >= 0;
					c += pBufLocal[i+3] >= 0;
				}
				for(; i < 0; i++) {
//					c += (~(pBufLocal[i])) >> 7;
					c += pBufLocal[i] >= 0;
				}
				m_nLengths = (UINT16)c;
			}
			NCSFree(pBuf);
		} 
		if(bParseLengths) {
			CNCSJPCPacketLengthType Length;
	
			while(Stream.Tell() < (nBegin + nLen)) {
				if(Length.Parse(Stream) == false) {
					Error = Stream;
					break;
				} else {
					m_nLengths++;
				}
			}
		}

		if(Error == NCS_SUCCESS) {
			m_bValid = true;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCPLTMarker::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	Error = CNCSJPCMarker::UnParse(JPC, Stream);
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		NCSJP2_CHECKIO(WriteUINT16(m_nLength));
		NCSJP2_CHECKIO(WriteUINT8(m_nZplt));

		for(UINT32 i = 0; i < m_pLengths->size(); i++) {
			if((*m_pLengths)[i].UnParse(Stream) == false) {
				Error = Stream;
				break;
			}
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

bool CNCSJPCPLTMarker::ParseLength(CNCSJPC &JPC, CNCSJPCIOStream &Stream, UINT16 nLength)
{
	AllocLengths();
	
	if(nLength >= m_pLengths->size()) {
		if(JPC.m_bFilePPMs == false && JPC.GetTile()->m_bFilePPTs == false) {
			m_bCumulativeLengths = true;
		}

		sm_Tracker.RemoveMem(m_pLengths->size() * sizeof(CNCSJPCPacketLengthType));
		sm_Tracker.AddMem(m_nLengths * sizeof(CNCSJPCPacketLengthType));
		m_pLengths->resize(m_nLengths);
		UINT32 i = 0;

		if(m_bCumulativeLengths) {
			// Only have header lengths
			// Store lengths as cumulative lengths, makes calculating offsets for 
			// random access faster later.
			if(Stream.Seek(m_nOffset + 2*sizeof(UINT16) + sizeof(UINT8), CNCSJPCIOStream::START)) {
				UINT32 nLen = m_nLength - sizeof(UINT16) - sizeof(UINT8);
				UINT8 *pBuf = (UINT8*)NCSMalloc(nLen, FALSE);
				if(pBuf && Stream.Read(pBuf, nLen)) {
					UINT64 nTotalHeaderLength = 0;
					INT32 nOffset = 0;
				
					for(i = 0; i < NCSMin((UINT32)1, m_nLengths); i++) {
						if((*m_pLengths)[i].Parse(pBuf, nOffset) == false) {
							NCSFree(pBuf);
							return(false);
						}
					}
					UINT32 nPrevHeaderLength = (i > 0) ? (*m_pLengths)[i-1].m_nHeaderLength : 0;
				
					for(; i < m_nLengths; i++) {
						CNCSJPCPacketLengthType &TMP = (*m_pLengths)[i];
						if(TMP.Parse(pBuf, nOffset) == false) {
							NCSFree(pBuf);
							return(false);
						}
						nTotalHeaderLength += nPrevHeaderLength;
						nPrevHeaderLength = TMP.m_nHeaderLength;
						TMP.m_nHeaderLength += (UINT32)nTotalHeaderLength;
					}
					if(nTotalHeaderLength > CNCSJPCBuffer::NCS_MAXUINT32) {
						// Too big to store cumulated length in the PacketLength struct, so 
						// set it back to false and reparse the entire PLT
						m_bCumulativeLengths = false;
					}
					NCSFree(pBuf);
				} else {
					NCSFree(pBuf);
					return(false);
				}
#ifdef NOTUSED
				UINT64 nTotalHeaderLength = 0;
			
				for(i = 0; i < NCSMin(1, m_nLengths); i++) {
					if((*m_pLengths)[i].Parse(Stream) == false) {
						return(false);
					}
				}
				UINT32 nPrevHeaderLength = (i > 0) ? (*m_pLengths)[i-1].m_nHeaderLength : 0;
			
				for(; i < m_nLengths; i++) {
					CNCSJPCPacketLengthType &TMP = (*m_pLengths)[i];
					if(TMP.Parse(Stream) == false) {
						return(false);
					}
					nTotalHeaderLength += nPrevHeaderLength;
					nPrevHeaderLength = TMP.m_nHeaderLength;
					TMP.m_nHeaderLength += nTotalHeaderLength;
				}
				if(nTotalHeaderLength > CNCSJPCBuffer::NCS_MAXUINT32) {
					// Too big to store cumulated length in the PacketLength struct, so 
					// set it back to false and reparse the entire PLT
					m_bCumulativeLengths = false;
				}
#endif
			} else {
				return(false);
			}
		}
		if(!m_bCumulativeLengths) {
			if(Stream.Seek(m_nOffset + 2*sizeof(UINT16) + sizeof(UINT8), CNCSJPCIOStream::START)) {
				for(i = 0; i < m_nLengths; i++) {
					if((*m_pLengths)[i].Parse(Stream) == false) {
						return(false);
					}
				}
			} else {
				return(false);
			}
		}		
	}
	return(true);
}

// Assignment operator
void CNCSJPCPLTMarker::operator=( const class CNCSJPCPLTMarker& src )
{
	(CNCSJPCMarker&)*this = (CNCSJPCMarker&)src;
	m_nLengths = src.m_nLengths;
	m_nZplt = src.m_nZplt;
	m_bCumulativeLengths = src.m_bCumulativeLengths;
	if(src.m_pLengths) {
		UINT16 nLen = (UINT16)src.m_pLengths->size();
		if(!m_pLengths) {
			m_pLengths = new std::vector<CNCSJPCPacketLengthType>;
		}
		m_pLengths->resize(nLen);
		for(UINT32 i = 0; i < nLen; i++) {
			(*m_pLengths)[i] = (*src.m_pLengths)[i];
		}	
	}
}

void CNCSJPCPLTMarker::AllocLengths(void)
{
	if(!m_pLengths) {
		m_pLengths = new std::vector<CNCSJPCPacketLengthType>;
		sm_Tracker.AddMem(sizeof(std::vector<CNCSJPCPacketLengthType>));
	}
}

void CNCSJPCPLTMarker::FreeLengths(void)
{
	if(m_pLengths) {
		sm_Tracker.RemoveMem(m_pLengths->size() * sizeof(CNCSJPCPacketLengthType) + sizeof(std::vector<CNCSJPCPacketLengthType>));
		m_pLengths->clear();
		delete m_pLengths;
		m_pLengths = NULL;
	}
}

std::vector<CNCSJPCPacketLengthType> *CNCSJPCPLTMarker::GetLengths()
{
	return(m_pLengths);
}

void CNCSJPCPLTMarker::AddLength(CNCSJPCPacketLengthType &Len)
{
	if(m_pLengths) {
		m_pLengths->push_back(Len);
		m_nLengths++;
		sm_Tracker.AddMem(sizeof(CNCSJPCPacketLengthType));
	}
}
