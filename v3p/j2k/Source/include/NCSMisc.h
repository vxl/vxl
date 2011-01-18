/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCSMisc.h
** CREATED:	Wed Oct 13 09:19:00 WST 1999
** AUTHOR: 	David Hayward
** PURPOSE:	Miscellaneous prototypes of useful functions
**			for the public SDKs.
**
**			NOTE: Must be kept in sync with the private
**				  includes.
**
** EDITS:
** [01] 	08-12-05 tfl lint fixes
 *******************************************************/

#ifndef NCSMISC_H
#define NCSMISC_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif

#ifdef _WIN32_WCE
#define NCSMin MIN
#define NCSMax MAX
#elif defined POSIX
#define NCSMin(a, b)   ((a) > (b) ? (b) : (a))
#define NCSMax(a, b)   ((a) < (b) ? (b) : (a))
#else
#define NCSMin min
#define NCSMax max
#endif // _WIN32_WCE


typedef enum {
	NCS_UNKNOWN	= 0,	/* Don't know what this platform is 	*/
	NCS_WINDOWS_9X	= 1,	/* Windows 9x (95, 98)			*/
	NCS_WINDOWS_NT	= 2,	/* Windows NT (NT4, 2000)		*/
	NCS_WINDOWS_CE	= 3,	/* Windows CE (CE, PocketPC)		*/
	NCS_MACINTOSH	= 4,	/* Macintosh (Sys 8/9)			*/
	NCS_MACINTOSH_OSX= 5,	/* Macintosh OSX			*/
	NCS_LINUX	= 6,	/* Linux				*/
	NCS_PALM	= 7,	/* PalmOS (v2+)				*/
	NCS_SOLARIS	= 8,	/* Solaris 2.5+				*/
	NCS_HPUX	= 9	/* HP-UX 11.0(64bit)			*/
} NCSPlatform;

void NCSFormatSizeText(INT64 nSizeBytes, char *buf);
NCSPlatform NCSGetPlatform(void);

/*
**	[06] Fast Float to UINT8 conversion logic
*/
#if (defined(WIN32) && defined(_X86_))

#define FLT_TO_INT_INIT() { unsigned int old_controlfp_val = _controlfp(_RC_NEAR, _MCW_RC)
#define FLT_TO_INT_INIT_FLR() { unsigned int old_controlfp_val = _controlfp(_RC_DOWN, _MCW_RC)
#define FLT_TO_INT_FINI() _controlfp(old_controlfp_val, _MCW_RC); }

#define FLT_TO_UINT8(a, b)								\
	{                                                   \
		INT32 FLT_TO_INT_rval;							\
		__asm                                           \
		{                                               \
			__asm fld dword ptr [b]                     \
			__asm fistp dword ptr [FLT_TO_INT_rval]     \
		}                                               \
		a = FLT_TO_INT_rval;							\
	}
#define FLT_TO_INT32(a, b)						\
		__asm                                   \
		{                                       \
			__asm fld dword ptr [b]             \
			__asm fistp dword ptr [a]           \
		}

#else	/* WIN32 && _X86_ */

#define FLT_TO_INT_INIT() { 
#define FLT_TO_INT_FINI()  }

#ifdef MACOSX	/**[16]**/

#define FLT_TO_INT32(a,b) a = rint(b)
//#define FLT_TO_UINT8(a, b) a = (UINT8) b
// rar (24-1-01): added from ECW mac port
#define FLT_TO_UINT8(a,b) 							\
	{ 												\
	UINT32	_x;										\
	FLT_TO_INT32(_x,b);								\
	a = (UINT8)_x;									\
	}
				   
#else	/* MACINTOSH */

#define FLT_TO_UINT8(a, b)								\
	   { a = (UINT8) b; }
#define FLT_TO_INT32(a,b)								\
	   { a = (INT32) b; }

#endif	/* MACINTOSH */
#endif	/* WIN32 && _X86_ */

#define NCS_INLINE_FUNCS
#ifdef NCS_INLINE_FUNCS

#if (defined(WIN32) && defined(_X86_))

static NCS_INLINE INT32 NCSfloatToInt32_RM(IEEE4 f) {
	INT32 i32;
	FLT_TO_INT32(i32, f);
	return(i32);
}

static NCS_INLINE INT32 NCSdoubleToInt32_RM(IEEE8 x) {
	return(NCSfloatToInt32_RM((IEEE4)x));
}

// Convert a float between 0.0 and 1.0 to an INT32
static NCS_INLINE INT32 NCSfloatToInt32_0_1(IEEE4 x)
{
    IEEE4 y = x + 1.f;
    return((*(INT32 *)&y) & 0x7FFFFF);	// last 23 bits
}

// Convert a float to an INT32
static NCS_INLINE INT32 NCSfloatToInt32(IEEE4 x)
{
	INT32 FltInt = *(INT32 *)&x;
	INT32 MyInt;
	INT32 mantissa = (FltInt & 0x07fffff) | 0x800000;
	INT32 exponent = 150 - ((FltInt >> 23) & 0xff);

	if (exponent < -(8*(int)sizeof(mantissa)-1)) {
		MyInt = (mantissa << (8*(int)sizeof(mantissa)-1));		      
	} else if(exponent < 0) {
		MyInt = (mantissa << -exponent);		      
	} else if(exponent > (8*(int)sizeof(mantissa)-1)) {
		MyInt = (mantissa >> (8*(int)sizeof(mantissa)-1));
	} else {
		MyInt = (mantissa >> exponent);
	}

	if (FltInt & 0x80000000)
		MyInt = -MyInt;
	return(MyInt);
}

// Convert a double to an INT32
static NCS_INLINE INT32 NCSdoubleToInt32(IEEE8 x)
{
	INT64 DblInt = *(INT64 *)&x;
	INT32 MyInt;
	INT64 mantissa = (DblInt & 0xfffffffffffff) | 0x10000000000000;
	INT32 exponent = (INT32)(1075 - ((DblInt >> 52) & 0x7ff));

	if (exponent < -(8*(int)sizeof(mantissa)-1)) {
		MyInt = (INT32)(mantissa << (8*(int)sizeof(mantissa)-1));		      
	} else if(exponent < 0) {
		MyInt = (INT32)(mantissa << -exponent);		      
	} else if(exponent > (8*(int)sizeof(mantissa)-1)) {
		MyInt = (INT32)(mantissa >> (8*(int)sizeof(mantissa)-1));
	} else {
		MyInt = (INT32)(mantissa >> exponent);
	}

	if (DblInt & 0x8000000000000000)
		MyInt = -MyInt;
	return(MyInt);
}

#else // WIN32 & X86

#define NCSfloatToInt32_RM(f) ((INT32)(f))
#define NCSfloatToInt32_0_1(x) ((INT32)(x))
#define NCSfloatToInt32(x) ((INT32)(x))
#define NCSdoubleToInt32(x) ((INT32)(x))
#define NCSdoubleToInt32_RM(x) ((INT32)(x))

#endif // WIN32 && X86

static NCS_INLINE INT32 NCSCeil(double a)
{
	if(a >= 0.0) {
		INT32 v = NCSdoubleToInt32(a);
		return(v + ((a != v) ? 1 : 0));
	} else {
		return(NCSdoubleToInt32(a));
	}
}

static NCS_INLINE INT32 NCSFloor(double a)
{
	if(a >= 0.0) {
		return(NCSdoubleToInt32(a));
	} else {
		INT32 v = NCSdoubleToInt32(a);
		return(v - ((a != v) ? 1 : 0));		
	}
}

static NCS_INLINE INT32 NCSCeilDiv(INT32 n, INT32 d)
{
	if(d == 0) {
		return(0x7fffffff);
	} else if(n >= 0 && d > 0) {
		return((n / d + ((n % d) ? 1 : 0)));
	} else {
		return(n / d);
	}
//	if(n < 0 || d < 0) {
//		return((INT32)ceil(n / (double)d));
//	} else {
//		return((n / d + ((n % d) ? 1 : 0)));
//	}
}

static NCS_INLINE INT32 NCSFloorDiv(INT32 n, INT32 d)
{
	switch(d) {
		case 1: return(n); 
			//break;
		case 2: return(n >> 1); 
			//break;
		case 4: return(n >> 2); 
			//break;
		default:
				if(n < 0 || d < 0) {
					return((INT32)NCSFloor(n / (double)d));
				} else {
					return(n / d);
				}
			//break;
	}
}

static NCS_INLINE UINT32 NCS2Pow(UINT32 n)
{
//	return(pow(2, n));
	return(1 << n);
}

static NCS_INLINE IEEE8 NCS2PowS(INT32 n)
{
//	return(pow(2, n));
	if(n >= 0) {
		return((IEEE8)(1 << n));
	} else {
		return(1.0 / (1 << -n));
	}
}

static NCS_INLINE INT32 NCSLog2(INT32 n)
{
    INT32 nLog;
    for(nLog = 0; n > 1; nLog++) {
        n >>= 1;
    }
    return nLog;
}

static NCS_INLINE UINT64 NCSAbs(INT64 a)
{
//	return(abs(a));
	return((a < 0) ? -a : a);
}

static NCS_INLINE BOOLEAN NCSIsPow2(UINT32 nValue) 
{
	if(NCS2Pow(NCSLog2(nValue)) == nValue) {
		return(TRUE);
	}
	return(FALSE);
}

#else

#ifdef __cplusplus
extern void *NCSNew(INT32 nSize, bool bClear = false);
extern void NCSDelete(void *p);
#endif // __cplusplus

extern INT32 NCSCeil(double a);
extern INT32 NCSFloor(double a);
extern INT32 NCSCeilDiv(INT32 n, INT32 d);
extern INT32 NCSFloorDiv(INT32 n, INT32 d);
extern UINT32 NCS2Pow(UINT32 n);
extern IEEE8 NCS2PowS(INT32 n);
extern UINT64 NCSAbs(INT64 a);

#endif /* NCS_INLINE_FUNCS */
// #endif // !MACOSX

#ifdef __cplusplus
}
#endif
#endif /* NCSMISC_H */
