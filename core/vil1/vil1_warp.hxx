// This is core/vil1/vil1_warp.hxx
#ifndef vil1_warp_hxx_
#define vil1_warp_hxx_
//:
// \file
// \brief See vil1_warp.h for a description of this file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   23 Jan 99
//
// \verbatim
//  Modifications
//   990123 AWF Initial version.
// \endverbatim
//
//-----------------------------------------------------------------------------

#include "vil1_warp.h"

#include <vil1/vil1_interpolate.h>
#include <vil1/vil1_memory_image_of.h>

static inline bool
vil1_warp_inrange_window(vil1_image const& in, int x, int y, int window_size)
{
  return x >= window_size && x < in.width() - window_size &&
    y >= window_size && y < in.height() - window_size;
}

// See vil1_warp.h
template <class PixelType, class Mapper>
void vil1_warp_output_driven(vil1_memory_image_of<PixelType> const& in,
                             vil1_memory_image_of<PixelType>& out,
                             Mapper const& map,
                             vil1_warp_interpolation_type interpolation)
{
  //int w = in.width();
  //int h = in.height();

  int out_w = out.width();
  int out_h = out.height();

  for (int oy = 0; oy < out_h; ++oy)
    for (int ox = 0; ox < out_w; ++ox) {
      // *** Find (ix, iy) from (ox,oy)
      double ixd, iyd;
      map.inverse_map(double(ox), double(oy), &ixd, &iyd);

      switch (interpolation) {
        case vil1_warp_interpolation_nearest_neighbour: {
          // nearest neighbour
          int ix = int(ixd + 0.5);
          int iy = int(iyd + 0.5);
          if (vil1_warp_inrange_window(in, ix, iy, 0))
            out(ox, oy) = in(ix,iy);
          break;
        }
        case vil1_warp_interpolation_bilinear: {
          // bilinear
          vil1_interpolate_bilinear(in, ixd, iyd, &out(ox,oy));
          break;
        }
        case vil1_warp_interpolation_bicubic: {
          vil1_interpolate_bicubic(in, ixd, iyd, &out(ox,oy));
          break;
        }
        default:
          break;
      }
    }
}

#define VIL1_WARP_INSTANTIATE(PixelType, Mapper) \
template void vil1_warp_output_driven(vil1_memory_image_of<PixelType > const&, \
                                      vil1_memory_image_of<PixelType >&,\
                                      Mapper const&,\
                                      vil1_warp_interpolation_type)

#endif // vil1_warp_hxx_
