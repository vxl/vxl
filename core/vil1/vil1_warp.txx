// This is core/vil/vil_warp.txx
#ifndef vil_warp_txx_
#define vil_warp_txx_
//:
// \file
// \brief ImageWarp
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   23 Jan 99
// \verbatim
//  Modifications:
//   990123 AWF Initial version.
// \endverbatim
//
//-----------------------------------------------------------------------------

#include "vil_warp.h"

#include <vil/vil_interpolate.h>
#include <vil/vil_memory_image_of.h>

static inline bool
vil_warp_inrange_window(vil_image const& in, int x, int y, int window_size)
{
  return x >= window_size && x < in.width() - window_size &&
    y >= window_size && y < in.height() - window_size;
}

// See vil_warp.h
template <class PixelType, class Mapper>
void vil_warp_output_driven(vil_memory_image_of<PixelType> const& in,
                            vil_memory_image_of<PixelType>& out,
                            Mapper const& map,
                            vil_warp_interpolation_type interpolation)
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
        case vil_warp_interpolation_nearest_neighbour: {
          // nearest neigbour
          int ix = int(ixd + 0.5);
          int iy = int(iyd + 0.5);
          if (vil_warp_inrange_window(in, ix, iy, 0))
            out(ox, oy) = in(ix,iy);
          break;
        }
        case vil_warp_interpolation_bilinear: {
          // bilinear
          vil_interpolate_bilinear(in, ixd, iyd, &out(ox,oy));
          break;
        }
        case vil_warp_interpolation_bicubic: {
          vil_interpolate_bicubic(in, ixd, iyd, &out(ox,oy));
          break;
        }
      }
    }
}

#define VIL_WARP_INSTANTIATE(PixelType, Mapper) \
template void vil_warp_output_driven(vil_memory_image_of<PixelType > const&, \
                                     vil_memory_image_of<PixelType >&,\
                                     Mapper const&,\
                                     vil_warp_interpolation_type)

#endif // vil_warp_txx_
