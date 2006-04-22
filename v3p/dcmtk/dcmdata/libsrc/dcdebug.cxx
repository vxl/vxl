/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Print debug information
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDARG
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "dcdebug.h"
#include "ofconsol.h"

OFGlobal<int> DcmDebugLevel(0);

#ifdef DEBUG

void debug_print(const char* text, ... )
{
#ifdef HAVE_VPRINTF
    char buf[4096]; // we hope that debug messages are never larger than 4K.
    va_list argptr;
    va_start( argptr, text );
    vsprintf(buf, text, argptr );  // vsnprintf is better but not available everywhere
    va_end( argptr );
    // we don't lock or unlock the console because this is already done by the calling debug or Cdebug macro.
    ofConsole.getCerr() << buf << endl;
#else
#error Need vprintf to compile this code
#endif
}

#endif
