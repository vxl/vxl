// This is mul/mil/mil_invert_image_2d.cxx
#include "mil_invert_image_2d.h"
//:
//  \file
//  \brief Function to invert 2d float plane images (black->white, white->black)
//  \author dac

//: creates an inverted version of an input image
void mil_invert_image_2d(mil_image_2d_of<float>& dest,
                         const mil_image_2d_of<float>& src)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();

  // calculate the max value of the src image
  float max=-10000;
  for (int i=0;i<n;++i)
  {
    const float* s_row = src.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      for (int x=0;x<nx;++x)
      {
        if (*s>max) max=*s;
        s+=sxstep;
      }

      s_row += systep;
    }
  }

  // calculate the inverted image
  for (int i=0;i<n;++i)
  {
    const float* s_row = src.plane(i);
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      float * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = max-*s;
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }

  dest.setWorld2im(src.world2im());
}
