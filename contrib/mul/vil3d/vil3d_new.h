// This is mul/vil3d/vil3d_new.h
#ifndef vil3d_new_h_
#define vil3d_new_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Make a new image.

#include <vil/vil_fwd.h> // for vil_stream
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>

//: Make a new image of given format.
// \relates vil3d_image_resource
vil3d_image_resource_sptr vil3d_new_image_resource(unsigned ni, unsigned nj, unsigned nk,
                                                  unsigned nplanes, vil_pixel_format format);


//: Make a new image on disk.
// \relates vil3d_image_resource
vil3d_image_resource_sptr vil3d_new_image_resource(const char* filename, unsigned ni, unsigned nj,
                                                   unsigned nk, unsigned nplanes,
                                                   vil_pixel_format format,
                                                   char const* file_format = 0);


//: Create an image view whose i step is 1.
template <class T>
vil3d_image_view<T> vil3d_new_image_view_plane_k_j_i(unsigned ni, unsigned nj,
  unsigned nk, unsigned nplanes, T /*dummy*/)
{
  vil_pixel_format fmt = vil_pixel_format_of(T());
  vil_memory_chunk_sptr chunk = new vil_memory_chunk(ni*nj*nk*nplanes*sizeof(T),
    vil_pixel_format_component_format(fmt));
  return vil3d_image_view<T>(chunk, reinterpret_cast<T *>(chunk->data()), ni, nj, nk,
    nplanes, 1, ni, ni*nj, ni*nj*nk);
}

#endif // vil3d_new_h_
