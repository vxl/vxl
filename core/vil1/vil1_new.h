// This is vxl/vil/vil_new.h
#ifndef vil_new_h_
#define vil_new_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <vil/vil_fwd.h>
#include <vil/vil_image.h>

//: Make a new image.
vil_image vil_new(int width, int height, vil_image const& prototype);

//: Make a new image.
vil_image vil_new(vil_stream* os,
                  int planes,
                  int width, int height,
                  int components,
                  int bits_per_component,
                  vil_component_format format,
                  char const* file_format = 0);

//: Make a new image.
vil_image vil_new(char const* filename,
                  int width, int height,
                  vil_image const &prototype,
                  char const* format = 0);

vil_image vil_new(vil_stream* os,
                  int width, int height,
                  vil_image const& prototype,
                  char const* file_format = 0);

#endif // vil_new_h_
