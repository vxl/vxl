// This is core/vil/file_formats/vil_nitf_util.h

//================ GE Aerospace NITF support libraries =================
//
// Description: This file defines utility methods for NITF.
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

//  Function to delete characters c from s. Options are:
//
//  \param s input string
//  \param c character to remove
//  \param optflag flag indicating how to remove characters
//    0: ALL:            delete all characters c.
//    1: TRAILING:       delete trailing characters c.
//    2: LEADING:          delete leading characters c.
//    3: HEAD/TAIL:        delete all leading && trailing characters c.
//
char * squeeze (char*, int, int);
void clear_string_vector (vcl_vector<vcl_string *> str_vector);
void display_as_hex (
    const unsigned char * buffer,
    unsigned int display_count,
    unsigned int bytes_per_value,
    vcl_string label = "");
void display_as_hex (const unsigned char value);

// Two functions below are from TargetJr file
//    .../IUPackages/GeneralUtility/Basics/geopt_util.C

enum geopt_coord {LAT, LON};

int geostr_to_latlon (const char* str, double* lat, double* lon);
int geostr_to_double (const char* str, double* val, geopt_coord lat_lon_flag);

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
inline bool GetInt (char* cbuf, int* ival, int count, vil_stream * f, bool preserve = true)
{
    int orig_val = *ival;
    bool rval = true;

    if (f->read (cbuf, count) < count) {
        rval = false;
    }
    else {
      char fmt[6];
      cbuf[count] = '\0'; // Prevent Purify UMR warning (MPP 5/7/2002)
      vcl_sprintf(fmt, "%%%dd", count);

      // IF cbuf CONTAINS BLANKS, TREAT AS ZERO
      if (vcl_sscanf(cbuf, fmt, ival) < 1) {
        *ival = 0;
      }
      if (preserve && *ival < 0) {
        rval = false;
        *ival = orig_val;
      }
    }
    return rval;
}

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

    char fmt[6];
    if (count > 99) return false; // avoid problems with fmt
    vcl_sprintf(fmt, "%%%uu", count);

    // IF cbuf CONTAINS BLANKS, TREAT AS ZERO
    if (vcl_sscanf(cbuf, fmt, ival) < 1) {
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
inline bool PutInt (char* b, int v, int w, vil_stream* f, bool p = true)
{
    if (p && v < 0) return false;
    vcl_sprintf(b, "%0*d", w, v);
    int pos = f->tell();
    if (f->write (b,w) < w) { f->seek(pos); return false; }
    return true;
}

// this inline assumes b and f are valid.
//
inline bool put_unsigned (char* b, unsigned int v, int w, vil_stream* f, bool p = true)
{
    bool rval = true;
    vcl_sprintf(b, "%0*u", w, v);
    int pos = f->tell();
    if (f->write (b, w) < w) {
      rval = false;
      f->seek(pos);
    }

    return rval;
}

inline bool GetUchar (char* b, vxl_byte* v, int w, vil_stream* f)
{
  bool rval = true;
  if (f->read (b,w) < w)
    rval = false;
  else
    for (int i =0; i < w; i++)
      v[i]= (vxl_byte)b[i];
  return rval;
}

// this inline assumes b and f are valid.
//
inline bool PutUchar (char* b, vxl_byte* v, int w, vil_stream* f)
{
    bool rval = true;
    if (w < 0) return false;
    for (int i =0; i < w; i++)
      b[i]= v[i];
    int pos = f->tell();
    if (f->write (b,w) < w) {
        rval = false;
        f->seek (pos);
    }
    return rval;
}

/////////////////////////////////////////////////
/// Create copy of string.
///
/// \param str string to be copied
/// \return char * pointer to new copy of string
///
/////////////////////////////////////////////////
inline char* new_strdup (const char* str)
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

// case insensitive string comparison
int nitf_strcasecmp( const char* s1, const char* s2 );

#endif // vil_nitf_util_h_
