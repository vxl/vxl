// This is core/vil/vil_rotate.txx
#ifndef vil_rotate_txx_
#define vil_rotate_txx_
//:
// \file
// \brief rotate an image, using the resampling functions
// \author dac
//
// A front end to the resampling functions that allows
// an image to be rotated by any angle theta

#include <vcl_cstdlib.h>
#include "vil_rotate.h"
#include <vil/vil_resample_bilin.h>


//: Calculate theta in range 0 to x
static double calc_theta_mod(double theta, double x)
{
  double theta_x= theta;
  if (theta>=x)
  {
    int n= (int) theta/x;
    theta_x= theta-x*n;
  }
  if (theta<0)
  {
    int n= -1+(int) theta/x;
    theta_x= theta-x*n;
  }

  return theta_x;
}

//: Rotate image by angle theta ( here theta is in degrees)
template <class sType, class dType>
void vil_rotate_image(const vil_image_view<sType>& src_image,
                         vil_image_view<dType>& dest_image,
                         double theta_deg)
{

  // nb if theta = 0, 90, 180 or 270 should employ a simpler + faster method
  // of rotating the image!

  double theta_90= calc_theta_mod( theta_deg, 90 );
  //vcl_cout<<"theta_90= "<<theta_90<<vcl_endl;

  double theta_360= calc_theta_mod( theta_deg, 360 );
  //vcl_cout<<"theta_360= "<<theta_360<<vcl_endl;


  // calculate dimensions of rotated image
  // a---b
  // |   |
  // c---d
  int src_ni = src_image.ni();
  int src_nj = src_image.nj();
  double cos= vcl_cos(theta_90*3.14159/180);
  double sin= vcl_sin(theta_90*3.14159/180); 

  // calc corners of grid to sample (in original image frame)
  double ai, aj, bi, bj, ci, cj, di, dj;
  int l1,l2;
  ai= -src_nj*sin*cos;
  aj= src_nj*sin*sin;
  bi= src_ni*cos*cos;
  bj= -src_ni*sin*cos;
  ci= src_ni*sin*sin;
  cj= src_nj+src_ni*sin*cos;
  di= src_ni+src_nj*cos*sin;
  dj= src_nj*cos*cos;

  // size of destination image
  l1= (int) ( src_nj*sin+ src_ni*cos );
  l2= (int) ( src_nj*cos+ src_ni*sin );
  
  // set up directions for sampling src image
  // nb varies every 90 degrees ( ie different corner at top of image!)
  double dx1, dy1, dx2, dy2, x0, y0;
  int n1,n2;
  if ( theta_360< 90 )
  {
    dx1= (bi-ai)/(l1-1); 
    dy1= (bj-aj)/(l1-1);
    dx2= (ci-ai)/(l2-1); 
    dy2= (cj-aj)/(l2-1);    
    x0 = ai;
    y0 = aj;
    n1 = l1;
    n2 = l2;
  }
  else
  if (theta_360< 180 )
  {
    dx1= (ai-ci)/(l2-1); 
    dy1= (aj-cj)/(l2-1);
    dx2= (di-ci)/(l1-1); 
    dy2= (dj-cj)/(l1-1);    
    x0 = ci;
    y0 = cj;
    n1 = l2;
    n2 = l1;
  }
  else
  if (theta_360< 270 )
  {
    dx1= (ci-di)/(l1-1); 
    dy1= (cj-dj)/(l1-1);
    dx2= (bi-di)/(l2-1); 
    dy2= (bj-dj)/(l2-1);    
    x0 = di;
    y0 = dj;
    n1 = l1;
    n2 = l2;
  }
  else
  if (theta_360< 360 )
  {
    dx1= (di-bi)/(l2-1); 
    dy1= (dj-bj)/(l2-1);
    dx2= (ai-bi)/(l1-1); 
    dy2= (aj-bj)/(l1-1);    
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

#endif // vil_rotate_txx_
