// This is core/vil/vil_warp.h
#ifndef vil_warp_h_
#define vil_warp_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Warp an image.
// \author awf@robots.ox.ac.uk
// \date 04 Dec 00
// \verbatim
//  Modifications
//   031201 IMS Convert to vil2. Used templates to simplify interface and code.
// \endverbatim

#include <vil/vil_fwd.h>

//: Warp an image under a 2D map.
// The size of the output map and the mapper defines the reqgion of the input image to be scanned.
// \param mapper, is a mapping such that out() = in(mapper(x,y)).
// It should be a functor with a signature
// \verbatim
// void mapper(double x_in, double y_in, double* x_out, double* y_out);
// \endverbatim
// \param interp, is an interpolator, with a signature similar
// to double vil_bilin_interp_safe(const vil_image_view<T> &, double, double, unsigned)
template <class sType, class dType, class MapFunctor, class InterpFunctor>
void vil_warp(const vil_image_view<sType> &in,
              vil_image_view<dType>&out,
              MapFunctor mapper,
              InterpFunctor interp)
{
  unsigned out_w = out.ni();
  unsigned out_h = out.nj();

  assert(out.nplanes() == in.nplanes());

  for (unsigned p = 0; p < out.nplanes(); ++p)
  {
    for (unsigned oy = 0; oy < out_h; ++oy)
    {
      for (unsigned ox = 0; ox < out_w; ++ox)
      {
        // *** Find (ix, iy) from (ox,oy)
        double ix, iy;
        mapper(double(ox), double(oy), ix, iy);
        out(ox, oy) = interp(in, ix, iy, p);
      }
    }
  }
}

#endif // vil_warp_h_
