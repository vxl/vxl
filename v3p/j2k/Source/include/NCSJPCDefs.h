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
** FILE:     $Archive: /NCS/Source/include/NCSJPCDefs.h $
** CREATED:  05/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPC Defines
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCDEFS_H
#define NCSJPCDEFS_H

//
// Disable dll-interface warning, compiler gives it on protected/private members.
// Disable truncated name mangling warning
//
#ifdef _MSC_VER
#pragma warning( disable : 4251 4786 4717 4275 4163)
#endif

//
// Remove unnecessary stubs to reduce binary size
//
#define NCSJPC_LEAN_AND_MEAN


#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif // NCSDEFS_H

#if defined(WIN32)&&!defined(_WIN32_WCE)&&((defined(_X86_) && defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 12008804))||(defined(_AMD64_) && defined(_MSC_VER) && _MSC_VER >= 1400))
//
// X86 "Multi Media Intrinsics" - ie, MMX, SSE, SSE2 optimisations
//
// Only defined if Win32, X86 and VC6 Processor Pack or newer _OR_ AMD64 and >= VS.NET 2005/AMD64 PlatformSDK compiler (ie, SSE intrinsics support in the compiler)
//
#define NCSJPC_X86_MMI
#if !defined(_AMD64_)
#define NCSJPC_X86_MMI_MMX

// Prevent mmintrin.h from being included
//#define _MMINTRIN_H_INCLUDED
#endif

#endif

//
// Use LCMS for ICC->RGB conversions, supports both
// restricted and full ICC profiles.
//
#define NCSJPC_USE_LCMS

//
// Use TinyXML for XML DOM Parsing
//
#define NCSJPC_USE_TINYXML

//
// Include ECW Decompression in lib
//
#define NCSJPC_ECW_SUPPORT

//
// Include ECW Compression in lib
//
#define ECW_COMPRESS

#ifndef NCSJPC_EXPORT
#define NCSJPC_EXPORT NCS_EXPORT
#endif // NCSJPC_EXPORT
#ifndef NCSJPC_EXPORT_ALL
#define NCSJPC_EXPORT_ALL NCS_EXPORT
#endif // NCSJPC_EXPORT_ALL

#define NCSJPC_2POW11 2048

#define NCSJP2_STRIP_HEIGHT	64

#endif // NCSJPCDEFS_H
