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
#include <vil2/vil2_image_view.h>

//: Make a new image of given format.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                                 vil2_pixel_format format);

//: Make a new image, similar format to the prototype.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_new_image_resource(unsigned ni, unsigned nj,
                                                 vil2_image_resource_sptr const& prototype);

//: Make a new image.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_new_image_resource(vil_stream* os,
                                                 unsigned ni, unsigned nj,
                                                 unsigned nplanes,
                                                 vil2_pixel_format format,
                                                 char const* file_format = 0);

//: Make a new image on disk
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_new_image_resource(char const* filename,
                                                 unsigned ni, unsigned nj,
                                                 unsigned nplanes,
                                                 vil2_image_resource_sptr const &prototype,
                                                 char const* format = 0);

//: Make a new image.
// \relates vil2_image_resource
vil2_image_resource_sptr vil2_new_image_resource(vil_stream* os,
                                                 unsigned ni, unsigned nj,
                                                 unsigned nplanes,
                                                 vil2_image_resource_sptr const& prototype,
                                                 char const* file_format = 0);

//: Create an image view whose plane step is 1.
// j_step will by nplanes.
template <class T>
vil2_image_view<T> vil2_new_image_view_i_j_plane(unsigned ni, unsigned nj, unsigned nplanes, T dummy)
{
  vil2_memory_chunk_sptr chunk = new vil2_memory_chunk(ni*nj*nplanes*sizeof(T),
    vil2_pixel_format_component_format(vil2_pixel_format_of(T())));
  return vil2_image_view<T>(chunk, (T *)chunk->data(), ni, nj, nplanes, nplanes*nj, nplanes, 1);
}

//: Create an image view whose j step is 1.
// i_step will be nj.
template <class T>
vil2_image_view<T> vil2_new_image_view_plane_i_j(unsigned ni, unsigned nj, unsigned nplanes, T dummy)
{
  vil2_memory_chunk_sptr chunk = new vil2_memory_chunk(ni*nj*nplanes*sizeof(T),
    vil2_pixel_format_component_format(vil2_pixel_format_of(T())));
  return vil2_image_view<T>(chunk, (T *)chunk->data(), ni, nj, nj, 1, nj*ni, 1);
}

#endif // vil2_new_h_
