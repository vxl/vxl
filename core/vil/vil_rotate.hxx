// This is core/vil/vil_rotate.hxx
#ifndef vil_rotate_hxx_
#define vil_rotate_hxx_
//:
// \file
// \brief rotate an image, using the resampling functions
// \author dac
//
// A front end to the resampling functions that allows
// an image to be rotated by any angle theta

#include <cmath>
#include "vil_rotate.h"
#include <vil/vil_resample_bilin.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Calculate theta in range 0 to x
static inline double calc_theta_mod(double theta, double x)
{
  if (x<0) x=-x;
  double theta_x = std::fmod(theta,x);
  if (theta_x<0)
    theta_x += x;
  return theta_x;
}

//: Rotate image by angle theta ( here theta is in degrees)
template <class sType, class dType>
void vil_rotate_image(const vil_image_view<sType>& src_image,
                      vil_image_view<dType>& dest_image,
                      double theta_deg)
{
  // nb if theta = 0, 90, 180 or 270 should employ a simpler + faster method
  // of rotating the image!  But at least, in those cases, d[xy][12] are 0 or 1.

  double theta_90= calc_theta_mod( theta_deg, 90.0 );
  double theta_360= calc_theta_mod( theta_deg, 360.0 );
#ifdef DEBUG
  std::cout<<"theta_90 = "<<theta_90<<std::endl
          <<"theta_360= "<<theta_360<<std::endl;
#endif

  // calculate dimensions of rotated image
  // a---b
  // |   |
  // c---d
  int src_ni = src_image.ni();
  int src_nj = src_image.nj();
  double c= std::cos(theta_90*3.14159265358979323846/180);
  double s= std::sin(theta_90*3.14159265358979323846/180);

  // calc corners of grid to sample (in original image frame)
  double ai= -src_nj*s*c;
  double aj=  src_nj*s*s;
  double bi=  src_ni*c*c;
  double bj= -src_ni*s*c;
  double ci=  src_ni*s*s;
  double cj=  src_nj-bj;
  double di=  src_ni-ai;
  double dj=  src_nj*c*c;

  // size of destination image
  int l1= int( src_nj*s+ src_ni*c );
  int l2= int( src_nj*c+ src_ni*s );

  // set up directions for sampling src image
  // nb varies every 90 degrees ( ie different corner at top of image!)
  double dx1, dy1, dx2, dy2, x0, y0;
  int n1,n2;

  assert(theta_360>= 0.0 && theta_360 < 360.0);

  if ( theta_360< 90.0 )
  {
    dx1= (bi-ai)/l1;
    dy1= (bj-aj)/l1;
    dx2= (ci-ai)/l2;
    dy2= (cj-aj)/l2;
    x0 = ai;
    y0 = aj;
    n1 = l1;
    n2 = l2;
  }
  else if (theta_360< 180.0 )
  {
    dx1= (ai-ci)/l2;
    dy1= (aj-cj)/l2;
    dx2= (di-ci)/l1;
    dy2= (dj-cj)/l1;
    x0 = ci;
    y0 = cj;
    n1 = l2;
    n2 = l1;
  }
  else if (theta_360< 270.0 )
  {
    dx1= (ci-di)/l1;
    dy1= (cj-dj)/l1;
    dx2= (bi-di)/l2;
    dy2= (bj-dj)/l2;
    x0 = di;
    y0 = dj;
    n1 = l1;
    n2 = l2;
  }
  else // if (theta_360< 360.0 )
  {
    dx1= (di-bi)/l2;
    dy1= (dj-bj)/l2;
    dx2= (ai-bi)/l1;
    dy2= (aj-bj)/l1;
    x0 = bi;
    y0 = bj;
    n1 = l2;
    n2 = l1;
  }

  vil_resample_bilin(src_image, dest_image, x0, y0, dx1, dy1, dx2, dy2, n1, n2 );
}

#define VIL_ROTATE_INSTANTIATE( sType, dType ) \
template void vil_rotate_image(const vil_image_view<sType >& src_image, \
                               vil_image_view<dType >& dest_image, \
                               double theta)

#endif // vil_rotate_hxx_
