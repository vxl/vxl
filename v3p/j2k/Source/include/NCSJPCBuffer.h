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
** FILE:     $Archive: /NCS/Source/include/NCSJPCBuffer.h $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCBuffer class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**	     [01] 12Dec05 tfl  lint fixes
**		 [02] 25Jan06 tfl  Added Multiply methods to support CNCSJP2FileView autoscale feature
 *******************************************************/

#ifndef NCSJPCBUFFER_H
#define NCSJPCBUFFER_H

#ifndef NCSJPCRECT_H
#include "NCSJPCRect.h"
#endif // NCSJPCRECT_H

#ifndef NCSMISC_H
#include "NCSMisc.h"
#endif
#ifndef NCSECWCLIENT_H
#include "NCSECWClient.h"
#endif
#ifndef NCSMEMPOOL_H
#include "NCSMemPool.h"
#endif // NCSMEMPOOL_H

#include <vector>

	/**
	 * CNCSJPCBuffer class - the base JPC scanline buffer.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.25 $ $Author: tom.lynch $ $Date: 2006/01/25 05:55:19 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCBuffer: public CNCSJPCRect {
public:
		/** Maximum UINT8 value */
	static const UINT32 NCS_MAXUINT8;
		/** Maximum UINT16 value */
	static const UINT32 NCS_MAXUINT16;
		/** Maximum UINT32 value */
	static const UINT32 NCS_MAXUINT32;
		/** Minimum UINT8 value */
	static const UINT32 NCS_MINUINT8;
		/** Minimum UINT16 value */
	static const UINT32 NCS_MINUINT16;
		/** Minimum UINT32 value */
	static const UINT32 NCS_MINUINT32;

		/** Maximum INT8 value */
	static const UINT32 NCS_MAXINT8;
		/** Maximum INT16 value */
	static const UINT32 NCS_MAXINT16;
		/** Maximum INT32 value */
	static const UINT32 NCS_MAXINT32;
		/** Minimum INT8 value */
	static const INT32 NCS_MININT8;
		/** Minimum INT16 value */
	static const INT32 NCS_MININT16;
		/** Minimum INT32 value */
	static const INT32 NCS_MININT32;
	
	/**
	 * SystemInfo class - Some basic system capabilities we care about.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.25 $ $Author: tom.lynch $ $Date: 2006/01/25 05:55:19 $ 
	 */	
	class NCSJPC_EXPORT_ALL SystemInfo {
	public:
			/** MMX Is present */
		bool	m_bMMXPresent;
			/** SSE is present */
		bool	m_bSSEPresent;
			/** SSE2 is present */
		bool	m_bSSE2Present;
			/** 3DNow is present */
		bool	m_b3DNowPresent;
			/** Bytes of physical memory on system */
		UINT64  m_nPhysicalMemBytes;
			/** Default constructor */
		SystemInfo();
			/** Current timestamp in US */
		static NCSTimeStampUs GetTime();
#ifdef WIN32
	private:
			/** high-resolution Timer frequency */
		static LARGE_INTEGER sm_iFrequency;
#endif
	};
		/** SystemInfo instance */
	static SystemInfo	sm_SystemInfo;
		/** Buffer copy US */
	static NCSTimeStampUs	sm_usCopy;
		/** buffer copy bytes */
	static UINT64			sm_nCopyBytes;
		/** buffer clear US */
	static NCSTimeStampUs	sm_usClear;
		/** buffer clear bytes */
	static UINT64			sm_nClearBytes;
		/** buffer Alloc US */
	static NCSTimeStampUs	sm_usAlloc;
		/** Current buffer alloc bytes */
	static UINT64			sm_nAllocBytes;
		/** buffer free US */
	static NCSTimeStampUs	sm_usFree;
	
	typedef enum {
			/** s16 integer buffer type */
		BT_INT16		= NCSCT_INT16,
			/** s32 integer buffer type */
		BT_INT32		= NCSCT_INT32,
			/** IEEE4 single precision float buffer type */
		BT_IEEE4		= NCSCT_IEEE4
	} Type;

		/** FLAG: This buffer owns resources */
	static const UINT32 AT_OWNER;	
		/** FLAG: A child node returned the buffer and will free it later */
	static const UINT32 AT_CHILD;	
		/** Buffer is locked and can't be assigned to. */
	static const UINT32 AT_LOCKED;	

	/**
	 * CNCSJPCBufferPool class - a rotating pool of buffers 
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.25 $ $Author: tom.lynch $ $Date: 2006/01/25 05:55:19 $ 
	 */	
	class CNCSJPCBufferPool {
	public:
			/** NCS memory pool */
		NCSPool	*m_pPool;
			/** Size */
		UINT32 m_nSize;

			/** Constructor */
		CNCSJPCBufferPool();
			/** Destructor */
		~CNCSJPCBufferPool();
			/** Set the CodeBlock size */
		void SetCBSize(Type eType, UINT32 nWidth, UINT32 nHeight);
			/** Alloc a buffer from the pool */
		void *Alloc();
			/** Return a buffer to the pool */
		void Free(void *p);
	};

		/** Buffer type */
	Type	m_eType;
		/** buffer flags */
	UINT32	m_Flags;

		/** Actual buffer pointer */
	void	*m_pBuffer;
		/** Pool - optional */
	CNCSJPCBufferPool	*m_pPool;

		/** Buffer step size to next line */
	int		m_nStep;

		/** Default constructor, initialises members */
	CNCSJPCBuffer();
		/** Copy constructor */
	CNCSJPCBuffer( const class CNCSJPCBuffer& src );
		/** constructor, specify rect values */
	CNCSJPCBuffer(UINT32 nWidth, Type eType);
		/** constructor, specify rect values */
	CNCSJPCBuffer(UINT32 nWidth, UINT32 nHeight, Type eType);
		/** constructor, specify rect values */
	CNCSJPCBuffer(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType);
		/** constructor, specify rect values */
	CNCSJPCBuffer(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType);
		/** Virtual destructor */
	virtual ~CNCSJPCBuffer();
	
		/** Assignment operator */
	class CNCSJPCBuffer& operator=( const class CNCSJPCBuffer& src );

		/** Alloc a buffer the specified width and type */
	bool Alloc(UINT32 nWidth, Type eType);
		/** Alloc a buffer the specified width, height and type */
	bool Alloc(UINT32 nWidth, UINT32 nHeight, Type eType);
		/** Alloc a buffer with the specified TL, BR and type */
	bool Alloc(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType);
		/** Alloc a buffer with the specified TL, width, height and type */
	bool Alloc(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType);
		/** Free the buffer */
	void Free();

		/** Assign a buffer to this buffer */
	bool Assign(Type eType, void *pPtr, INT32 nStep = 0, bool bFree = true);
		/** Assign a buffer to this buffer */
	bool Assign(UINT32 nWidth, Type eType, void *pPtr, INT32 nStep = 0, bool bFree = true);
		/** Assign a buffer to this buffer */
	bool Assign(UINT32 nWidth, UINT32 nHeight, Type eType, void *pPtr, INT32 nStep = 0, bool bFree = true);
		/** Assign a buffer to this buffer */
	bool Assign(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1, Type eType, void *pPtr, INT32 nStep = 0, bool bFree = true);
		/** Assign a buffer to this buffer */
	bool Assign(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight, Type eType, void *pPtr, INT32 nStep = 0, bool bFree = true);
		/** Release this buffer */
	void Release();

		/** Set flags */
	void SetFlag(UINT32 Flag) { m_Flags |= Flag; };
		/** Unset flags */
	void UnSetFlags(UINT32 Flags) { m_Flags &= ~Flags; };
		/** Get flags */
	UINT32 GetFlags() { return(m_Flags); };
		/** test flags */
	bool TestFlags(UINT32 Flag) { return((m_Flags & Flag) ? true : false); };

		/** 
		 * Return the size of a buffer in bytes (default 1 cell).
		 * @param		eType		Type of buffer
		 * @param		nWidth		Number of pixels in buffer.
		 * @return      UINT32		calculated buffer size in bytes.
		 */
	static NCS_INLINE UINT32 Size(Type eType, UINT32 nWidth = 1, UINT32 nHeight = 1) {
			UINT32 nLineSize = 0;
			switch(eType) {
				case BT_INT16:
						nLineSize = sizeof(INT16) * nWidth;
					break;
				case BT_INT32:
						nLineSize = sizeof(INT32) * nWidth;
					break;
				case BT_IEEE4:
						nLineSize = sizeof(IEEE4) * nWidth;
					break;
			}
			
			if(nHeight != 1 && nLineSize % 32) {
				// Pad scanlines to 32 byte multiples for performance
				// when the buffer is > 1 line high 
				nLineSize = (1 + nLineSize / 32) * 32;
			}
			return(nLineSize * nHeight);
		}
		/** Get buffer step in cells */
	NCS_INLINE UINT32 GetStep() { return(m_nStep); };
		/** Get buffer step in bytes */
	NCS_INLINE UINT32 GetStepBytes() { return(m_nStep * Size(m_eType)); };
		/** get buffer type */
	NCS_INLINE Type GetType() { return(m_eType); };
		/** get pointer to start of buffer (0,0) */
	NCS_INLINE void *GetPtr() { return(m_pBuffer); };
		/**	get pointer to buffer at (x,y) */
	NCS_INLINE void *GetPtr(INT32 x, INT32 y) { return((UINT8*)m_pBuffer + (y - GetY0()) * (INT32)GetStepBytes() + (x - GetX0()) * (INT32)Size(GetType())); };

		/** 
		 * Return the size of the Wavelet Tranform border for the specified pipeline type.
		 * @param		eType		Type of buffer
		 * @return      UINT32		Width/Height of border in CELLS.
		 */
	static NCS_INLINE UINT32 Border(Type eType) {
			switch(eType) {
				case BT_INT16:
				case BT_INT32:
						return(1);
					//break;
				case BT_IEEE4:
						return(2);
					//break;
			}
			return(0);
		}

		/** Clear buffer to 0 */
	void Clear();
		/** 
		 * Convert a line to a line of type T.
		 * @param		pDst		Pointer to Destination buffer
		 * @return      bool		true on succes, else false.
		 */
	template <class T> bool Convert(T *pDst, UINT8 nStep = 1) {
			if(m_pBuffer && pDst) {
				UINT32 x = 0;
				UINT32 nWidth = GetWidth();

				if(nStep == 1) {
					// Single band
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								FLT_TO_INT_INIT();
									for(x = 0; x < nWidth; x++) {
										pDst[x] = (T)NCSfloatToInt32_RM(((IEEE4*)m_pBuffer)[x]);
					//					pDst[x] = (T)NCSFloor(((IEEE4*)m_pBuffer)[x]);
									}
								FLT_TO_INT_FINI();
							break;
					}
				} else if(nStep == 3) {
					// 3 band - eg RGB
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 3] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 3] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								FLT_TO_INT_INIT();
									for(x = 0; x < nWidth; x++) {
										pDst[x*3] = (T)NCSfloatToInt32_RM(((IEEE4*)m_pBuffer)[x]);
									}
								FLT_TO_INT_FINI();
							break;
					}
				} else if(nStep == 4) {
					// 4 band, eg RGBA
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 4] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 4] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								FLT_TO_INT_INIT();
									for(x = 0; x < nWidth; x++) {
										pDst[x*4] = (T)NCSfloatToInt32_RM(((IEEE4*)m_pBuffer)[x]);
									}
								FLT_TO_INT_FINI();
					}
				} else {
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * nStep] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * nStep] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								FLT_TO_INT_INIT();
									for(x = 0; x < nWidth; x++) {
										pDst[x*nStep] = (T)NCSfloatToInt32_RM(((IEEE4*)m_pBuffer)[x]);
									}
								FLT_TO_INT_FINI();
							break;
					}
				}
				return(true);
			}
			return(false);
		}
		/** 
		 * Convert a line to a line of type T.
		 * @param		pDst		Pointer to Destination buffer
		 * @return      bool		true on succes, else false.
		 */
	template <class T> bool ConvertFLT(T *pDst, UINT8 nStep = 1) {
			if(m_pBuffer && pDst) {
				UINT32 x = 0;
				UINT32 nWidth = GetWidth();

				if(nStep == 1) {
					// Single band
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								for(x = 0; x < nWidth; x++) {
									pDst[x] = (T)((IEEE4*)m_pBuffer)[x];
								}
							break;
					}
				} else if(nStep == 3) {
					// 3 band - eg RGB
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 3] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 3] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 3] = (T)((IEEE4*)m_pBuffer)[x];
								}
							break;
					}
				} else if(nStep == 4) {
					// 4 band, eg RGBA
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 4] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 4] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								for(x = 0; x < nWidth; x++) {
									pDst[x * 4] = (T)((IEEE4*)m_pBuffer)[x];
								}
							break;
					}
				} else {
					switch(GetType()) {
						case CNCSJPCBuffer::BT_INT16:
								for(x = 0; x < nWidth; x++) {
									pDst[x * nStep] = (T)((INT16*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_INT32:
								for(x = 0; x < nWidth; x++) {
									pDst[x * nStep] = (T)((INT32*)m_pBuffer)[x];
								}
							break;
						case CNCSJPCBuffer::BT_IEEE4:
								for(x = 0; x < nWidth; x++) {
									pDst[x * nStep] = (T)((IEEE4*)m_pBuffer)[x];
								}
							break;
					}
				}
				return(true);
			}
			return(false);
		}
		/** 
		 * Read buffer to a type T.
		 * @param		pDst		Pointer to Destination buffer
		 * @return      bool		true on succes, else false.
		 */
	template <class T> bool Read(T *pDst, UINT8 nStep = 1) {
			for(UINT32 y = 0; y < GetHeight(); y++) {
				Convert(pDst, nStep);
			}
			return(true);
		}
		/** 
		 * Read buffer to a (IEEE4/IEEE8) type T.
		 * @param		pDst		Pointer to Destination buffer
		 * @return      bool		true on succes, else false.
		 */
	template <class T> bool ReadFLT(T *pDst, UINT8 nStep = 1) {
			for(UINT32 y = 0; y < GetHeight(); y++) {
				ConvertFLT(pDst, nStep);
			}
			return(true);
		}

		/** Read value(s) from the buffer at the specified position, size */
	NCS_INLINE bool Read(void *pValue, INT32 nX, INT32 nY = 0, UINT32 nWidth = 1, UINT32 nHeight = 1, UINT32 nStep = 1) {
			UINT32 x, y;
			switch(m_eType) {
				case BT_INT16:
						for(y = 0; y < nHeight; y++) {
							for(x = 0; x < nWidth; x++) {
								*((INT16*)pValue + nStep * y + x) = *((INT16*)m_pBuffer + m_nStep * ((nY - m_Y0) + y) + (nX - m_X0) + x);
							}
						}
					break;
				case BT_INT32:
						for(y = 0; y < nHeight; y++) {
							for(x = 0; x < nWidth; x++) {
								*((INT32*)pValue + nStep * y + x) = *((INT32*)m_pBuffer + m_nStep * ((nY - m_Y0) + y) + (nX - m_X0) + x);
							}
						}
					break;
				case BT_IEEE4:
						for(y = 0; y < nHeight; y++) {
							for(x = 0; x < nWidth; x++) {
								*((IEEE4*)pValue + nStep * y + x) = *((IEEE4*)m_pBuffer + m_nStep * ((nY - m_Y0) + y) + (nX - m_X0) + x);
							}
						}
					break;
			}
			return(true);
		}
		/** Read from the buffer to the destination buffer, copying only the intersecting area */
	NCS_INLINE bool Read(CNCSJPCBuffer *pDst) {
			CNCSJPCRect r(NCSMax(GetX0(), pDst->GetX0()),
						  NCSMax(GetY0(), pDst->GetY0()),
						  NCSMin(GetX1(), pDst->GetX1()),
						  NCSMin(GetY1(), pDst->GetY1()));
			if(pDst->GetType() == GetType()) {
				return(Copy(r.GetWidth(), r.GetHeight(), 
					(UINT8*)pDst->GetPtr() + (r.GetY0() - pDst->GetY0()) * pDst->GetStepBytes() + Size(m_eType, r.GetX0() - pDst->GetX0()), 
					pDst->GetStepBytes(), 
					(UINT8*)GetPtr() + (r.GetY0() - GetY0()) * GetStepBytes() + Size(m_eType, r.GetX0() - GetX0()), 
					GetStepBytes(), 
					m_eType));
			} else {
				for(UINT32 y = 0; y < r.GetHeight(); y++) {
					CNCSJPCBuffer tmp;
					tmp.Assign(r.GetX0(), r.GetY0(), r.GetWidth(), r.GetHeight(), GetType(), (UINT8*)GetPtr() + (y + r.GetY0() - GetY0()) * GetStepBytes() + Size(m_eType, r.GetX0() - GetX0()));
					switch(pDst->GetType()) {
						case BT_INT16:
								tmp.Convert((INT16*)((UINT8*)pDst->GetPtr() + (y + r.GetY0() - pDst->GetY0()) * pDst->GetStepBytes() + Size(m_eType, r.GetX0() - pDst->GetX0())));
							break;
						case BT_INT32:
								tmp.Convert((INT32*)((UINT8*)pDst->GetPtr() + (y + r.GetY0() - pDst->GetY0()) * pDst->GetStepBytes() + Size(m_eType, r.GetX0() - pDst->GetX0())));
							break;
						case BT_IEEE4:
								tmp.ConvertFLT((IEEE4*)((UINT8*)pDst->GetPtr() + (y + r.GetY0() - pDst->GetY0()) * pDst->GetStepBytes() + Size(m_eType, r.GetX0() - pDst->GetX0())));
							break;
					}
					tmp.Release();
				}
			}
			return(true);
		}

		/** 
		 * Clip valus in the buffer to the specified range.
		 * @param		nMin		Min value
		 * @param		nMax		Max value
		 * @return      bool		true on success, else false & error set.
		 */
	bool Clip(INT32 nMin, INT32 nMax);
		/** 
		 * Clip valus in a buffer to the specified range.
		 * @param		fMin		Min value
		 * @param		fMax		Max value
		 * @return      bool		true on success, else false & error set.
		 */
	bool Clip(IEEE4 fMin, IEEE4 fMax);
		/** 
		 * Add a constant value to the buffer.
		 * @param		nValue		Constant value to add
		 * @param		nScale		Scale factor, *= NCS2Pow(-nScale)
		 * @return      bool		true on succes, else false.
		 */
	bool Add(const INT32 nValue, INT16 nScale = 0);
		/** 
		 * Add a constant value to the line.
		 * @param		fValue		Constant value to add
		 * @param		nScale		Scale factor, *= fScale
		 * @return      bool		true on succes, else false.
		 */
	bool Add(const IEEE4 fValue, INT16 nScale = 0);
		/** 
		 * Change bit depth (data range) of a buffer
		 * @param		nCurrentBits	Current bit depth
		 * @param		nNewBits		New bit depth
		 * @return      bool		true on succes, else false.
		 */
	bool OffsetShift(const INT32 nValue, UINT8 nCurrentBits, UINT8 nNewBits);
		/**
		 * Multiply the contents of the buffer by an integer.
		 * @param		nValue		Multiplication factor
		 * @return		bool		true on success, else false.
		 */
	bool Multiply(const IEEE4 fValue);
		/**
		 * Multiply the contents of the buffer by an integer.
		 * @param		nValue		Multiplication factor
		 * @return		bool		true on success, else false.
		 */
	bool Multiply(const INT32 nValue);
		/** 
		 * Copy from one pointer to another (may overlap).
		 * @param		nWidth		Width in pixels.
		 * @param		nHeight		Height in pixels.
		 * @param		pDst		Dest buffer
		 * @param		nDstStep	Dest buffer step size
		 * @param		pSrc		Src buffer
		 * @param		nSrcStep	Src buffer step size
		 * @param		eType		Buffer Type
		 * @return      bool		true on success, else false & error set.
		 */
	static bool Copy(UINT32 nWidth, UINT32 nHeight, void *pDst, INT32 nDstStep, const void *pSrc, INT32 nSrcStep, Type eType);

protected:

private:
	static CNCSJPCBufferPool sm_Pool;

	bool Alloc();
};

typedef std::vector<CNCSJPCBuffer> CNCSJPCBufferVector;

class NCSJPC_EXPORT_ALL CNCSJPCBufferCache {
public:
	virtual ~CNCSJPCBufferCache() {};
	virtual void SetBuffers(UINT32 nBuffers);
	virtual CNCSJPCBuffer *GetBuffer(UINT32 nBuffer);
	virtual CNCSJPCBuffer *FindBuffer(UINT32 &nBuffer, CNCSJPCRect &r, bool &bFound, CNCSJPCBuffer::Type eType);
private:
	CNCSJPCBufferVector m_Buffers;
};

#endif // !NCSJPCBUFFER_H
