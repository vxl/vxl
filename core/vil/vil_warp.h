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
#include <vcl_cassert.h>

//: Warp an image under a 2D map.
// The size of the output map and the mapper defines the region of
// the input image to be scanned.
// \param mapper, is the inverse of the mapping from the input image's
// co-ordinate frame to the output image's frame.
// i.e. out() = in(mapper(x,y)). It should be a functor with a signature
// \code
//    void mapper(double x_in, double y_in, double* x_out, double* y_out);
// \endcode
// \param interp, is an interpolator, with a signature similar to
// \code
//   S vil_bilin_interp_safe(const vil_image_view<T>&, double, double, unsigned)
// \endcode
//
// Note that if you want to store a warp with an image to create a registered image,
// the vimt library (in contrib/mul/vimt) provides efficient registered images
// with transforms up to projective.
template <class sType, class dType, class MapFunctor, class InterpFunctor>
void vil_warp(const vil_image_view<sType>& in,
              vil_image_view<dType>& out,
              MapFunctor mapper,
              InterpFunctor interp)
{
  unsigned const out_w = out.ni();
  unsigned const out_h = out.nj();

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
        out(ox, oy, p) = dType(interp(in, ix, iy, p));
      }
    }
  }
}

#endif // vil_warp_h_
