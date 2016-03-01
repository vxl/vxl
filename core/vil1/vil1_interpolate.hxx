// This is core/vil1/vil1_interpolate.hxx

//-*- c++ -*-------------------------------------------------------------------
#ifndef vil1_interpolate_hxx_
#define vil1_interpolate_hxx_
// Author: awf@robots.ox.ac.uk
// Created: 02 Apr 00

#include "vil1_interpolate.h"

// Normal (x,y) access doesn't have range checking, so why should this? [capes Nov 1999]
//
// Indeed, but why support interpolation on the boundary, then? The current boundary
// interpolation is broken anyway because it does nearest neighbour interpolation if
// only one coordinate is on the boundary and the other is not. [fsm Sep 2000]

template <class T, class U>
bool vil1_interpolate_nearest(vil1_memory_image_of<T> const& img, double src_x, double src_y, U* out)
{
  int src_x_int = (int)(src_x + 0.5);
  int src_y_int = (int)(src_y + 0.5);
  *out = U(img(src_x_int,src_y_int));
  return true;
}

template <class T, class U>
bool vil1_interpolate_bilinear(vil1_memory_image_of<T> const& img,
                               double src_x, double src_y,
                               U* out)
{
  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;
#if 1
  // bounds check
  int width = (int)img.width();
  int height = (int)img.height();
  if (src_x_int < 0 || src_y_int < 0 || src_x_int+1 >= width || src_y_int+1 >= height)
    return false;
#endif
  T pix00 = img(src_x_int  , src_y_int  );
  T pix10 = img(src_x_int+1, src_y_int  );
  T pix01 = img(src_x_int  , src_y_int+1);
  T pix11 = img(src_x_int+1, src_y_int+1);

  double u = src_x - src_x_int;
  double v = src_y - src_y_int;

  // use bilinear interpolation on transferred point
  double weight00 = (u-1)*(v-1);
  double weight10 = u*(1-v);
  double weight01 = v*(1-u);
  double weight11 = u*v;

  *out = U(pix00 * weight00 + pix10 * weight10 + pix01 * weight01 + pix11 * weight11);
  return true;
}

template <class T, class U>
bool vil1_interpolate_bilinear_grad(vil1_memory_image_of<T> const& img,
                                    double src_x, double src_y,
                                    U* out_i,
                                    U* out_dx,
                                    U* out_dy)
{
  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;
#if 1
  // bounds check
  int width = (int)img.width();
  int height = (int)img.height();
  if (src_x_int < 0 || src_y_int < 0 || src_x_int+1 >= width || src_y_int+1 >= height)
    return false;
#endif
  T pix00 = img(src_x_int  , src_y_int  );
  T pix10 = img(src_x_int+1, src_y_int  );
  T pix01 = img(src_x_int  , src_y_int+1);
  T pix11 = img(src_x_int+1, src_y_int+1);

  // Get 1D weighting factors u0 + u1 = v0 + v1 = 1 and use separability.
  double u1 = src_x - src_x_int, u0 = 1-u1;
  double v1 = src_y - src_y_int, v0 = 1-v1;

  // the derivatives are obtained by differentiating the interpolating expression.
  // capes@robots: vil1_rgb does not have the left operator- defined.
  *out_i  = U(  (pix00 * v0 + pix01 * v1)*u0 + (pix10 * v0 + pix11 * v1)*u1 );
  *out_dx = U(  (pix10 - pix00) * v0 + (pix11 - pix01) * v1  );
  *out_dy = U(  (pix01 - pix00) * u0 + (pix11 - pix10) * u1  );

  return true;
}

template <class T, class U>
bool vil1_interpolate_bicubic(vil1_memory_image_of<T> const& img,
                              double src_x, double src_y,
                              U* out)
{
  int src_x_int = (int)src_x;
  int src_y_int = (int)src_y;

#if 1
  // bounds check
  int width = (int)img.width();
  int height = (int)img.height();
  if (src_x_int < 1 || src_y_int < 1 || src_x_int+2 >= width || src_y_int+2 >= height)
    return false;
#endif

  double u = src_x - src_x_int;
  double v = src_y - src_y_int;

  // like bilinear interpolation, use separability.
  // the {s}s are for the x-direction and the {t}s for the y-direction.
  double s0 = ((2-u)*u-1)*u;    // -1
  double s1 = (3*u-5)*u*u+2;    //  0
  double s2 = ((4-3*u)*u+1)*u;  // +1
  double s3 = (u-1)*u*u;        // +2

  double t0 = ((2-v)*v-1)*v;
  double t1 = (3*v-5)*v*v+2;
  double t2 = ((4-3*v)*v+1)*v;
  double t3 = (v-1)*v*v;

#define I(dx,dy) (img(src_x_int + (dx), src_y_int + (dy)))
  *out =U(0.25*
          ( (s0*I(-1,-1) + s1*I(+0,-1) + s2*I(+1,-1) + s3*I(+2,-1)) * t0 +
            (s0*I(-1,+0) + s1*I(+0,+0) + s2*I(+1,+0) + s3*I(+2,+0)) * t1 +
            (s0*I(-1,+1) + s1*I(+0,+1) + s2*I(+1,+1) + s3*I(+2,+1)) * t2 +
            (s0*I(-1,+2) + s1*I(+0,+2) + s2*I(+1,+2) + s3*I(+2,+2)) * t3 ));
#undef I
  return true;
}

#define VIL1_INTERPOLATE_INSTANTIATE(T, U) \
template bool vil1_interpolate_nearest(vil1_memory_image_of<T > const&, double, double, U*); \
template bool vil1_interpolate_bilinear(vil1_memory_image_of<T > const&, double, double, U*); \
template bool vil1_interpolate_bilinear_grad(vil1_memory_image_of<T > const&, double, double, U*, U*, U*); \
template bool vil1_interpolate_bicubic(vil1_memory_image_of<T > const&, double, double, U*)

#endif // vil1_interpolate_hxx_
