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
//   16 Feb 2000 AWF - Initial version.
//   25 Sep 2002 Ian Scott - convert to vil.
//   30 Mar 2007 Peter Vanroose- Removed deprecated vil_new_image_view_j_i_plane
// \endverbatim

#include <vil/vil_fwd.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_image_view.h>
#include <vxl_config.h>

//: Make a new image of given format.
// If the format is not scalar, the number of planes must be 1. When you create
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                               vil_pixel_format format);

//: Make a new image of given format with interleaved planes.
// The format must be scalar.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource_interleaved(unsigned ni, unsigned nj,
                                                           unsigned nplanes,
                                                           vil_pixel_format format);

//: Make a new image resource that is a wrapper on an existing view's data.
// \note The output will be a shallow copy of the input, so changing the pixel values
// of one may change the pixel value of the other. Thanks to the magic of smart pointers,
// the output will remain valid even if you destroy the input. When you wrap
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource_of_view(vil_image_view_base const& view);


//: Make a new image, similar format to the prototype.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype);

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format = nullptr);

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format = nullptr);

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_image_resource_sptr const &prototype,
                                               char const* format = nullptr);

//: Make a new vil_image_resource, writing to stream "os", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_image_resource_sptr const& prototype,
                                               char const* file_format = nullptr);
//: Make a new blocked resource file
vil_blocked_image_resource_sptr
vil_new_blocked_image_resource(vil_stream* os, unsigned ni, unsigned nj,
                               unsigned nplanes, vil_pixel_format format,
                               unsigned size_block_i, unsigned size_block_j,
                               char const* file_format = nullptr);

//: Make a new blocked resource file
vil_blocked_image_resource_sptr
vil_new_blocked_image_resource(char const* filename, unsigned ni, unsigned nj,
                               unsigned nplanes, vil_pixel_format format,
                               unsigned size_block_i, unsigned size_block_j,
                               char const* file_format = nullptr);

//: create a blocked interface around any image resource
// For zero size blocks, appropriate default blocking is created
vil_blocked_image_resource_sptr
vil_new_blocked_image_facade(const vil_image_resource_sptr& src,
                             const unsigned size_block_i=0,
                             const unsigned size_block_j=0);
//: Make a new cached resource
vil_blocked_image_resource_sptr
vil_new_cached_image_resource(const vil_blocked_image_resource_sptr& bir,
                              const unsigned cache_size = 100);


//: Make a new pyramid image resource for writing.
//  Any number of pyramid layers can be inserted and with any scale.
//  Image resources that duplicate existing scales are not inserted.
vil_pyramid_image_resource_sptr
vil_new_pyramid_image_resource(char const* file_or_directory,
                               char const* file_format);

//: Construct a pyramid image resource from a base image.
//  All levels are stored in the same resource file. Each level has the same
//  scale ratio (0.5) to the preceding level. Level 0 is the original
//  base image. The resource is returned open for reading.
//  The temporary directory is for storing intermediate image
//  resources during the construction of the pyramid. Files are
//  be removed from the directory after completion.  If temp_dir is 0
//  then the intermediate resources are created in memory.
vil_pyramid_image_resource_sptr
vil_new_pyramid_image_from_base(char const* filename,
                                vil_image_resource_sptr const& base_image,
                                unsigned nlevels,
                                char const* file_format,
                                char const* temp_dir);

//: Construct a new pyramid image resource from a base image
//  The result is a directory containing separate images for each
//  pyramid level. Each level has the same scale ratio (0.5) to the
//  preceding level and is created using level_file_format.
//  Level 0 is the original base image. If copy_base is false, then Level 0
//  is already present in the directory and is used without modification.
//  Each pyramid file in the directory is
//   named filename + "level_index", e.g. R0, R1, ... Rn.
vil_pyramid_image_resource_sptr
vil_new_pyramid_image_list_from_base(char const* directory,
                                     vil_image_resource_sptr const& base_image,
                                     unsigned nlevels,
                                     bool copy_base,
                                     char const* level_file_format,
                                     char const* filename);

#if 0 // deprecated
//: Create a new image view whose plane step is 1.
//  Pixel data type is the type of the last (dummy) argument.
//  i_step will be nplanes, j_step will be nplanes x ni.
//  \relatesalso vil_image_view
// \deprecated in favour of vil_image_view constructor
template <class T>
vil_image_view<T> vil_new_image_view_j_i_plane(unsigned ni, unsigned nj, unsigned nplanes, T /*dummy*/)
{
  VXL_DEPRECATED_MACRO("vil_new_image_view_j_i_plane");
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T),
                                                     vil_pixel_format_component_format(fmt));
  return vil_image_view<T>(chunk, reinterpret_cast<T*>(chunk->data()), ni, nj, nplanes, nplanes, nplanes*ni, 1);
}
#endif // 0

//: Create a new image view whose j_step is 1.
//  Pixel data type is the type of the last (dummy) argument.
//  i_step will be nj, planestep will be ni x nj.
//  \relatesalso vil_image_view
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
//  \relatesalso vil_image_view
template <class T>
vil_image_view<T> vil_new_image_view_i_j_plane(unsigned ni, unsigned nj, unsigned nplanes, T /*dummy*/)
{
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nplanes*sizeof(T),
                                                     vil_pixel_format_component_format(fmt));
  return vil_image_view<T>(chunk, reinterpret_cast<T*>(chunk->data()), ni, nj, nplanes, nplanes*nj, nplanes, 1);
}

//: Create a shallow copy of an image and wrap it in a vil_image_view_base_sptr
// \note vil_image_view_base_sptr almost certainly doesn't behave as
// you would expect, and this function should really only be used by experts.
// \relatesalso vil_image_view
vil_image_view_base_sptr vil_new_image_view_base_sptr(const vil_image_view_base&);


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               wchar_t const* file_format = 0);
//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(wchar_t const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               wchar_t const* file_format = 0);

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(wchar_t const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_image_resource_sptr const &prototype,
                                               wchar_t const* format = 0);

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T


#endif // vil_new_h_
