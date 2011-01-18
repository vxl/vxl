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
** FILE:     $Archive: /NCS/Source/include/NCSJPCTypes.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPC additional Types header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCTYPES_H
#define NCSJPCTYPES_H

#include "NCSDefs.h"

#ifndef NCSJPCDEFS_H
#include "NCSJPCDefs.h"
#endif // NCSJPCDEFS_H

#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif // NCSMALLOC_H
#ifndef NCSERROR_H
#include "NCSError.h"
#endif // NCSERROR_H

// MMI headers
#ifdef NCSJPC_X86_MMI

#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef NCSJPC_X86_MMI_MMX

// Temporarily disable waning C4799 to keep VC happy
#pragma warning( disable : 4799 )

__forceinline __m128 _mm_cvtpi16_ps__INLINE(__m64 a)
{
  __m128 tmp;
  __m64  ext_val = _mm_cmpgt_pi16(_mm_setzero_si64(), a);

  tmp = _mm_cvtpi32_ps(_mm_setzero_ps(), _mm_unpackhi_pi16(a, ext_val));
  return(_mm_cvtpi32_ps(_mm_movelh_ps(tmp, tmp), 
                        _mm_unpacklo_pi16(a, ext_val)));
}
__forceinline __m64 _mm_cvtps_pi16__INLINE(__m128 a)
{
  return _mm_packs_pi32(_mm_cvtps_pi32(a), 
                        _mm_cvtps_pi32(_mm_movehl_ps(a, a)));
}

#pragma warning( default : 4799 )

#endif //NCSJPC_X86_MMI_MMX
#endif //NCSJPC_X86_MMI

// STL headers we need
#include <vector>
#include <map>
#include <list>
#include <memory>

// stdarg header.
#include <stdarg.h>

#ifdef SOLARIS
#include <sys/varargs.h>
#endif // POSIX

	/** packet ID type */
typedef NCSBlockId NCSJPCPacketId;	

	/** SubBand enumerated type */
typedef enum {
		/** LowLow subband */
	NCSJPC_LL		= 0,
		/** HighLow - horizontally high-pass subband */
	NCSJPC_HL		= 1,
	/** LowHigh - vertically high-pass subband */
	NCSJPC_LH		= 2,
		/** HighHigh */
	NCSJPC_HH		= 3
} NCSJPCSubBandType;

/**
 * Cached value template.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.30 $ $Author: PCDEV\simon $ $Date: 2006/01/11 06:21:29 $ 
 */	
template <class T> class NCSJPC_EXPORT TNCSCachedValue {
public:
	bool m_bCached;
	T	m_Value;

	TNCSCachedValue() { Invalidate(); };
	TNCSCachedValue(const TNCSCachedValue<T> &s) { *this = s; };

	NCS_INLINE void Invalidate() { m_bCached = false; };
	NCS_INLINE bool Cached() { return(m_bCached); };

	NCS_INLINE T& operator=(const T& s) {
			m_bCached = true;
			m_Value = s;
			return(m_Value);
	   };
	NCS_INLINE operator T() {
			return(m_Value);
	   };
		/** 
		 * EQ operator.
		 * @return      bool		TF is equal.
		 */
	NCS_INLINE bool operator==(const TNCSCachedValue<T> &s) const {
		return(m_Value == s.m_Value);
	}
		/** 
		 * NEQ operator.
		 * @return      bool		TF is NOT equal.
		 */
	NCS_INLINE bool operator!=(const TNCSCachedValue<T> &s) const {
		return(m_Value != s.m_Value);
	}
		/** 
		 * LT operator.
		 * @return      bool		TF is LT.
		 */
	NCS_INLINE bool operator<(const TNCSCachedValue<T> &s) const {
		return(m_Value < s.m_Value);
	}
};

/**
 * NCSUUID - UUID class.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.30 $ $Author: PCDEV\simon $ $Date: 2006/01/11 06:21:29 $ 
 */	
class NCSJPC_EXPORT NCSUUID {
public:
	UINT8 m_UUID[16];
	
	NCSUUID() {
		memset(m_UUID, 0, sizeof(m_UUID));
	};
	NCSUUID(const UINT8 Src[16]) {
		memcpy(m_UUID, Src, sizeof(m_UUID));
	};
	NCSUUID(const NCSUUID &Src) {
		memcpy(m_UUID, Src.m_UUID, sizeof(m_UUID));
	};
	virtual ~NCSUUID() {};

	NCS_INLINE bool operator==(const NCSUUID &s) {
		return(memcmp(m_UUID, s.m_UUID, sizeof(m_UUID)) == 0);
	}
	NCS_INLINE bool operator!=(const NCSUUID &s) {
		return(memcmp(m_UUID, s.m_UUID, sizeof(m_UUID)) != 0);
	}
};

/**
 * CNCSJPCGlobalLock - A global lock (mutex) class.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.30 $ $Author: PCDEV\simon $ $Date: 2006/01/11 06:21:29 $ 
 */
class NCS_EXPORT CNCSJPCGlobalLock {
public:
	CNCSJPCGlobalLock();
	CNCSJPCGlobalLock(bool bAutoLock);
	virtual ~CNCSJPCGlobalLock();
	void Lock(void);
	bool TryLock(void);
	void UnLock(void);
private:
	bool m_bAutoLock;
};

/**
 * CNCSJPCNodeVector - vector of CNCSJPCNodes.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.30 $ $Author: PCDEV\simon $ $Date: 2006/01/11 06:21:29 $ 
 */
typedef std::vector<class CNCSJPCNode*> CNCSJPCNodeVector;

#endif // NCSJPCTYPES_H

