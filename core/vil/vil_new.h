// This is core/vil/vil_new.h
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
// with more than 4 arguments.
//
// \author awf@robots.ox.ac.uk
// \date 26 Feb 00
//
// \verbatim
//  Modifications
//   000216 AWF Initial version.
//   25 Sep 2002 Ian Scott - convert to vil.
// \endverbatim

#include <vil/vil_fwd.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vcl_deprecated.h>

//: Make a new image of given format.
// If the format is not scalar, the number of planes must be 1. When you create
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                               vil_pixel_format format);

//: Make a new image resource that is a wrapper on an existing view's data.
// \note The output will be a shallow copy of the input, so changing the pixel values
// of one may change the pixel value of the other. Thanks to the magic of smart pointers,
// the output will remain valid even if you destroy the input. When you wrap
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource_of_view(vil_image_view_base const& view);


//: Make a new image, similar format to the prototype.
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype);

//: Make a new image.
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format = 0);

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_image_resource_sptr const &prototype,
                                               char const* format = 0);

//: Make a new vil_image_resource, writing to stream "os", size ni x nj, copying pixel format etc from "prototype".
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_image_resource_sptr const& prototype,
                                               char const* file_format = 0);

//: Create a new image view whose plane step is 1.
//  Pixel data type is the type of the last (dummy) argument.
//  i_step will be nplanes, j_step will be nplanes x ni.
// \deprecated in favour of vil_image_view constructor
template <class T>
vil_image_view<T> vil_new_image_view_j_i_plane(unsigned ni, unsigned nj, unsigned nplanes, T /*dummy*/)
{
  VXL_DEPRECATED("vil_new_image_view_j_i_plane");
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T),
                                                     vil_pixel_format_component_format(fmt));
  return vil_image_view<T>(chunk, reinterpret_cast<T*>(chunk->data()), ni, nj, nplanes, nplanes, nplanes*ni, 1);
}

//: Create a new image view whose j_step is 1.
//  Pixel data type is the type of the last (dummy) argument.
//  i_step will be nj, planestep will be ni x nj.
template <class T>
vil_image_view<T> vil_new_image_view_plane_i_j(unsigned ni, unsigned nj, unsigned nplanes, T /*dummy*/)
{
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T),
                                                     vil_pixel_format_component_format(fmt));
  return vil_image_view<T>(chunk, reinterpret_cast<T*>(chunk->data()), ni, nj, nplanes, nj, 1, nj*ni);
}

//: Create a new image view whose plane step is 1 and whose j_step is nplanes.
//  Pixel data type is the type of the last (dummy) argument.
//  i_step will be nplanes x nj.
template <class T>
vil_image_view<T> vil_new_image_view_i_j_plane(unsigned ni, unsigned nj, unsigned nplanes, T /*dummy*/)
{
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T),
                                                     vil_pixel_format_component_format(fmt));
  return vil_image_view<T>(chunk, reinterpret_cast<T*>(chunk->data()), ni, nj, nplanes, nplanes*nj, nplanes, 1);
}

#endif // vil_new_h_
