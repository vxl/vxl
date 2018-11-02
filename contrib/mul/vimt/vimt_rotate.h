// This is mul/vimt/vimt_rotate.h
#ifndef vimt_rotate_h_
#define vimt_rotate_h_
//:
// \file
// \brief Rotate an image, using the resampling functions
// \author Tim Cootes
//
// A front end to the resampling functions that allows
// an image to be rotated by any angle theta

#include <iostream>
#include <cmath>
#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Calculate theta in range 0 to x
inline double calc_theta_mod(double theta, double x)
{
  if (x<0) x=-x;
  double theta_x = std::fmod(theta,x);
  if (theta_x<0)
    theta_x += x;
  return theta_x;
}

//: Rotate image by angle theta
// On exit, dest is sized to completely include rotated source.
// Its world2im() transformation includes that of the src and
// the rotation (ie world frame is preserved).
// Note that this currently assumes square pixels.
// So dest_image.world2im()=rotation*src_image.world2im()
//  \relatesalso vil_image_view
template <class sType, class dType>
inline void vimt_rotate(const vimt_image_2d_of<sType>& src_image,
                        vimt_image_2d_of<dType>& dest_image,
                        double theta_deg)
{
  if (theta_deg==0.0)
  {
    dest_image=src_image;
    return;
  }

  // nb if theta = 0, 90, 180 or 270 should employ a simpler + faster method
  // of rotating the image!  But at least, in those cases, d[xy][12] are 0 or 1.

  double theta_90= calc_theta_mod( theta_deg, 90.0 );
  double theta_360= calc_theta_mod( theta_deg, 360.0 );

  // calculate dimensions of rotated image
  // a---b
  // |   |
  // c---d
  int src_ni = src_image.image().ni();
  int src_nj = src_image.image().nj();
  double c= std::cos(theta_90*vnl_math::pi_over_180);
  double s= std::sin(theta_90*vnl_math::pi_over_180);

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

  vil_resample_bilin(src_image.image(), dest_image.image(),
                     x0, y0, dx1, dy1, dx2, dy2, n1, n2 );

  // Set up rotation transformation (giving image to rotated frame)
  vimt_transform_2d rot;
  rot.set_similarity(vgl_point_2d<double>(dx1,dy1),
                     vgl_point_2d<double>(x0,y0));
  vimt_transform_2d im2w = src_image.world2im().inverse()*rot;
  dest_image.set_world2im(im2w.inverse());
}

#endif // vimt_rotate_h_
