// This is core/vil/file_formats/vil_nitf_util.h

//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief This file defines utility methods for NITF.
//
// For a given header 'head', head->Read (file) is called when the
// appropriate place in the file is reached, and it will return a
// StatusCode indicating success or failure.
//
// \author  mlaymon
// \date 2003/12/26
//
// Written by:       Burt Smith
// Date:             August 26, 1992
//
//====================================================================

#ifndef vil_nitf_util_h_
#define vil_nitf_util_h_

#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vxl_config.h>
#include <vil/vil_stream.h>

// PUT MACROS FOR STRCPY AND STRNCPY IN SEPARATE FILE FOR NOW.
#include "vil_nitf_macro_defs.h"

//: Function to delete characters c from s.
//  Options are:
//
//  \param s input string
//  \param c character to remove
//  \param optflag flag indicating how to remove characters
//    - ALL:            delete all characters c.
//    - TRAILING:       delete trailing characters c.
//    - LEADING:        delete leading characters c.
//    - HEAD/TAIL:      delete all leading && trailing characters c.
//
char * squeeze(char*, int, int);
void clear_string_vector(vcl_vector<vcl_string *> str_vector);
void display_as_hex(
    const unsigned char * buffer,
    unsigned int display_count,
    unsigned int bytes_per_value,
    vcl_string label = "");
void display_as_hex(const unsigned char value);

// Two functions below are from TargetJr file
//    .../IUPackages/GeneralUtility/Basics/geopt_util.C

enum geopt_coord {LAT, LON};

int geostr_to_latlon(const char* str, double* lat, double* lon);
int geostr_to_double(const char* str, double* val, geopt_coord lat_lon_flag);

//:
// this inline assumes b, v and f are valid.  It does not
// change the value of v unless the read is successful.
//
// \param cbuf input buffer
// \param ival pointer to integer value
// \param count number of digits to read for ival
// \param f pointer to stream from which characters will be read
// \param preserve flag indicating if original value should be restored on failure
//
// \return true if read successful
//
inline bool GetInt(char* cbuf, int* ival, int count, vil_stream * f, bool preserve = true)
{
    int orig_val = *ival;

    if (f->read(cbuf, count) < count) {
        return false;
    }
    else {
      cbuf[count] = '\0'; // Prevent sscanf from reading beyond end of buffer
      // IF cbuf CONTAINS BLANKS, TREAT AS ZERO
      if (vcl_sscanf(cbuf, "%d", ival) < 1) {
        *ival = 0;
      }
      if (preserve && *ival < 0) {
        *ival = orig_val;
        return false;
      }
    }
    return true;
}

//:
// this inline assumes b, v and f are valid.  It does not
// change the value of v unless the read is successful.
//
// \param cbuf input buffer
// \param ival pointer to unsigned integer value
// \param count number of digits to read for ival
// \param f pointer to stream from which characters will be read
// \param preserve flag indicating if original value should be restored on failure
//
// \return true if read successful
//
inline bool get_unsigned(char* cbuf, unsigned * ival, int count, vil_stream * f, bool preserve = true)
{
#if 0 // see below
    unsigned orig_val = *ival;
#endif

    if (f->read(cbuf, count) < count)
        return false;

    cbuf[count] = '\0'; // Prevent sscanf from reading beyond end of buffer
    // IF cbuf CONTAINS BLANKS, TREAT AS ZERO
    if (vcl_sscanf(cbuf, "%u", ival) < 1) {
      *ival = 0;
    }
#if 0 // *ival is unsigned, so it can never be < 0
    if (preserve && *ival < 0) {
      *ival = orig_val;
      return false;
    }
#endif
    return true;
}

// this inline assumes b and f are valid.
//
inline bool PutInt(char* b, int v, int w, vil_stream* f, bool p = true)
{
    if (p && v < 0) return false;
    vcl_sprintf(b, "%0*d", w, v);
    int pos = f->tell();
    if (f->write(b,w) < w) { f->seek(pos); return false; }
    return true;
}

// this inline assumes b and f are valid.
//
inline bool put_unsigned(char* b, unsigned int v, int w, vil_stream* f, bool p = true)
{
    vcl_sprintf(b, "%0*u", w, v);
    int pos = f->tell();
    if (f->write(b, w) < w) {
      f->seek(pos);
      return false;
    }
    return true;
}

inline bool GetUchar(char* b, vxl_byte* v, int w, vil_stream* f)
{
  if (f->read(b,w) < w)
    return false;
  else
    for (int i =0; i < w; i++)
      v[i]= (vxl_byte)b[i];
  return true;
}

// this inline assumes b and f are valid.
//
inline bool PutUchar(char* b, vxl_byte* v, int w, vil_stream* f)
{
    if (w < 0) return false;
    for (int i =0; i < w; i++)
      b[i]= v[i];
    int pos = f->tell();
    if (f->write(b,w) < w) {
        f->seek(pos);
        return false;
    }
    return true;
}

/////////////////////////////////////////////////
//: Create copy of string.
//
// \param str string to be copied
// \return char * pointer to new copy of string
//
/////////////////////////////////////////////////
inline char* new_strdup(const char* str)
{
    char *ret;

    if (str) {
        ret = new char[vcl_strlen(str)+1];
        vcl_strcpy(ret, str);
    }
    else {
      ret = 0;
    }
    return ret;
}

//: case insensitive string comparison
int nitf_strcasecmp( const char* s1, const char* s2 );

//: Copy n characters from y to x, but delete x first and recreate with exactly right length.
inline void nitf_strncpy (char *& x, const char * y, int n) 
{
    delete[] x ;
    x = new char[n+1] ;
    x[n] = 0 ;

    // NOTE: OLD MACRO STRNCPY USED 'y?y:""' AS SECOND PARAMETER
    // TO vcl_strncpy.  IF y WAS A CONSTANT, THIS CAUSED WARNINGS
    // ON INTEL COMPILER.  USE EXPLICIT TEST FOR y EQUAL NULL TO
    // AVOID THESE WARNINGS.  5jan2003  MAL.
    if (y != 0) {
      vcl_strncpy (x, y, n) ;
    }
    else {
      vcl_strncpy (x, "", n) ;
    }
}

#endif // vil_nitf_util_h_
