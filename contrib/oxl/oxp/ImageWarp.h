// This is oxl/oxp/ImageWarp.h
#ifndef ImageWarp_h_
#define ImageWarp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief namespace for static functions
//
//    ImageWarp is a namespace for static `image warping' functions
//    warp, warp_inverse, gapfill, and mean_nz_intensity.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Jan 99
//-----------------------------------------------------------------------------

#include <vil/vil_memory_image_of.h>
#include <oxp/Mapping_2d_2d.h>

template <class PixelType>
class ImageWarp
{
 public:
  //:
  // The type of the return value of "bilinear" and "bicubic", which
  // will be a floating point version of the pixel type.
  typedef typename vnl_numeric_traits<PixelType>::real_t real_t;

  static void mean_nz_intensity(const vil_memory_image_of<PixelType>& in,
                                int x, int y, int window_size,
                                real_t* out,
                                int * nnzp = 0);
  static void gapfill(vil_memory_image_of<PixelType>& out, int ngaps);
  static void warp(Mapping_2d_2d& map, const vil_memory_image_of<PixelType>& in, vil_memory_image_of<PixelType>& out);
  static void warp_inverse(Mapping_2d_2d& map, const vil_memory_image_of<PixelType>& in, vil_memory_image_of<PixelType>& out);
};

#endif // ImageWarp_h_
