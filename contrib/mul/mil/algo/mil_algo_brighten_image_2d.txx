#ifndef mil_algo_brighten_image_2d_txx_
#define mil_algo_brighten_image_2d_txx_
//:
//  \file
//  \brief Functions to change the contrast and brightness of 2d images
//  \author Ian Scott

#include "mil_algo_brighten_image_2d.h"

//: creates a brighter or darker version of an input image.
// dest.pixel = src.pixel * scale
template <class OUTPIX, class INPIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX>& dest,
                                const mil_image_2d_of<INPIX>& src,
                                SCALE scale)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();

  // calculate the scaled image
  for (int i=0;i<n;++i)
  {
    const INPIX* s_row = src.plane(i);
    OUTPIX* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const INPIX* s = s_row;
      OUTPIX * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = scale * (*s);
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }

  dest.setWorld2im(src.world2im());
}

//: creates a brighter or darker version of an input image.
// dest.pixel = src.pixel * scale + offset
template <class OUTPIX, class INPIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX>& dest,
                                const mil_image_2d_of<INPIX>& src,
                                SCALE scale, SCALE offset)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();

  // calculate the scaled image
  for (int i=0;i<n;++i)
  {
    const INPIX* s_row = src.plane(i);
    OUTPIX* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const INPIX* s = s_row;
      OUTPIX * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = scale * (*s) + offset;
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }

  dest.setWorld2im(src.world2im());
}


//: Brighten an image.
// image.pixel = image.pixel * scale
template <class PIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<PIX>& image,
                                SCALE scale)
{
  int n = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int xstep=image.xstep();
  int ystep=image.ystep();

  // calculate the scaled image
  for (int i=0;i<n;++i)
  {
    PIX* row = image.plane(i);

    for (int y=0;y<ny;++y)
    {
      PIX * p = row;

      for (int x=0;x<nx;++x)
      {
        *p = scale * (*p);
        p+=xstep;
      }

      row += ystep;
    }
  }
}

//: Brighten an image.
// image.pixel = image.pixel * scale + offset
template <class PIX, class SCALE>
void mil_algo_brighten_image_2d(mil_image_2d_of<PIX>& image,
                                SCALE scale, SCALE offset)
{
  int n = image.n_planes();
  int nx = image.nx();
  int ny = image.ny();
  int xstep=image.xstep();
  int ystep=image.ystep();

  // calculate the scaled image
  for (int i=0;i<n;++i)
  {
    PIX* row = image.plane(i);

    for (int y=0;y<ny;++y)
    {
      PIX * p = row;

      for (int x=0;x<nx;++x)
      {
        *p = scale * (*p) + offset;
        p+=xstep;
      }

      row += ystep;
    }
  }
}

#define MIL_ALGO_BRIGHTEN_IMAGE_2D_INSTANTIATE( OUTPIX, INPIX, SCALE ) \
template void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX >& dest, \
                         const mil_image_2d_of<INPIX >& src, \
                         SCALE scale); \
template void mil_algo_brighten_image_2d(mil_image_2d_of<OUTPIX >& dest, \
                         const mil_image_2d_of<INPIX >& src, \
                         SCALE scale, SCALE offset); \
template void mil_algo_brighten_image_2d(mil_image_2d_of<INPIX >& image,\
                         SCALE scale); \
template void mil_algo_brighten_image_2d(mil_image_2d_of<INPIX >& dest, \
                         SCALE scale, SCALE offset)

#endif // mil_algo_brighten_image_2d_txx_
