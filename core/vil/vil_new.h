// This is mul/vil2/vil2_new.h
#ifndef vil2_new_h_
#define vil2_new_h_
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
// with more than 4 arguments.
//
// \author awf@robots.ox.ac.uk
// \date 26 Feb 00
//
//\verbatim
//  Modifications
//     000216 AWF Initial version.
//     25 Sep 2002 Ian Scott - convert to vil2.
//\endverbatim

#include <vil/vil_fwd.h> // for vil_stream
#include <vil2/vil2_image_resource.h>

//: Make a new image of given format.
vil2_image_resource_sptr vil2_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                         vil2_pixel_format format);

//: Make a new image, similar format to the prototype.
vil2_image_resource_sptr vil2_new_image_resource(unsigned ni, unsigned nj,
                                         vil2_image_resource_sptr const& prototype);

//: Make a new image.
vil2_image_resource_sptr vil2_new_image_resource(vil_stream* os,
                                         unsigned ni, unsigned nj,
                                         unsigned nplanes,
                                         vil2_pixel_format format,
                                         char const* file_format = 0);

//: Make a new image on disk
vil2_image_resource_sptr vil2_new_image_resource(char const* filename,
                                         unsigned ni, unsigned nj,
                                         unsigned nplanes,
                                         vil2_image_resource_sptr const &prototype,
                                         char const* format = 0);

//: Make a new image.
vil2_image_resource_sptr vil2_new_image_resource(vil_stream* os,
                                         unsigned ni, unsigned nj,
                                         unsigned nplanes,
                                         vil2_image_resource_sptr const& prototype,
                                         char const* file_format = 0);

#endif // vil2_new_h_
