/********************************************************
** Copyright 2003 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCBuffer.cpp $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCBuffer class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
** 			[01] 25Jan06 tfl  Added Multiply methods to support autoscale feature in CNCSJP2FileView
 *******************************************************/

#include "NCSJPCTypes.h"
#include "NCSJPCBuffer.h"
#include "NCSMalloc.h"
#include "NCSMisc.h"

//void * memcpy_amd(void *dest, const void *src, size_t n);

const UINT32 CNCSJPCBuffer::NCS_MAXUINT8	= (unsigned)0xff;
const UINT32 CNCSJPCBuffer::NCS_MAXUINT16	= (unsigned)0xffff;
const UINT32 CNCSJPCBuffer::NCS_MAXUINT32	= (unsigned)0xffffffff;
const UINT32 CNCSJPCBuffer::NCS_MINUINT8    = 0;
const UINT32 CNCSJPCBuffer::NCS_MINUINT16   = 0;
const UINT32 CNCSJPCBuffer::NCS_MINUINT32   = 0;

const UINT32 CNCSJPCBuffer::NCS_MAXINT8     = 0x7f;
const UINT32 CNCSJPCBuffer::NCS_MAXINT16    = 0x7fff;
const UINT32 CNCSJPCBuffer::NCS_MAXINT32    = 0x7fffffff;
const INT32 CNCSJPCBuffer::NCS_MININT8     = (-1 * (signed)CNCSJPCBuffer::NCS_MAXINT8 - 1);
const INT32 CNCSJPCBuffer::NCS_MININT16    = (-1 * (signed)CNCSJPCBuffer::NCS_MAXINT16 - 1);
const INT32 CNCSJPCBuffer::NCS_MININT32    = (-1 * (signed)CNCSJPCBuffer::NCS_MAXINT32 - 1);

const UINT32 CNCSJPCBuffer::AT_OWNER = (1 << 0);
const UINT32 CNCSJPCBuffer::AT_CHILD = (1 << 1);
const UINT32 CNCSJPCBuffer::AT_LOCKED = (1 << 2);	

CNCSJPCBuffer::SystemInfo CNCSJPCBuffer::sm_SystemInfo;
#ifdef WIN32
LARGE_INTEGER CNCSJPCBuffer::SystemInfo::sm_iFrequency;
#endif

NCSTimeStampUs	CNCSJPCBuffer::sm_usCopy;
UINT64			CNCSJPCBuffer::sm_nCopyBytes;
NCSTimeStampUs	CNCSJPCBuffer::sm_usClear;
UINT64			CNCSJPCBuffer::sm_nClearBytes;
NCSTimeStampUs	CNCSJPCBuffer::sm_usAlloc;
UINT64			CNCSJPCBuffer::sm_nAllocBytes;
NCSTimeStampUs	CNCSJPCBuffer::sm_usFree;

// Constructor
CNCSJPCBuffer::CNCSJPCBuffer()
{
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
}

CNCSJPCBuffer::CNCSJPCBuffer( const class CNCSJPCBuffer& src )
{
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
	*this = src;
}

CNCSJPCBuffer::CNCSJPCBuffer(UINT32 nWidth, Type eType)
{ 
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
	Alloc(nWidth, eType); 
}

CNCSJPCBuffer::CNCSJPCBuffer(UINT32 nWidth, UINT32 nHeight, Type eType)
{ 
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
	Alloc(nWidth, nHeight, eType); 
}

CNCSJPCBuffer::CNCSJPCBuffer(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType)
{ 
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
	Alloc(nX0, nY0, nX1, nY1, eType); 
}

CNCSJPCBuffer::CNCSJPCBuffer(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType)
{ 
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_pPool = NULL;
	m_nStep = 0;
	Alloc(nX0, nY0, nWidth, nHeight, eType); 
}

// Assignment operator
class CNCSJPCBuffer& CNCSJPCBuffer::operator=( const class CNCSJPCBuffer& src )
{
	CNCSJPCBuffer &Src = (CNCSJPCBuffer&)src;

	Assign(Src.GetX0(), Src.GetY0(), Src.GetWidth(), Src.GetHeight(), Src.GetType(), Src.GetPtr(), Src.GetStep());
	if(Src.TestFlags(AT_OWNER)) {
		// Ownership moves to dst buffer
		UnSetFlags(AT_CHILD);
		SetFlag(AT_OWNER);
		Src.UnSetFlags(AT_OWNER);
		Src.SetFlag(AT_CHILD);
	}
	return(*this);
}

// Destructor
CNCSJPCBuffer::~CNCSJPCBuffer()
{
	Free();
}

bool CNCSJPCBuffer::Alloc(UINT32 nWidth, Type eType)
{
	return(Alloc(0, 0, nWidth, (UINT32)1, eType));
}

bool CNCSJPCBuffer::Alloc(UINT32 nWidth, UINT32 nHeight, Type eType)
{
	return(Alloc(0, 0, nWidth, nHeight, eType));
}

bool CNCSJPCBuffer::Alloc(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType)
{
	return(Alloc(nX0, nY0, (INT32)(nX0 + nWidth), (INT32)(nY0 + nHeight), eType));
}

bool CNCSJPCBuffer::Alloc(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType)
{
	CNCSJPCRect Rect(nX0, nY0, nX1, nY1);
	if(Rect.GetWidth() != GetWidth() || Rect.GetHeight() != GetHeight() || eType != m_eType) {
		Free();
	}
	m_X0 = nX0;
	m_Y0 = nY0;
	m_X1 = nX1;
	m_Y1 = nY1;
	m_eType = eType;
	m_Flags = AT_OWNER;
	return(Alloc());
}


// Allocate a buffer of the specified type and size.
bool CNCSJPCBuffer::Alloc()
{
#ifdef NCSJPC_PROFILE
	NCSTimeStampUs tsBegin = SystemInfo::GetTime();
#endif
	if(!m_pBuffer) {
		m_nStep = Size(m_eType, GetWidth());

		if(GetHeight() != 1 && m_nStep % 32) {
			// Pad scanlines to 32 byte multiples for performance
			// when the buffer is > 1 line high 
			m_nStep = (1 + m_nStep / 32) * 32;
		}
//		if(false && m_nStep * GetHeight() <= sm_Pool.m_nSize) {
//			m_pBuffer = sm_Pool.Alloc();
//			m_pPool = &sm_Pool;
//		} else {
#if defined(WIN32)&&!defined(_WIN32_WCE)
			m_pBuffer = _aligned_malloc(m_nStep * GetHeight(), 16);
#else
			m_pBuffer = NCSMalloc(m_nStep * GetHeight(), false);
#endif
//		}
		sm_nAllocBytes += m_nStep * GetHeight();
			// Store step in # cells
		m_nStep /= Size(m_eType);
	}
#ifdef NCSJPC_PROFILE
	sm_usAlloc += SystemInfo::GetTime() - tsBegin;
#endif
	if(m_pBuffer) {
		return(true);
	}
	return(false);
}

// Free buffer allocated with Alloc().
void CNCSJPCBuffer::Free()
{
	if(m_pBuffer && TestFlags(AT_OWNER)) {
#ifdef NCSJPC_PROFILE
		NCSTimeStampUs tsBegin = SystemInfo::GetTime();
#endif
		if(m_pPool) {
			m_pPool->Free(m_pBuffer);
		} else {
#if defined(WIN32)&&!defined(_WIN32_WCE)
			_aligned_free(m_pBuffer);
#else
			NCSFree(m_pBuffer);
#endif
		}
#ifdef NCSJPC_PROFILE
		sm_usFree += SystemInfo::GetTime() - tsBegin;
#endif
		sm_nAllocBytes -= Size(m_eType, GetWidth(), GetHeight());
	}
	m_pBuffer = (void*)NULL;
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;

	m_pPool = NULL;
	m_nStep = 0;
}

bool CNCSJPCBuffer::Assign(Type eType, void *pPtr, INT32 nStep, bool bFree)
{
	return(Assign(GetX0(), GetY0(), GetX1(), GetY1(), eType, pPtr, nStep, bFree));
}

bool CNCSJPCBuffer::Assign(UINT32 nWidth, Type eType, void *pPtr, INT32 nStep, bool bFree)
{
	return(Assign(nWidth, (UINT32)1, eType, pPtr, nStep, bFree));
}

bool CNCSJPCBuffer::Assign(UINT32 nWidth, UINT32 nHeight, Type eType, void *pPtr, INT32 nStep, bool bFree)
{
	return(Assign(0, 0, nWidth, nHeight, eType, pPtr, nStep, bFree));
}

bool CNCSJPCBuffer::Assign(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType, void *pPtr, INT32 nStep, bool bFree)
{
	return(Assign(nX0, nY0, (INT32)(nX0 + nWidth), (INT32)(nY0 + nHeight), eType, pPtr, nStep, bFree));
}

bool CNCSJPCBuffer::Assign(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType, void *pPtr, INT32 nStep, bool bFree)
{
	bFree;//Keep compiler happy
//	if(true) {
		Free();
//	}
	m_X0 = nX0;
	m_Y0 = nY0;
	m_X1 = nX1;
	m_Y1 = nY1;
	m_eType = eType;
	UnSetFlags(AT_OWNER);
	SetFlag(AT_CHILD);

	m_pBuffer = pPtr;

	if(nStep) {
		m_nStep = nStep;
	} else {
		m_nStep = Size(eType, GetWidth());
		if(GetHeight() != 1 && m_nStep % 32) {
			// Pad scanlines to 32 byte multiples for performance
			// when the buffer is > 1 line high 
			m_nStep = (1 + m_nStep / 32) * 32;
		}
		// Store step in # cells
		m_nStep /= Size(eType);
	}
	return(true);
}

void CNCSJPCBuffer::Release()
{
	m_X0 = 0;
	m_Y0 = 0;
	m_X1 = 0;
	m_Y1 = 0;
	m_eType = BT_INT16;
	m_Flags = AT_OWNER;
	m_pBuffer = (void*)NULL;
	m_nStep = 0;
}

// Clip valus in a buffer to the specified range.
bool CNCSJPCBuffer::Clip(INT32 nMin, INT32 nMax)
{
	if(GetPtr()) {
		UINT32 x = 0;
		UINT32 nWidth = GetWidth();
		switch(m_eType) {
			case CNCSJPCBuffer::BT_INT16:
					// Optimise some common cases 
					if(nMin == NCS_MINUINT8 && nMax == NCS_MAXUINT8) {
						// UINT8 range
						for(x = 0; x < nWidth; x++) {
							INT16 v = ((INT16*)m_pBuffer)[x];
							if(v & 0xff00) { // Top bit is sign bit, so this tests for both >nMax & <nMin in one
								if(v > (INT16)NCS_MAXUINT8) {
									((INT16*)m_pBuffer)[x] = NCS_MAXUINT8;
								} else if(v < (INT16)NCS_MINUINT8) {
									((INT16*)m_pBuffer)[x] = NCS_MINUINT8;
								}
							}
						}					
					} else if(nMin == NCS_MININT8 && nMax == NCS_MAXINT8) {
						// INT8 range
						for(x = 0; x < nWidth; x++) {
							INT16 v = ((INT16*)m_pBuffer)[x];
							if(v & 0xff80) { // Top bit is sign bit, so this tests for both >nMax & <nMin in one
								if(v > (INT16)NCS_MAXINT8) {
									((INT16*)m_pBuffer)[x] = NCS_MAXINT8;
								} else if(v < (INT16)NCS_MININT8) {
									((INT16*)m_pBuffer)[x] = NCS_MININT8;
								}
							}
						}					
					} else {		
						for(x = 0; x < nWidth; x++) {
							INT16 v = ((INT16*)m_pBuffer)[x];
							if(v > nMax) {
								((INT16*)m_pBuffer)[x] = (INT16)nMax;
							} else if(v < nMin) {
								((INT16*)m_pBuffer)[x] = (INT16)nMin;
							}
						}
					}
				break;
			case CNCSJPCBuffer::BT_INT32:
					// Optimise some common cases 
					if(nMin == NCS_MINUINT16 && nMax == NCS_MAXUINT16) {
						// UINT16 range
						for(x = 0; x < nWidth; x++) {
							INT32 v = ((INT32*)m_pBuffer)[x];
							if(v & 0xffff0000) { // Top bit is sign bit, so this tests for both >nMax & <nMin in one
								if(v > (INT32)NCS_MAXUINT16) {
									((INT32*)m_pBuffer)[x] = NCS_MAXUINT16;
								} else if(v < (INT32)NCS_MINUINT16) {
									((INT32*)m_pBuffer)[x] = NCS_MINUINT16;
								}
							}
						}					
					} else if(nMin == NCS_MININT16 && nMax == NCS_MAXINT16) {
						// INT16 range
						for(x = 0; x < nWidth; x++) {
							INT32 v = ((INT32*)m_pBuffer)[x];
							if(v & 0xffff8000) { // Top bit is sign bit, so this tests for both >nMax & <nMin in one
								if(v > (INT32)NCS_MAXINT16) {
									((INT32*)m_pBuffer)[x] = NCS_MAXINT16;
								} else if(v < (INT32)NCS_MININT16) {
									((INT32*)m_pBuffer)[x] = NCS_MININT16;
								}
							}
						}					
					} else {		
						for(x = 0; x < nWidth; x++) {
							INT32 v = ((INT32*)m_pBuffer)[x];
							if(v > nMax) {
								((INT32*)m_pBuffer)[x] = nMax;
							} else if(v < nMin) {
								((INT32*)m_pBuffer)[x] = nMin;
							}				
						}					
					}
				break;
			case CNCSJPCBuffer::BT_IEEE4:
					return(Clip((IEEE4)nMin, (IEEE4)nMax));
				break;
		}
		return(true);
	}
	return(false);
}


// Clip valus in a buffer to the specified range.
bool CNCSJPCBuffer::Clip(IEEE4 fMin, IEEE4 fMax)
{
	if(GetPtr()) {
		UINT32 x = 0;
		UINT32 nWidth = GetWidth();

		switch(m_eType) {
			case CNCSJPCBuffer::BT_INT16:
			case CNCSJPCBuffer::BT_INT32:
					return(Clip(NCSCeil(fMin), NCSFloor(fMax)));
				break;
			case CNCSJPCBuffer::BT_IEEE4:
					if(fMin == NCS_MINUINT8 && fMax == NCS_MAXUINT8) {
						for(x = 0; x < nWidth; x++) {
							IEEE4 v = ((IEEE4*)m_pBuffer)[x];
							if(v > NCS_MAXUINT8) {
								((IEEE4*)m_pBuffer)[x] = NCS_MAXUINT8;
							} else if(v < NCS_MINUINT8) {
								((IEEE4*)m_pBuffer)[x] = NCS_MINUINT8;
							}		
						}					
					} else {
						for(x = 0; x < nWidth; x++) {
							IEEE4 v = ((IEEE4*)m_pBuffer)[x];
							if(v > fMax) {
								((IEEE4*)m_pBuffer)[x] = fMax;
							} else if(v < fMin) {
								((IEEE4*)m_pBuffer)[x] = fMin;
							}		
						}					
					}
				break;
		}
		return(true);
	}
	return(false);
}


// Add a constant value to the line.
bool CNCSJPCBuffer::Add(const INT32 nValue, INT16 nScale)
{
	bool bRet = false;

	if(nValue == 0 && nScale == 0) {
		bRet = true;
	} else {
		UINT32 x;
		UINT32 nWidth = GetWidth();

		switch(m_eType) {
			case CNCSJPCBuffer::BT_INT16:
					if(!bRet) {
						if(nScale < 0) {
							nScale *= -1;
							for(x = 0; x < nWidth; x++) {
								((INT16*)m_pBuffer)[x] = (INT16)((((INT16*)m_pBuffer)[x] + nValue) << nScale);
							}
						} else if(nScale > 0) {
							for(x = 0; x < nWidth; x++) {
								((INT16*)m_pBuffer)[x] = (INT16)((((INT16*)m_pBuffer)[x] + nValue) >> nScale);
							}
						} else if(nValue == 128) {
							for(x = 0; x < nWidth; x++) {
								((INT16*)m_pBuffer)[x] += 128;
							}
						} else {
							for(x = 0; x < nWidth; x++) {
								((INT16*)m_pBuffer)[x] = ((INT16*)m_pBuffer)[x] + (INT16)nValue;
							}
						}
						bRet = true;
					}
				break;

			case CNCSJPCBuffer::BT_INT32:
					if(nScale < 0) {
						nScale *= -1;
						for(x = 0; x < nWidth; x++) {
							((INT32*)m_pBuffer)[x] = (((INT32*)m_pBuffer)[x] + nValue) << nScale;
						}
					} else if(nScale > 0) {
						for(x = 0; x < nWidth; x++) {
							((INT32*)m_pBuffer)[x] = (((INT32*)m_pBuffer)[x] + nValue) >> nScale;
						}
					} else if(nValue == 128) {
						for(x = 0; x < nWidth; x++) {
							((INT32*)m_pBuffer)[x] += 128;
						}
					} else {
						for(x = 0; x < nWidth; x++) {
							((INT32*)m_pBuffer)[x] += nValue;
						}
					}
					bRet = true;
				break;
			
			case CNCSJPCBuffer::BT_IEEE4:
					bRet = Add((IEEE4)nValue, nScale);
		}
	}
	return(bRet);
}

// Add a constant value to the line.
bool CNCSJPCBuffer::Add(const IEEE4 fValue, INT16 nScale)
{
	bool bRet = false;
	if(!bRet) {
		UINT32 nWidth = GetWidth();
		if(nScale != 0) {
			float fScale = 1.0f / NCS2Pow(nScale);
			for(UINT32 x = 0; x < nWidth; x++) {
				((IEEE4*)m_pBuffer)[x] = (((IEEE4*)m_pBuffer)[x] + fValue) * fScale;
			}
		} else {
			for(UINT32 x = 0; x < nWidth; x++) {
				((IEEE4*)m_pBuffer)[x] += fValue;
			}
		}
		bRet = true;
	}
	return(bRet);
}
//[01]
bool CNCSJPCBuffer::Multiply(const INT32 nValue)
{
	bool bRet = false;
	UINT32 nWidth = GetWidth();
	UINT32 x;

	switch (m_eType)
	{
	case BT_INT16:
		for (x = 0; x < nWidth; ++x)
		{
			((INT16 *)m_pBuffer)[x] *= nValue;
		}
		bRet = true;
		break;
	case BT_INT32:
		for (x = 0; x < nWidth; ++x)
		{
			((INT32 *)m_pBuffer)[x] *= nValue;
		}
		bRet = true;
		break;
	case BT_IEEE4:
		bRet = Multiply((IEEE4)nValue);
		break;
	}
	return bRet;
}

bool CNCSJPCBuffer::Multiply(const IEEE4 fValue)
{
	bool bRet = false;
	UINT32 nWidth = GetWidth();
	UINT32 x;

	switch (m_eType)
	{
		case BT_INT16:
		for (x = 0; x < nWidth; ++x)
		{
			((INT16 *)m_pBuffer)[x] = (INT16)(((INT16 *)m_pBuffer)[x] * fValue);
		}
		bRet = true;
		break;
	case BT_INT32:
		for (x = 0; x < nWidth; ++x)
		{
			((INT32 *)m_pBuffer)[x] = (INT32)(((INT32 *)m_pBuffer)[x] * fValue);
		}
		bRet = true;
		break;
	case BT_IEEE4:
		for (UINT32 x = 0; x < nWidth; ++x)
		{
			((IEEE4 *)m_pBuffer)[x] *= fValue;
		}
		bRet = true;
		break;
	}
	return bRet;
}


bool CNCSJPCBuffer::OffsetShift(const INT32 nValue, UINT8 nCurrentBits, UINT8 nNewBits)
{
	if(m_eType == BT_IEEE4) {
		UINT32 nWidth = GetWidth();
		if(nCurrentBits != nNewBits || nValue != 0) {
			float fScale = 1.0f / ((NCS2Pow(nCurrentBits) - 1.0f) / (NCS2Pow(nNewBits) - 1.0f));
			for(UINT32 x = 0; x < nWidth; x++) {
				((IEEE4*)m_pBuffer)[x] = (((IEEE4*)m_pBuffer)[x] + nValue) * fScale;
			}
		}
	} else {
		return(Add(nValue, nCurrentBits - nNewBits));
	}
	return(true);
}

bool CNCSJPCBuffer::Copy(UINT32 nWidth, UINT32 nHeight, 
						 void *pDst, int nDstStep, 
						 const void *pSrc, int nSrcStep, 
						 Type eType)
{
#ifdef NCSJPC_PROFILE
	NCSTimeStampUs tsBegin = SystemInfo::GetTime();
#endif
	bool bRet = false;

	if(pDst > pSrc && pDst < (UINT8*)pSrc + nSrcStep * nHeight) {
		// Overapping SRC/DST, copy backwards
		for(INT32 y = nHeight - 1; y >= 0; y--) {
			size_t nSize = Size(eType, nWidth);
			memcpy((UINT8*)pDst + nDstStep * y, (UINT8*)pSrc + nSrcStep * y, nSize);
#ifdef NCSJPC_PROFILE
			sm_nCopyBytes += nSize;
#endif
		}
		bRet = true;
	} else {
#ifdef NOTDEF
		NCSJPC_X86_MMI
#define CACHEBLOCK 256 // number of QWORDs in a chunk
		if(sm_SystemInfo.m_bMMXPresent && Size(eType, nWidth) >= CACHEBLOCK * sizeof(UINT64)) {
			for(UINT32 y = 0; y < nHeight; y++) {
				INT32 len = CACHEBLOCK * ((Size(eType, nWidth) / 8) / CACHEBLOCK);
				INT32 len2 = Size(eType, nWidth) - (len * sizeof(UINT64));
				UINT8 *pSrc8 = (UINT8*)pSrc + nSrcStep * y;
				UINT8 *pDst8 = (UINT8*)pDst + nDstStep * y;

				if(len) {
					__asm {
						mov esi, [pSrc8] // source array
						mov edi, [pDst8] // destination array
						mov ecx, [len] // total number of QWORDS (8 bytes)
						// (assumes len / CACHEBLOCK = integer)
						lea esi, [esi+ecx*8]
						lea edi, [edi+ecx*8]
						neg ecx
						mainloop:
						mov eax, CACHEBLOCK / 16 // note: prefetch loop is unrolled 2X
						add ecx, CACHEBLOCK // move up to end of block
						prefetchloop:
						mov ebx, [esi+ecx*8-32] // read one address in this cache line...
						mov ebx, [esi+ecx*8-64] // read one address in this cache line...
						mov ebx, [esi+ecx*8-96] // read one address in this cache line...
						mov ebx, [esi+ecx*8-128] // ... and one in the previous line
						sub ecx, 16 // 16 QWORDS = 2 64-byte cache lines
						dec eax
						jnz prefetchloop
						mov eax, CACHEBLOCK / 8
						writeloop:
						movq mm0, qword ptr [esi+ecx*8]
						movq mm1, qword ptr [esi+ecx*8+8]
						movq mm2, qword ptr [esi+ecx*8+16]
						movq mm3, qword ptr [esi+ecx*8+24]
						movq mm4, qword ptr [esi+ecx*8+32]
						movq mm5, qword ptr [esi+ecx*8+40]
						movq mm6, qword ptr [esi+ecx*8+48]
						movq mm7, qword ptr [esi+ecx*8+56]
						movntq qword ptr [edi+ecx*8], mm0
						movntq qword ptr [edi+ecx*8+8], mm1
						movntq qword ptr [edi+ecx*8+16], mm2
						movntq qword ptr [edi+ecx*8+24], mm3
						movntq qword ptr [edi+ecx*8+32], mm4
						movntq qword ptr [edi+ecx*8+40], mm5
						movntq qword ptr [edi+ecx*8+48], mm6
						movntq qword ptr [edi+ecx*8+56], mm7
						add ecx, 8
						dec eax
						jnz writeloop
						or ecx, ecx
						jnz mainloop
						sfence
						emms
					}
				}
				if(len2) {
					memcpy(pDst8 + len * sizeof(UINT64), pSrc8 + len * sizeof(UINT64), (size_t)len2);
				}
			}
		} else {
#endif // NCSJPC_X86_MMI
			for(UINT32 y = 0; y < nHeight; y++) {
				size_t nSize = (size_t)Size(eType, nWidth);
				memcpy((UINT8*)pDst + nDstStep * y, (UINT8*)pSrc + nSrcStep * y, nSize);
#ifdef NCSJPC_PROFILE
				sm_nCopyBytes += nSize;
#endif
			}
#ifdef NOTDEF
			NCSJPC_X86_MMI
		}
#endif //NCSJPC_X86_MMI
		bRet = true;
	}
#ifdef NCSJPC_PROFILE
	sm_usCopy += SystemInfo::GetTime() - tsBegin;
#endif
	return(bRet);
}

void CNCSJPCBuffer::Clear()
{
	if(m_pBuffer) {
#ifdef NCSJPC_PROFILE
		NCSTimeStampUs tsBegin = SystemInfo::GetTime();
#endif
		size_t nSize = (size_t)Size(GetType(), GetWidth(), GetHeight());
/*		void *dst = m_pBuffer;
		while (nSize-- && ((UINT32)dst & 0x3)) {
			*(char *)dst = (char)0;
			dst = (char *)dst + 1;
        }
		while ((nSize -= 4) >= 4) {
			*(UINT32 *)dst = (UINT32)0;
			dst = (UINT32 *)dst + 1;
        }
		while (nSize--) {
			*(char *)dst = (char)0;
			dst = (char *)dst + 1;
        }
		*/memset(m_pBuffer, 0, nSize);
#ifdef NCSJPC_PROFILE
		sm_nClearBytes += nSize;
		sm_usClear += SystemInfo::GetTime() - tsBegin;
#endif
	}
}

CNCSJPCBuffer::SystemInfo::SystemInfo() {
#ifdef NCSJPC_X86_MMI
	m_bMMXPresent = IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE) ? true : false;
	m_bSSEPresent = IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) ? true : false;
	m_bSSE2Present = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) ? true : false;
	m_b3DNowPresent = IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE) ? true : false;
#else
	m_bMMXPresent = false;
	m_bSSEPresent = false;
	m_bSSE2Present = false;
	m_b3DNowPresent = false;
#endif
	m_nPhysicalMemBytes = (UINT64)NCSPhysicalMemorySize();
#ifdef WIN32
	if(QueryPerformanceFrequency(&sm_iFrequency) == FALSE) {
		sm_iFrequency.QuadPart = 0;
	}
#endif
}

NCSTimeStampUs CNCSJPCBuffer::SystemInfo::GetTime()
{
#if defined(WIN32) && !defined(_WIN32_WCE) && defined(_X86_)
	if(sm_iFrequency.QuadPart) {
//		LARGE_INTEGER iCount;
//		if(QueryPerformanceCounter(&iCount)) {
//			return((iCount.QuadPart * 1000000) / sm_iFrequency.QuadPart);
//		}
		UINT32 hiword;
		UINT32 loword;
		_asm
		{
			_emit 0x0f	// insert rtdsc opcode
			_emit 0x31
			mov hiword , edx
			mov loword , eax
		}		
	return ((((UINT64)hiword << 32) + loword) * 1000000) / sm_iFrequency.QuadPart;;

	}
#endif
	return(NCSGetTimeStampMs() * 1000);
}
void CNCSJPCBufferCache::SetBuffers(UINT32 nBuffers)
{ 
	if(m_Buffers.size() != nBuffers) {
		m_Buffers.resize(nBuffers); 
	}
}

CNCSJPCBuffer *CNCSJPCBufferCache::GetBuffer(UINT32 nBuffer)
{ 
	if(nBuffer < m_Buffers.size()) {
		return(&m_Buffers[nBuffer]); 
	} else {
		return(NULL);
	}
}

CNCSJPCBuffer *CNCSJPCBufferCache::FindBuffer(UINT32 &nBuffer, CNCSJPCRect &r, bool &bFound, CNCSJPCBuffer::Type eType)
{
	bFound = false;
	UINT32 nBuffers = (UINT32)m_Buffers.size();
			
	for(nBuffer = 0; nBuffer < nBuffers; nBuffer++) {
		CNCSJPCBuffer *pBuf = GetBuffer(nBuffer);
		if(pBuf->IsValid() && ((CNCSJPCRect)*pBuf) == r && pBuf->GetType() == eType) {
			bFound = true;
			break;
		}
	}
	if(!bFound) {
		UINT32 nSmallest = 0;
		CNCSJPCBuffer *pBuf = GetBuffer(nSmallest);
		CNCSJPCBuffer *pSmallest = pBuf;

		if(pBuf->GetPtr()) {
			for(nBuffer = 1; nBuffer < nBuffers; nBuffer++) {
				pBuf = GetBuffer(nBuffer);
				if(!pBuf->GetPtr() || !pBuf->IsValid() || (pBuf->IsValid() && pSmallest->IsValid() && ((CNCSJPCRect)*pBuf) < ((CNCSJPCRect)*pSmallest))) {
					nSmallest = nBuffer;
					pSmallest = pBuf;
				}
			}
			pBuf = pSmallest;
		}
		pBuf->Alloc(r.GetX0(), r.GetY0(), r.GetWidth(), r.GetHeight(), eType);
		nBuffer = nSmallest;
	}
	return(GetBuffer(nBuffer));
}

CNCSJPCBuffer::CNCSJPCBufferPool CNCSJPCBuffer::sm_Pool;

CNCSJPCBuffer::CNCSJPCBufferPool::CNCSJPCBufferPool()
{
	m_pPool = NULL;
	m_nSize = 0;
	//SetCBSize(CNCSJPCBuffer::BT_INT16, 64, 64);
}

CNCSJPCBuffer::CNCSJPCBufferPool::~CNCSJPCBufferPool()
{
	NCSPoolDestroy(m_pPool);
	m_pPool = NULL;
}

void CNCSJPCBuffer::CNCSJPCBufferPool::SetCBSize(Type eType, UINT32 nWidth, UINT32 nHeight)
{
	if(m_pPool == NULL) {
		m_nSize = Size(eType, nWidth, nHeight);
		m_pPool = NCSPoolCreate(m_nSize, 64);
	}
}

void *CNCSJPCBuffer::CNCSJPCBufferPool::Alloc()
{
	return(NCSPoolAlloc(m_pPool, FALSE));
}

void CNCSJPCBuffer::CNCSJPCBufferPool::Free(void *p)
{
	NCSPoolFree(m_pPool, p);
}
  
