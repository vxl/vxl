//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_interpolate_txx_
#define vil_interpolate_txx_
// Author: awf@robots.ox.ac.uk
// Created: 02 Apr 00

#include <vil/vil_interpolate.h>

template <class T>
T vil_interpolate_nearest(vil_memory_image_of<T> const& img, double src_x, double src_y)
{
  int src_x_int = (int)(src_x + 0.5);
  int src_y_int = (int)(src_y + 0.5);
  return img(src_x_int,src_y_int);
}

template <class T, class U>
bool vil_interpolate_bilinear(vil_memory_image_of<T> const& img, double src_x, double src_y, U* out)
{
  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;
	
  // Return T() for out of range
  // Normal (x,y) access doesn't have range checking, so why should this? [capes Nov 1999]
  /*
  if (! ( (src_x_int > 0) &&
	  (src_x_int+1 < (int)GetSizeX()-1) && 
	  (src_y_int > 0) && 
	  (src_y_int+1 < (int)GetSizeY()-1) )) {
    real_temp_ = real_t();
    return false;
  }
  */

  int width = (int)img.width();
  int height = (int)img.height();

  // Boundary - just return value with no interpolation
  if (src_x_int == width-1 || src_y_int == height-1) {
    *out = U(img(src_x_int,src_y_int));
    return true;
  }

  double alpha = src_x - src_x_int;
  double beta = src_y - src_y_int;

  T pix00 = img(src_x_int  , src_y_int  );
  T pix10 = img(src_x_int+1, src_y_int  );
  T pix01 = img(src_x_int  , src_y_int+1);
  T pix11 = img(src_x_int+1, src_y_int+1);
  
  // use bilinear interpolation on transfered point
  double weight00 = (alpha-1)*(beta-1);
  double weight10 = alpha*(1-beta);
  double weight01 = beta*(1-alpha);
  double weight11 = alpha*beta;
      
  *out = U(pix00 * weight00 + pix10 * weight10 + pix01 * weight01 + pix11 * weight11);
  return true;
}

template <class T, class U>
bool vil_interpolate_bilinear_grad(vil_memory_image_of<T> const& img, double src_x, double src_y,
				   U* out_i,
				   U* out_dx,
				   U* out_dy
				   )
{
  typedef U real_t;

  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;
	
  // Return T() for out of range
  // Normal (x,y) access doesn't have range checking, so why should this? [capes Nov 1999]
//  if (! ( (src_x_int >= 0) &&
//	  (src_x_int+1 < (int)img.width()-1) && 
//	  (src_y_int >= 0) && 
//	  (src_y_int+1 < (int)img.width()-1) ))
//    return false;

  double alpha = src_x - src_x_int;
  double beta = src_y - src_y_int;

  T pix00 = img(src_x_int  , src_y_int  );
  T pix10 = img(src_x_int+1, src_y_int  );
  T pix01 = img(src_x_int  , src_y_int+1);
  T pix11 = img(src_x_int+1, src_y_int+1);
  
  // use bilinear interpolation on transfered point
  double weight00 = (alpha-1)*(beta-1);
  double weight10 = alpha*(1-beta);
  double weight01 = beta*(1-alpha);
  double weight11 = alpha*beta;
      
  *out_i = U(pix00 * weight00 + pix10 * weight10 + pix01 * weight01 + pix11 * weight11);
  *out_dx = U(pix00 * (beta-1) + pix10 * (1-beta) - pix01 * beta + pix11 * beta);
  *out_dy = U(pix00 * (alpha-1) - pix10 * alpha + pix01 * (1-alpha) + pix11 * alpha);

  return true;
}

template <class T, class U>
bool vil_interpolate_bicubic(vil_memory_image_of<T> const& img, double src_x, double src_y, U* out)
{
  typedef U real_t;

  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;

  double alpha = src_x - src_x_int;
  double beta = src_y - src_y_int;

  int width = (int)img.width();
  int height = (int)img.height();

  // Return false if out of range
  // Normal (x,y) access doesn't have range checking, so why should this? [capes Nov 1999]
//  if (! ( (src_x_int >= 0) &&
//	  (src_x_int < width) &&
//	  (src_y_int >= 0) &&
//	  (src_y_int < height) ))
//    return false;

  // Boundary - just return value with no interpolation
  if (src_x_int <= 0 || src_y_int <= 0 || src_x_int >= width-2 || src_y_int >= height-2) {
    *out = U(img(src_x_int,src_y_int));
    return true;
  }
  
  double s0 = ((2-alpha)*alpha-1)*alpha;
  double s1 = (3*alpha-5)*alpha*alpha+2;
  double s2 = ((4-3*alpha)*alpha+1)*alpha;
  double s3 = (alpha-1)*alpha*alpha;
  double t0 = ((2-beta)*beta-1)*beta;
  double t1 = (3*beta-5)*beta*beta+2;
  double t2 = ((4-3*beta)*beta+1)*beta;
  double t3 = (beta-1)*beta*beta;

#define I(dx,dy) (img(src_x_int + (dx), src_y_int + (dy)))
  *out =U(0.25*
    ( (s0*I(-1,-1) + s1*I(+0,-1) + s2*I(+1,-1) + s3*I(+2,-1))*t0 +
      (s0*I(-1,+0) + s1*I(+0,+0) + s2*I(+1,+0) + s3*I(+2,+0))*t1 +
      (s0*I(-1,+1) + s1*I(+0,+1) + s2*I(+1,+1) + s3*I(+2,+1))*t2 +
      (s0*I(-1,+2) + s1*I(+0,+2) + s2*I(+1,+2) + s3*I(+2,+2))*t3 ));
#undef I
  return true;
}

#define VIL_INTERPOLATE_INSTANTIATE(T, U) \
template T vil_interpolate_nearest(vil_memory_image_of<T> const& img, double src_x, double src_y); \
template bool vil_interpolate_bilinear(vil_memory_image_of<T> const& img, double src_x, double src_y, U*); \
template bool vil_interpolate_bilinear_grad(vil_memory_image_of<T> const&, double, double, U*, U*, U*); \
template bool vil_interpolate_bicubic(vil_memory_image_of<T> const&, double, double, U*);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_interpolate.

