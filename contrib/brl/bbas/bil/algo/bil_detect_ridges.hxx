#ifndef bil_detect_ridges_hxx_
#define bil_detect_ridges_hxx_

#include "bil_detect_ridges.h"
#include <vil/vil_convert.h>
#include <vil/algo/vil_correlate_2d.h>
#include <vil/vil_transpose.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>

template <class T>
void bil_detect_ridges( const vil_image_view<T>& Im,
                        float sigma,
                        float epsilon,
                        vil_image_view<int>& rho_int,
                        vil_image_view<float>& ex,
                        vil_image_view<float>& ey,
                        vil_image_view<float>& lambda)
{
  vil_image_view<float> I;
  vil_convert_cast(Im,I);
  unsigned ni = I.ni();
  unsigned nj = I.nj();

  // Compute the gaussian smoothed derivatives of this image
  // define gaussian derivative operators
  int width = vnl_math::rnd(3*sigma);
  double ssq = sigma*sigma;
  double d1_denominator = vnl_math::twopi*ssq*ssq;
  double d2_denominator = vnl_math::twopi*ssq*ssq*ssq;

  vil_image_view<float> dgau2D(2*width+1,2*width+1);
  vil_image_view<float> d2gau2D(2*width+1,2*width+1);
  vil_image_view<float> d2gauxy2D(2*width+1,2*width+1);

  for (int y = -width; y <= width; y++) {
    for (int x = -width; x <= width; x++) {
      int i = x + width;
      int j = y + width;
      double exponential = std::exp(-(x*x + y*y)/(2*ssq));

      dgau2D(i,j)    = -float(x*exponential/d1_denominator);
      d2gau2D(i,j)   = -float((ssq-x*x)*exponential/d2_denominator);
      d2gauxy2D(i,j) =  float(x*y*exponential/d2_denominator);
    }
  }

  vil_image_view<float> Ix ;
  vil_image_view<float> Iy ;
  vil_image_view<float> Ixx;
  vil_image_view<float> Iyy;
  vil_image_view<float> Ixy;

  vil_image_resource_sptr res = vil_new_image_resource(ni+2*width,nj+2*width, 1, VIL_PIXEL_FORMAT_FLOAT);
  vil_image_view<float> padded = res->get_view();
  padded.fill(0);
  res->put_view(padded,0,0);
  res->put_view(I,width,width);

  padded = res->get_view();

  // compute the derivatives

  float dummy=0;
  vil_correlate_2d(padded,Ix,dgau2D,dummy);
  vil_correlate_2d(padded,Iy,vil_transpose(dgau2D),dummy);
  vil_correlate_2d(padded,Ixx,d2gau2D,dummy);
  vil_correlate_2d(padded,Iyy,vil_transpose(d2gau2D),dummy);
  vil_correlate_2d(padded,Ixy,d2gauxy2D,dummy);

  bil_detect_ridges( Ix, Iy, Ixx, Iyy, Ixy,
                     sigma, epsilon,
                     rho_int, ex, ey, lambda);
}

#undef  BIL_DETECT_RIDGES_INSTANTIATE
#define BIL_DETECT_RIDGES_INSTANTIATE(T) \
template void bil_detect_ridges( const vil_image_view<T >& Im,\
                                 float sigma,\
                                 float epsilon,\
                                 vil_image_view<int >& rho_int,\
                                 vil_image_view<float >& ex,\
                                 vil_image_view<float >& ey,\
                                 vil_image_view<float >& lambda)

#endif // bil_detect_ridges_hxx_
