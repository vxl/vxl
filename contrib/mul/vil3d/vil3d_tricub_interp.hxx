#ifndef vil3d_tricub_interp_hxx_
#define vil3d_tricub_interp_hxx_
//:
// \file
// \brief Tricubic interpolation functions for 3D images

#include <iostream>
#include <limits>
#include "vil3d_tricub_interp.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_round.h>


namespace
{
  void get_tricubic_coeff( double t,
                           double &c0, double &c1, double &c2, double &c3 )
  {
    c0 = ((2-t)*t-1)*t;    // -1
    c1 = (3*t-5)*t*t+2;    //  0
    c2 = ((4-3*t)*t+1)*t;  // +1
    c3 = (t-1)*t*t;        // +2
  }
}


template<class T>
// double vil3d_tricub_interp_unsafe(double x, double y, double z,
double vil3d_tricub_interp_raw(double x, double y, double z,
                               const T* data,
                               std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  int p1x=int(x);
  double normx = x-p1x;
  int p1y=int(y);
  double normy = y-p1y;
  int p1z=int(z);
  double normz = z-p1z;

  const T* pix1 = data + p1y*ystep + p1x*xstep + p1z*zstep;

  // use separability.
  // the s's are for the x-direction, the t's for the y-direction,
  // the u's are for the z-direction.
  double s0, s1, s2, s3;
  get_tricubic_coeff( normx, s0, s1, s2, s3 );
  double t0, t1, t2, t3;
  get_tricubic_coeff( normy, t0, t1, t2, t3 );
  double u0, u1, u2, u3;
  get_tricubic_coeff( normz, u0, u1, u2, u3 );

#define vil3d_I(dx,dy,dz) (pix1[(dx)*xstep+(dy)*ystep+(dz)*zstep])

  double xi00 = s0*vil3d_I(-1,-1,-1) + s1*vil3d_I(+0,-1,-1) + s2*vil3d_I(+1,-1,-1) + s3*vil3d_I(+2,-1,-1);
  double xi10 = s0*vil3d_I(-1,+0,-1) + s1*vil3d_I(+0,+0,-1) + s2*vil3d_I(+1,+0,-1) + s3*vil3d_I(+2,+0,-1);
  double xi20 = s0*vil3d_I(-1,+1,-1) + s1*vil3d_I(+0,+1,-1) + s2*vil3d_I(+1,+1,-1) + s3*vil3d_I(+2,+1,-1);
  double xi30 = s0*vil3d_I(-1,+2,-1) + s1*vil3d_I(+0,+2,-1) + s2*vil3d_I(+1,+2,-1) + s3*vil3d_I(+2,+2,-1);

  double xi01 = s0*vil3d_I(-1,-1,+0) + s1*vil3d_I(+0,-1,+0) + s2*vil3d_I(+1,-1,+0) + s3*vil3d_I(+2,-1,+0);
  double xi11 = s0*vil3d_I(-1,+0,+0) + s1*vil3d_I(+0,+0,+0) + s2*vil3d_I(+1,+0,+0) + s3*vil3d_I(+2,+0,+0);
  double xi21 = s0*vil3d_I(-1,+1,+0) + s1*vil3d_I(+0,+1,+0) + s2*vil3d_I(+1,+1,+0) + s3*vil3d_I(+2,+1,+0);
  double xi31 = s0*vil3d_I(-1,+2,+0) + s1*vil3d_I(+0,+2,+0) + s2*vil3d_I(+1,+2,+0) + s3*vil3d_I(+2,+2,+0);

  double xi02 = s0*vil3d_I(-1,-1,+1) + s1*vil3d_I(+0,-1,+1) + s2*vil3d_I(+1,-1,+1) + s3*vil3d_I(+2,-1,+1);
  double xi12 = s0*vil3d_I(-1,+0,+1) + s1*vil3d_I(+0,+0,+1) + s2*vil3d_I(+1,+0,+1) + s3*vil3d_I(+2,+0,+1);
  double xi22 = s0*vil3d_I(-1,+1,+1) + s1*vil3d_I(+0,+1,+1) + s2*vil3d_I(+1,+1,+1) + s3*vil3d_I(+2,+1,+1);
  double xi32 = s0*vil3d_I(-1,+2,+1) + s1*vil3d_I(+0,+2,+1) + s2*vil3d_I(+1,+2,+1) + s3*vil3d_I(+2,+2,+1);

  double xi03 = s0*vil3d_I(-1,-1,+2) + s1*vil3d_I(+0,-1,+2) + s2*vil3d_I(+1,-1,+2) + s3*vil3d_I(+2,-1,+2);
  double xi13 = s0*vil3d_I(-1,+0,+2) + s1*vil3d_I(+0,+0,+2) + s2*vil3d_I(+1,+0,+2) + s3*vil3d_I(+2,+0,+2);
  double xi23 = s0*vil3d_I(-1,+1,+2) + s1*vil3d_I(+0,+1,+2) + s2*vil3d_I(+1,+1,+2) + s3*vil3d_I(+2,+1,+2);
  double xi33 = s0*vil3d_I(-1,+2,+2) + s1*vil3d_I(+0,+2,+2) + s2*vil3d_I(+1,+2,+2) + s3*vil3d_I(+2,+2,+2);

#undef vil3d_I

  double val0 = xi00*t0 + xi10*t1 + xi20*t2 + xi30*t3;
  double val1 = xi01*t0 + xi11*t1 + xi21*t2 + xi31*t3;
  double val2 = xi02*t0 + xi12*t1 + xi22*t2 + xi32*t3;
  double val3 = xi03*t0 + xi13*t1 + xi23*t2 + xi33*t3;

  double val = 0.125 * ( val0*u0 + val1*u1 + val2*u2 + val3*u3 );

  return val;
}

#ifdef _MSC_VER
#pragma optimize( "tpgsy", off )
#endif // _MSC_VER

template<class T>
double vil3d_tricub_interp_safe_trilinear_extend(double x, double y, double z,
                                                 const T* data,
                                                 int nx, int ny, int nz,
                                                 std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  if ( x>=1    && y>=1    && z>=1 &&
       x<=nx-3 && y<=ny-3 && z<=nz-3 )
    return vil3d_tricub_interp_raw(x,y,z,data,xstep,ystep,zstep);

  int p1x=vil_round_floor(x);
  double normx = x - p1x;
  int p1y=vil_round_floor(y);
  double normy = y - p1y;
  int p1z=vil_round_floor(z);
  double normz = z - p1z;

  const T* pix1 = data + p1y*ystep + p1x*xstep + p1z*zstep;
  double xi00 = std::numeric_limits<double>::infinity();
  double xi10 = std::numeric_limits<double>::infinity();
  double xi20 = std::numeric_limits<double>::infinity();
  double xi30 = std::numeric_limits<double>::infinity();
  double xi01 = std::numeric_limits<double>::infinity();
  double xi11 = std::numeric_limits<double>::infinity();
  double xi21 = std::numeric_limits<double>::infinity();
  double xi31 = std::numeric_limits<double>::infinity();
  double xi02 = std::numeric_limits<double>::infinity();
  double xi12 = std::numeric_limits<double>::infinity();
  double xi22 = std::numeric_limits<double>::infinity();
  double xi32 = std::numeric_limits<double>::infinity();
  double xi03 = std::numeric_limits<double>::infinity();
  double xi13 = std::numeric_limits<double>::infinity();
  double xi23 = std::numeric_limits<double>::infinity();
  double xi33 = std::numeric_limits<double>::infinity();

  double val0 = std::numeric_limits<double>::infinity();
  double val1 = std::numeric_limits<double>::infinity();
  double val2 = std::numeric_limits<double>::infinity();
  double val3 = std::numeric_limits<double>::infinity();

  double val  = std::numeric_limits<double>::infinity();

#define vil3d_I(dx,dy,dz) (pix1[(dx)*xstep+(dy)*ystep+(dz)*zstep])
  if ( x < 0 ) x = 0.0;
  if ( y < 0 ) y = 0.0;
  if ( z < 0 ) z = 0.0;
  if ( x > nx-1 ) x = nx-1;
  if ( y > ny-1 ) y = ny-1;
  if ( z > nz-1 ) z = nz-1;

  if (z<1 || z>nz-3)
  {
    if (y<1 || y>ny-3)
    {
      if (x<1 || x>nx-3)
      {
        xi11 = vil3d_I(+0,+0,+0) + (vil3d_I(+1,+0,+0)-vil3d_I(+0,+0,+0))*normx;
        xi21 = vil3d_I(+0,+1,+0) + (vil3d_I(+1,+1,+0)-vil3d_I(+0,+1,+0))*normx;

        xi12 = vil3d_I(+0,+0,+1) + (vil3d_I(+1,+0,+1)-vil3d_I(+0,+0,+1))*normx;
        xi22 = vil3d_I(+0,+1,+1) + (vil3d_I(+1,+1,+1)-vil3d_I(+0,+1,+1))*normx;
      }
      else
      {
        double s0, s1, s2, s3;
        get_tricubic_coeff( normx, s0, s1, s2, s3 );

        xi11 = 0.5 * ( s0*vil3d_I(-1,+0,+0) +
                       s1*vil3d_I(+0,+0,+0) +
                       s2*vil3d_I(+1,+0,+0) +
                       s3*vil3d_I(+2,+0,+0) );
        xi21 = 0.5 * ( s0*vil3d_I(-1,+1,+0) +
                       s1*vil3d_I(+0,+1,+0) +
                       s2*vil3d_I(+1,+1,+0) +
                       s3*vil3d_I(+2,+1,+0) );

        xi12 = 0.5 * ( s0*vil3d_I(-1,+0,+1) +
                       s1*vil3d_I(+0,+0,+1) +
                       s2*vil3d_I(+1,+0,+1) +
                       s3*vil3d_I(+2,+0,+1) );
        xi22 = 0.5 * ( s0*vil3d_I(-1,+1,+1) +
                       s1*vil3d_I(+0,+1,+1) +
                       s2*vil3d_I(+1,+1,+1) +
                       s3*vil3d_I(+2,+1,+1) );
      } // end if/else x

      val1 = xi11 + (xi21 - xi11)*normy;
      val2 = xi12 + (xi22 - xi12)*normy;
    } // end of if y
    else
    {
      if (x<1 || x>nx-3)
      {
        xi01 = vil3d_I(+0,-1,+0) + (vil3d_I(+1,-1,+0)-vil3d_I(+0,-1,+0))*normx;
        xi11 = vil3d_I(+0,+0,+0) + (vil3d_I(+1,+0,+0)-vil3d_I(+0,+0,+0))*normx;
        xi21 = vil3d_I(+0,+1,+0) + (vil3d_I(+1,+1,+0)-vil3d_I(+0,+1,+0))*normx;
        xi31 = vil3d_I(+0,+2,+0) + (vil3d_I(+1,+2,+0)-vil3d_I(+0,+2,+0))*normx;

        xi02 = vil3d_I(+0,-1,+1) + (vil3d_I(+1,-1,+1)-vil3d_I(+0,-1,+1))*normx;
        xi12 = vil3d_I(+0,+0,+1) + (vil3d_I(+1,+0,+1)-vil3d_I(+0,+0,+1))*normx;
        xi22 = vil3d_I(+0,+1,+1) + (vil3d_I(+1,+1,+1)-vil3d_I(+0,+1,+1))*normx;
        xi32 = vil3d_I(+0,+2,+1) + (vil3d_I(+1,+2,+1)-vil3d_I(+0,+2,+1))*normx;
      }
      else
      {
        double s0, s1, s2, s3;
        get_tricubic_coeff( normx, s0, s1, s2, s3 );

        xi01 = 0.5 * ( s0*vil3d_I(-1,-1,+0) +
                       s1*vil3d_I(+0,-1,+0) +
                       s2*vil3d_I(+1,-1,+0) +
                       s3*vil3d_I(+2,-1,+0) );
        xi11 = 0.5 * ( s0*vil3d_I(-1,+0,+0) +
                       s1*vil3d_I(+0,+0,+0) +
                       s2*vil3d_I(+1,+0,+0) +
                       s3*vil3d_I(+2,+0,+0) );
        xi21 = 0.5 * ( s0*vil3d_I(-1,+1,+0) +
                       s1*vil3d_I(+0,+1,+0) +
                       s2*vil3d_I(+1,+1,+0) +
                       s3*vil3d_I(+2,+1,+0) );
        xi31 = 0.5 * ( s0*vil3d_I(-1,+2,+0) +
                       s1*vil3d_I(+0,+2,+0) +
                       s2*vil3d_I(+1,+2,+0) +
                       s3*vil3d_I(+2,+2,+0) );

        xi02 = 0.5 * ( s0*vil3d_I(-1,-1,+1) +
                       s1*vil3d_I(+0,-1,+1) +
                       s2*vil3d_I(+1,-1,+1) +
                       s3*vil3d_I(+2,-1,+1) );
        xi12 = 0.5 * ( s0*vil3d_I(-1,+0,+1) +
                       s1*vil3d_I(+0,+0,+1) +
                       s2*vil3d_I(+1,+0,+1) +
                       s3*vil3d_I(+2,+0,+1) );
        xi22 = 0.5 * ( s0*vil3d_I(-1,+1,+1) +
                       s1*vil3d_I(+0,+1,+1) +
                       s2*vil3d_I(+1,+1,+1) +
                       s3*vil3d_I(+2,+1,+1) );
        xi32 = 0.5 * ( s0*vil3d_I(-1,+2,+1) +
                       s1*vil3d_I(+0,+2,+1) +
                       s2*vil3d_I(+1,+2,+1) +
                       s3*vil3d_I(+2,+2,+1) );
      } // end if/else x

      double t0, t1, t2, t3;
      get_tricubic_coeff( normy, t0, t1, t2, t3 );
      val1 = 0.5 * ( xi01*t0 + xi11*t1 + xi21*t2 + xi31*t3 );
      val2 = 0.5 * ( xi02*t0 + xi12*t1 + xi22*t2 + xi32*t3 );
    } // end of else y

    val = val1 + (val2 - val1) * normz;
  } //end of if z
  else
  {
    if (y<1 || y> ny-3)
    {
      if (x<1 || x>nx-3)
      {
        xi10 = vil3d_I(+0,+0,-1) + (vil3d_I(+1,+0,-1)-vil3d_I(+0,+0,-1))*normx;
        xi20 = vil3d_I(+0,+1,-1) + (vil3d_I(+1,+1,-1)-vil3d_I(+0,+1,-1))*normx;

        xi11 = vil3d_I(+0,+0,+0) + (vil3d_I(+1,+0,+0)-vil3d_I(+0,+0,+0))*normx;
        xi21 = vil3d_I(+0,+1,+0) + (vil3d_I(+1,+1,+0)-vil3d_I(+0,+1,+0))*normx;

        xi12 = vil3d_I(+0,+0,+1) + (vil3d_I(+1,+0,+1)-vil3d_I(+0,+0,+1))*normx;
        xi22 = vil3d_I(+0,+1,+1) + (vil3d_I(+1,+1,+1)-vil3d_I(+0,+1,+1))*normx;

        xi13 = vil3d_I(+0,+0,+2) + (vil3d_I(+1,+0,+2)-vil3d_I(+0,+0,+2))*normx;
        xi23 = vil3d_I(+0,+1,+2) + (vil3d_I(+1,+1,+2)-vil3d_I(+0,+1,+2))*normx;
      }
      else
      {
        double s0, s1, s2, s3;
        get_tricubic_coeff( normx, s0, s1, s2, s3 );

        xi10 = 0.5 * ( s0*vil3d_I(-1,+0,-1) +
                       s1*vil3d_I(+0,+0,-1) +
                       s2*vil3d_I(+1,+0,-1) +
                       s3*vil3d_I(+2,+0,-1) );
        xi20 = 0.5 * ( s0*vil3d_I(-1,+1,-1) +
                       s1*vil3d_I(+0,+1,-1) +
                       s2*vil3d_I(+1,+1,-1) +
                       s3*vil3d_I(+2,+1,-1) );

        xi11 = 0.5 * ( s0*vil3d_I(-1,+0,+0) +
                       s1*vil3d_I(+0,+0,+0) +
                       s2*vil3d_I(+1,+0,+0) +
                       s3*vil3d_I(+2,+0,+0) );
        xi21 = 0.5 * ( s0*vil3d_I(-1,+1,+0) +
                       s1*vil3d_I(+0,+1,+0) +
                       s2*vil3d_I(+1,+1,+0) +
                       s3*vil3d_I(+2,+1,+0) );

        xi12 = 0.5 * ( s0*vil3d_I(-1,+0,+1) +
                       s1*vil3d_I(+0,+0,+1) +
                       s2*vil3d_I(+1,+0,+1) +
                       s3*vil3d_I(+2,+0,+1) );
        xi22 = 0.5 * ( s0*vil3d_I(-1,+1,+1) +
                       s1*vil3d_I(+0,+1,+1) +
                       s2*vil3d_I(+1,+1,+1) +
                       s3*vil3d_I(+2,+1,+1) );

        xi13 = 0.5 * ( s0*vil3d_I(-1,+0,+2) +
                       s1*vil3d_I(+0,+0,+2) +
                       s2*vil3d_I(+1,+0,+2) +
                       s3*vil3d_I(+2,+0,+2) );
        xi23 = 0.5 * ( s0*vil3d_I(-1,+1,+2) +
                       s1*vil3d_I(+0,+1,+2) +
                       s2*vil3d_I(+1,+1,+2) +
                       s3*vil3d_I(+2,+1,+2) );
      } // end if/else x

      val0 = xi10 + (xi20 - xi10)*normy;
      val1 = xi11 + (xi21 - xi11)*normy;
      val2 = xi12 + (xi22 - xi12)*normy;
      val3 = xi13 + (xi23 - xi13)*normy;
    } // end of if y
    else
    {
      if (x<1 || x>nx-3)
      {
        xi00 = vil3d_I(+0,-1,-1) + (vil3d_I(+1,-1,-1)-vil3d_I(+0,-1,-1))*normx;
        xi10 = vil3d_I(+0,+0,-1) + (vil3d_I(+1,+0,-1)-vil3d_I(+0,+0,-1))*normx;
        xi20 = vil3d_I(+0,+1,-1) + (vil3d_I(+1,+1,-1)-vil3d_I(+0,+1,-1))*normx;
        xi30 = vil3d_I(+0,+2,-1) + (vil3d_I(+1,+2,-1)-vil3d_I(+0,+2,-1))*normx;

        xi01 = vil3d_I(+0,-1,+0) + (vil3d_I(+1,-1,+0)-vil3d_I(+0,-1,+0))*normx;
        xi11 = vil3d_I(+0,+0,+0) + (vil3d_I(+1,+0,+0)-vil3d_I(+0,+0,+0))*normx;
        xi21 = vil3d_I(+0,+1,+0) + (vil3d_I(+1,+1,+0)-vil3d_I(+0,+1,+0))*normx;
        xi31 = vil3d_I(+0,+2,+0) + (vil3d_I(+1,+2,+0)-vil3d_I(+0,+2,+0))*normx;

        xi02 = vil3d_I(+0,-1,+1) + (vil3d_I(+1,-1,+1)-vil3d_I(+0,-1,+1))*normx;
        xi12 = vil3d_I(+0,+0,+1) + (vil3d_I(+1,+0,+1)-vil3d_I(+0,+0,+1))*normx;
        xi22 = vil3d_I(+0,+1,+1) + (vil3d_I(+1,+1,+1)-vil3d_I(+0,+1,+1))*normx;
        xi32 = vil3d_I(+0,+2,+1) + (vil3d_I(+1,+2,+1)-vil3d_I(+0,+2,+1))*normx;

        xi03 = vil3d_I(+0,-1,+2) + (vil3d_I(+1,-1,+2)-vil3d_I(+0,-1,+2))*normx;
        xi13 = vil3d_I(+0,+0,+2) + (vil3d_I(+1,+0,+2)-vil3d_I(+0,+0,+2))*normx;
        xi23 = vil3d_I(+0,+1,+2) + (vil3d_I(+1,+1,+2)-vil3d_I(+0,+1,+2))*normx;
        xi33 = vil3d_I(+0,+2,+2) + (vil3d_I(+1,+2,+2)-vil3d_I(+0,+2,+2))*normx;
      }
      else
      {
        double s0, s1, s2, s3;
        get_tricubic_coeff( normx, s0, s1, s2, s3 );

        xi00 = 0.5 * ( s0*vil3d_I(-1,-1,-1) +
                       s1*vil3d_I(+0,-1,-1) +
                       s2*vil3d_I(+1,-1,-1) +
                       s3*vil3d_I(+2,-1,-1) );
        xi10 = 0.5 * ( s0*vil3d_I(-1,+0,-1) +
                       s1*vil3d_I(+0,+0,-1) +
                       s2*vil3d_I(+1,+0,-1) +
                       s3*vil3d_I(+2,+0,-1) );
        xi20 = 0.5 * ( s0*vil3d_I(-1,+1,-1) +
                       s1*vil3d_I(+0,+1,-1) +
                       s2*vil3d_I(+1,+1,-1) +
                       s3*vil3d_I(+2,+1,-1) );
        xi30 = 0.5 * ( s0*vil3d_I(-1,+2,-1) +
                       s1*vil3d_I(+0,+2,-1) +
                       s2*vil3d_I(+1,+2,-1) +
                       s3*vil3d_I(+2,+2,-1) );

        xi01 = 0.5 * ( s0*vil3d_I(-1,-1,+0) +
                       s1*vil3d_I(+0,-1,+0) +
                       s2*vil3d_I(+1,-1,+0) +
                       s3*vil3d_I(+2,-1,+0) );
        xi11 = 0.5 * ( s0*vil3d_I(-1,+0,+0) +
                       s1*vil3d_I(+0,+0,+0) +
                       s2*vil3d_I(+1,+0,+0) +
                       s3*vil3d_I(+2,+0,+0) );
        xi21 = 0.5 * ( s0*vil3d_I(-1,+1,+0) +
                       s1*vil3d_I(+0,+1,+0) +
                       s2*vil3d_I(+1,+1,+0) +
                       s3*vil3d_I(+2,+1,+0) );
        xi31 = 0.5 * ( s0*vil3d_I(-1,+2,+0) +
                       s1*vil3d_I(+0,+2,+0) +
                       s2*vil3d_I(+1,+2,+0) +
                       s3*vil3d_I(+2,+2,+0) );

        xi02 = 0.5 * ( s0*vil3d_I(-1,-1,+1) +
                       s1*vil3d_I(+0,-1,+1) +
                       s2*vil3d_I(+1,-1,+1) +
                       s3*vil3d_I(+2,-1,+1) );
        xi12 = 0.5 * ( s0*vil3d_I(-1,+0,+1) +
                       s1*vil3d_I(+0,+0,+1) +
                       s2*vil3d_I(+1,+0,+1) +
                       s3*vil3d_I(+2,+0,+1) );
        xi22 = 0.5 * ( s0*vil3d_I(-1,+1,+1) +
                       s1*vil3d_I(+0,+1,+1) +
                       s2*vil3d_I(+1,+1,+1) +
                       s3*vil3d_I(+2,+1,+1) );
        xi32 = 0.5 * ( s0*vil3d_I(-1,+2,+1) +
                       s1*vil3d_I(+0,+2,+1) +
                       s2*vil3d_I(+1,+2,+1) +
                       s3*vil3d_I(+2,+2,+1) );

        xi03 = 0.5 * ( s0*vil3d_I(-1,-1,+2) +
                       s1*vil3d_I(+0,-1,+2) +
                       s2*vil3d_I(+1,-1,+2) +
                       s3*vil3d_I(+2,-1,+2) );
        xi13 = 0.5 * ( s0*vil3d_I(-1,+0,+2) +
                       s1*vil3d_I(+0,+0,+2) +
                       s2*vil3d_I(+1,+0,+2) +
                       s3*vil3d_I(+2,+0,+2) );
        xi23 = 0.5 * ( s0*vil3d_I(-1,+1,+2) +
                       s1*vil3d_I(+0,+1,+2) +
                       s2*vil3d_I(+1,+1,+2) +
                       s3*vil3d_I(+2,+1,+2) );
        xi33 = 0.5 * ( s0*vil3d_I(-1,+2,+2) +
                       s1*vil3d_I(+0,+2,+2) +
                       s2*vil3d_I(+1,+2,+2) +
                       s3*vil3d_I(+2,+2,+2) );
      } // end if/else x

      double t0, t1, t2, t3;
      get_tricubic_coeff( normy, t0, t1, t2, t3 );
      val0 = 0.5 * ( xi00*t0 + xi10*t1 + xi20*t2 + xi30*t3 );
      val1 = 0.5 * ( xi01*t0 + xi11*t1 + xi21*t2 + xi31*t3 );
      val2 = 0.5 * ( xi02*t0 + xi12*t1 + xi22*t2 + xi32*t3 );
      val3 = 0.5 * ( xi03*t0 + xi13*t1 + xi23*t2 + xi33*t3 );
    } // end of else y

    double u0, u1, u2, u3;
    get_tricubic_coeff( normz, u0, u1, u2, u3 );

    val = 0.5 * ( val0*u0 + val1*u1 + val2*u2 + val3*u3 );
  } //end of else z

  return val;
}

#define VIL3D_TRICUB_INTERP_INSTANTIATE(T)                              \
template double vil3d_tricub_interp_raw (double x, double y, double z, const T* data,  \
                                         std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep); \
template double vil3d_tricub_interp_safe_trilinear_extend (double x, double y, double z, const T* data,  \
                                                           int nx, int ny, int nz, \
                                                           std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)

#endif // vil3d_tricub_interp_hxx_
