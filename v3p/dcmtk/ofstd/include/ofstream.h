/*
 *
 *  Copyright (C) 2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: C++ header to handle standard and old stream libraries.
 *
 */


#ifndef __OFSTREAM_H
#define __OFSTREAM_H

#include <iostream>
#include <ios>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "osconfig.h"

#ifdef USE_STD_CXX_INCLUDES

#ifdef HAVE_IOS
#endif
// For standard STREAMS library: preference for standard stringstream
#if defined(HAVE_SSTREAM)
#define USE_STRINGSTREAM
#elif defined(HAVE_STRSTREAM)
#include <strstream>
#else
#error DCMTK needs stringstream or strstream type
#endif
#ifdef HAVE_STD_NAMESPACE
namespace std { };
using namespace std;
#endif

#else /* USE_STD_CXX_INCLUDES */

#include <iostream.h>
#include <fstream.h>
// For old STREAMS library: preference for strstream
#if defined(HAVE_STRSTREA_H) || defined(HAVE_STRSTREAM_H)
#ifdef HAVE_STRSTREA_H
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#elif defined(HAVE_SSTREAM_H)
#include <sstream.h>
#define USE_STRINGSTREAM
#else
#error DCMTK needs stringstream or strstream type
#endif
#include <iomanip.h>

#endif

#ifdef USE_STRINGSTREAM

typedef stringstream OFStringStream;
typedef ostringstream OFOStringStream;
typedef istringstream OFIStringStream;

#define OFStringStream_ends ""
#define OFSTRINGSTREAM_GETOFSTRING(oss, string) \
    OFString string((oss).str().c_str());
// The following two macros define a block structure. Please note that variables
// declared between xxx_GETSTR and xxx_FREESTR are only valid within this scope.
#define OFSTRINGSTREAM_GETSTR(oss, chptr) \
{ \
    string chptr##__ = (oss).str(); \
    const char *chptr = chptr##__.c_str();
#define OFSTRINGSTREAM_FREESTR(chptr) \
}

#else /* USE_STRINGSTREAM */

typedef strstream OFStringStream;
typedef ostrstream OFOStringStream;
typedef istrstream OFIStringStream;

#define OFStringStream_ends ends
#define OFSTRINGSTREAM_GETOFSTRING(oss, string) \
    char * string##__ = (oss).str(); \
    OFString string(string##__); \
    delete[] string##__;
// The following two macros define a block structure. Please note that variables
// declared between xxx_GETSTR and xxx_FREESTR are only valid within this scope.
#define OFSTRINGSTREAM_GETSTR(oss, chptr) \
{ \
    const char *chptr = (oss).str();
#define OFSTRINGSTREAM_FREESTR(chptr) \
    delete[] (char *)chptr; \
}

#endif /* USE_STRINGSTREAM */

#endif /* USE_STD_CXX_INCLUDES */
