// This is mul/mil/mil_convert_image_2d.cxx
#include "mil_convert_image_2d.h"
//:
//  \file
//  \brief Functions to convert images from one type to another
//  \author Tim Cootes

#include <vcl_cassert.h>

//: Copies src_im (of bytes) into dest_im (of float)
void mil_convert_image_2d(mil_image_2d_of<float>& dest,
                          const mil_image_2d_of<vxl_byte>& src)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();
  for (int i=0;i<n;++i)
  {
    const vxl_byte* s_row = src.plane(i);
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const vxl_byte* s = s_row;
      float * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = float(*s);
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }
  dest.setWorld2im(src.world2im());
}


//: Copies src_im (of bytes) into dest_im (of float) and flips actual data
void mil_convert_image_2d_flip(mil_image_2d_of<float>& dest,
                               const mil_image_2d_of<vxl_byte>& src)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();
  for (int i=0;i<n;++i)
  {
    const vxl_byte* s_row = src.plane(i)+(ny-1)*systep;
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const vxl_byte* s = s_row;
      float * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = float(*s);
        s+=sxstep;
        d+=dxstep;
      }

      s_row -= systep;
      d_row += dystep;
    }
  }
  // not sure about this ???
  dest.setWorld2im(src.world2im());
}


//: Converts float plane image to vxl_byte and stretches to 0-255 range
void mil_convert_image_2d_stretch(mil_image_2d_of<vxl_byte>& dest,
                                  const mil_image_2d_of<float>& src)
{
  float min, max;
  src.getRange(min,max);
  mil_convert_image_2d(dest,src,255/(max-min),-min);
}

//: Copies src_im (of float) into dest_im (of byte)
void mil_convert_image_2d(mil_image_2d_of<vxl_byte>& dest,
                          const mil_image_2d_of<float>& src)
{
  mil_convert_image_2d(dest,src,1.0,0);
}

//: Copies src_im (of float) into dest_im (of byte) after linear transform
//  Applies scale and offset to pixels of src_im and puts results in dest_im
void mil_convert_image_2d(mil_image_2d_of<vxl_byte>& dest,
                          const mil_image_2d_of<float>& src,
                          double scale, double offset)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();
  for (int i=0;i<n;++i)
  {
    const float* s_row = src.plane(i);
    vxl_byte* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      vxl_byte * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = vxl_byte( (*s + offset)* scale );
        //*d = vxl_byte(*s * scale + offset);
        s+=sxstep;
        d+=dxstep;
      }
      s_row += systep;
      d_row += dystep;
    }
  }
  dest.setWorld2im(src.world2im());
}

//: Convert 3plane RGB image to 1 plane greyscale image
void mil_rgb_to_greyscale(mil_image_2d_of<vxl_byte>& g_im,
                          const mil_image_2d_of<vxl_byte>& rgb_im)
{
  assert(rgb_im.n_planes()==3);

  int nx = rgb_im.nx();
  int ny = rgb_im.ny();
  g_im.resize(nx,ny);
  int cxstep=rgb_im.xstep();
  int cystep=rgb_im.ystep();
  int gxstep=g_im.xstep();
  int gystep=g_im.ystep();

  const vxl_byte* c0_row = rgb_im.plane(0);
  const vxl_byte* c1_row = rgb_im.plane(1);
  const vxl_byte* c2_row = rgb_im.plane(2);
  vxl_byte* grey_row = g_im.plane(0);

  for (int y=0;y<ny;++y)
  {
    const vxl_byte* c0 = c0_row;
    const vxl_byte* c1 = c1_row;
    const vxl_byte* c2 = c2_row;
    vxl_byte * g = grey_row;

    for (int x=0;x<nx;++x)
    {
      *g = vxl_byte((int(*c0)+int(*c1)+int(*c2))/3);
      g+=gxstep;
      c0+=cxstep;
      c1+=cxstep;
      c2+=cxstep;
    }
    grey_row += gystep;
    c0_row += cystep;
    c1_row += cystep;
    c2_row += cystep;
  }
  g_im.setWorld2im(rgb_im.world2im());
}

//: Convert 3plane RGB image to 1 plane greyscale image
void mil_rgb_to_greyscale(mil_image_2d_of<float>& g_im,
                          const mil_image_2d_of<float>& rgb_im)
{
  assert(rgb_im.n_planes()==3);

  int nx = rgb_im.nx();
  int ny = rgb_im.ny();
  g_im.resize(nx,ny);
  int cxstep=rgb_im.xstep();
  int cystep=rgb_im.ystep();
  int gxstep=g_im.xstep();
  int gystep=g_im.ystep();

  const float* c0_row = rgb_im.plane(0);
  const float* c1_row = rgb_im.plane(1);
  const float* c2_row = rgb_im.plane(2);
  float* grey_row = g_im.plane(0);

  for (int y=0;y<ny;++y)
  {
    const float* c0 = c0_row;
    const float* c1 = c1_row;
    const float* c2 = c2_row;
    float * g = grey_row;

    for (int x=0;x<nx;++x)
    {
      *g = ((*c0)+(*c1)+(*c2))/3;
      g+=gxstep;
      c0+=cxstep;
      c1+=cxstep;
      c2+=cxstep;
    }
    grey_row += gystep;
    c0_row += cystep;
    c1_row += cystep;
    c2_row += cystep;
  }
  g_im.setWorld2im(rgb_im.world2im());
}

