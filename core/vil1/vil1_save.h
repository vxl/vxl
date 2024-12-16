// This is core/vil1/vil1_save.h
#ifndef vil1_save_h_
#define vil1_save_h_
//:
// \file
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
//  Modifications
//     000216 AWF Initial version.
//     011002 Peter Vanroose - vil1_save now respects top-is-first; vil1_save_raw not
//\endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vil1_stream;
#include "vil1_image.h"

//: Send vil1_image to disk, deducing format from filename
bool
vil1_save(const vil1_image &, const char * filename);

//: Send vil1_image to disk, given filename
bool
vil1_save(vil1_image, const char * filename, const char * file_format);

//: Send vil1_image to disk, given filename; preserve byte order
bool
vil1_save_raw(const vil1_image &, const char * filename, const char * file_format);

//: Send vil1_image to outstream
bool
vil1_save_raw(const vil1_image &, vil1_stream * outstream, const char * file_format = "pnm");

//: Save raw unsigned chars, deducing format from filename
void
vil1_save_gray(const unsigned char * p, int w, int h, const std::string & fn);

//: Save raw floats as gray.
// No scaling is performed, so values would be 0..255.
// File format is deduced from filename.
void
vil1_save_gray(const float * p, int w, int h, const std::string & fn);

//: Save raw doubles as gray.
// No scaling is performed, so values would be 0..255.
// File format is deduced from filename.
void
vil1_save_gray(const double * p, int w, int h, const std::string & fn);


//: Save raw RGB, deducing format from filename
void
vil1_save_rgb(const unsigned char * p, int w, int h, const std::string & fn);

//: Save raw floats as RGB.  See vil1_save_gray.
void
vil1_save_rgb(const float * p, int w, int h, const std::string & fn);

//: Save raw doubles as RGB.  See vil1_save_gray.
void
vil1_save_rgb(const double * p, int w, int h, const std::string & fn);

#endif // vil1_save_h_
