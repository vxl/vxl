// Source

//================ GE Aerospace NITF support libraries =================
//
// Description:	This file defines the NITF headers for NITF v2.0.
//
//  $Revision: 1.1 $ 
//  $Date: 2003/12/26 00:20:23 $
//  $Author: mlaymon $
//
// For a given header 'head', head->Read(file) is called when the
// appropriate place in the file is reached, and it will return a
// StatusCode indicating success or failure.
//
// Written by:       Lynn Bigelow
// Date:             July, 1993
//
//  Modified by J. Mundy Feb. 1998               
//  Made the ICAT and IFC fields accept spaces as well as the 
//  standard entries.  Needed to ingest Northrup Grummond images
//=====================lkbjfcbtdddhtargbskmtaps=======================

#ifndef	_vil_nitf_header_v20_h
#define _vil_nitf_header_v20_h

#include "vil_nitf_message_header_v20.h"
#include "vil_nitf_image_subheader_v20.h"

#if 0  // NOT IMPLEMENTED

#ifndef _NITFv20LabelSubHeader_h
#  define _NITFv20LabelSubHeader_h
#  define LabelSubHeader_v11 LabelSubHeader_v20
#  undef  _NITFv11LabelSubHeader_h
#  include "ge_file_formats/NITFv11LabelSubHeader.h"
#  undef LabelSubHeader_v11
#endif

#ifndef _vil_nitf_symbol_subheader_v20_h
#define _vil_nitf_symbol_subheader_v20_h
#define vil_nitf_symbol_subheader_v11 vil_nitf_symbol_subheader_v20
#  undef  _vil_nitf_symbol_subheader_v11_h
#  include "vil_nitf_symbol_subheader_v11.h"
#  undef vil_nitf_symbol_subheader_v11
#endif

#ifndef _NITFv20TextSubHeader_h
#  define _NITFv20TextSubHeader_h
#  define TextSubHeader_v11 TextSubHeader_v20
#  undef  _NITFv11TextSubHeader_h
#  include "ge_file_formats/NITFv11TextSubHeader.h"
#  undef TextSubHeader_v11
#endif

#endif

//====================================================================
// The Audio and NPI Sub Headers are not defined for NITF v2.0.
// Instead, it has a Data Extension Segment and a Reserved Extension
// Segment.  The Reserved Extension Segment is not currently used.
//====================================================================

// NITFv20 objects do not implement the Ascii methods

#if 0  // NOT IMPLEMENTED

inline StatusCode LabelSubHeader_v20::AsciiWrite (vil_stream*) {return (STATUS_BAD);}
inline StatusCode LabelSubHeader_v20::AsciiRead (vil_stream*)  {return (STATUS_BAD);}
inline StatusCode TextSubHeader_v20::AsciiRead (vil_stream*)   {return (STATUS_BAD);}
inline StatusCode TextSubHeader_v20::AsciiWrite (vil_stream*)  {return (STATUS_BAD);}

#endif

// Format for strftime
#ifdef _MSC_VER
   static const char *date_format =  "%d%H%M%SZ%b%y";
#else
   static const char *date_format =  "%d%H%M%SZ%h%y";
#endif

inline const char* GetVersion20Date()
{
    static char rval[15];

    // We must fool SCCS, otherwise it will try to
    // be smart and interpret the percents in the string.
    // 
    time_t clock;
    struct tm *tm;
    clock = time (NULL) ;
    tm = localtime (&clock) ;
    // char* format = "%d%H%M%SZ%h%y" ;

    strncpy (rval, "              ", 15);
    strftime (rval, 15, date_format, tm);

    return (rval) ;
}

#endif  // _vil_nitf_header_v20_h
