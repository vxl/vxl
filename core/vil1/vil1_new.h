// This is core/vil1/vil1_new.h
#ifndef vil1_new_h_
#define vil1_new_h_
//:
// \file
// \brief Make a new image.
//
// If it's very big, it might make a disk image, with a temporary name in which
// case "prototype" will be consulted about issues such as file format etc.  If
// you want more control over the exact disk format, use one of the routines
// with more than 3 arguments.
//
// \author awf@robots.ox.ac.uk
// \date 26 Feb 00
//
//\verbatim
//  Modifications
//     000216 AWF Initial version.
//\endverbatim

#include "vil1_fwd.h"
#include "vil1_image.h"

//: Make a new image.
vil1_image
vil1_new(int width, int height, const vil1_image & prototype);

//: Make a new image.
vil1_image
vil1_new(vil1_stream * os,
         int planes,
         int width,
         int height,
         int components,
         int bits_per_component,
         vil1_component_format format,
         const char * file_format = nullptr);

//: Make a new image.
vil1_image
vil1_new(const char * filename, int width, int height, const vil1_image & prototype, const char * format = nullptr);

vil1_image
vil1_new(vil1_stream * os, int width, int height, const vil1_image & prototype, const char * file_format = nullptr);

#endif // vil1_new_h_
