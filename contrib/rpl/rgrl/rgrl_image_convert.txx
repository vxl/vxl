#include "rgrl_image_convert.h"

#include <vcl_cmath.h>

#include <vil3d/vil3d_trilin_interp.h>

template< class PixelType >
void
rgrl_image_convert_3d( const vil3d_image_view< PixelType >& src,
                       double x_space, double y_space, double z_space,
                       vil3d_image_view< PixelType >& dest)
{
  // Compute the image size in the physical space. 
  const unsigned ni = unsigned(vcl_ceil( src.ni() * x_space ));
  const unsigned nj = unsigned(vcl_ceil( src.nj() * y_space ));
  const unsigned nk = unsigned(vcl_ceil( src.nk() * z_space ));
  const unsigned np = src.nplanes();
  dest.set_size(ni, nj, nk, np);
  dest.fill(0);

  // Perform trilinear interpolation
  for (unsigned p = 0; p < np; ++p)
    for (unsigned k = 0; k < nk; ++k)
      for (unsigned j = 0; j < nj; ++j)
        for (unsigned i = 0; i < ni; ++i) {
          // get the nearest pixel
          double pi = double(i)/x_space;
          double pj = double(j)/y_space;
          double pk = double(k)/z_space;
          dest(i,j,k,p) = (PixelType)vil3d_trilin_interp_safe( pi, pj, pk, 
                                                    src.origin_ptr(), 
                                                    src.ni(), 
                                                    src.nj(), 
                                                    src.nk(), 
                                                    src.istep(), 
                                                    src.jstep(), 
                                                    src.kstep() );
        }
}

#define RGRL_IMAGE_CONVERT_INSTANTIATE(PixelType) \
template void rgrl_image_convert_3d( const vil3d_image_view< PixelType >& src, \
                       double x_space, double y_space, double z_space, \
                       vil3d_image_view< PixelType >& dest);
