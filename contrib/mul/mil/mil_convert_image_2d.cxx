//: \file
//  \brief Functions to convert images from one type to another
//  \author Tim Cootes

#include <mil/mil_convert_image_2d.h>

//: Copys src_im (of bytes) into dest_im (of float)
void mil_convert_image_2d(mil_image_2d_of<float>& dest,
                          const mil_image_2d_of<vil_byte>& src)
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
    const vil_byte* s_row = src.plane(i);
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const vil_byte* s = s_row;
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

//: Copys src_im (of float) into dest_im (of byte)
void mil_convert_image_2d(mil_image_2d_of<vil_byte>& dest,
                          const mil_image_2d_of<float>& src)
{
  mil_convert_image_2d(dest,src,1.0,0);
}

//: Copys src_im (of float) into dest_im (of byte) after linear transform
//  Applies scale and offset to pixels of src_im and puts results in dest_im
void mil_convert_image_2d(mil_image_2d_of<vil_byte>& dest,
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
    vil_byte* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      vil_byte * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = vil_byte(*s * scale + offset);
        s+=sxstep;
        d+=dxstep;
      }
      s_row += systep;
      d_row += dystep;
    }
  }
  dest.setWorld2im(src.world2im());
}
