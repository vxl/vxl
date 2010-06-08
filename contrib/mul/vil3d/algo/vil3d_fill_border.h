// This is contrib/mul/vil3d/algo/vil3d_fill_border.h
#ifndef vil3d_fill_border_h_
#define vil3d_fill_border_h_
//:
// \file
// \brief Fills border pixels of image with given value
// \author Tim Cootes

#include <vil3d/vil3d_crop.h>

//: Fills border pixels of image with given value
template<class T>
inline void vil3d_fill_border(vil3d_image_view<T>& im, unsigned bi, unsigned bj, unsigned bk, T value)
{
  unsigned ni=im.ni();
  if (ni<=2*bi) { im.fill(value); return; }
  unsigned nj=im.nj();
  if (nj<=2*bj) { im.fill(value); return; }
  unsigned nk=im.nk();
  if (nk<=2*bk) { im.fill(value); return; }

  // Fill borders by filling elements of cropped images
  if (bi>0)
  {
    vil3d_image_view<T> im_ilo=vil3d_crop(im,0,bi, 0,nj, 0,nk);
    im_ilo.fill(value);
    vil3d_image_view<T> im_ihi=vil3d_crop(im,ni-bi,bi, 0,nj, 0,nk);
    im_ihi.fill(value);
  }

  if (bj>0)
  {
    vil3d_image_view<T> im_jlo=vil3d_crop(im,0,ni, 0,bj, 0,nk);
    im_jlo.fill(value);
    vil3d_image_view<T> im_jhi=vil3d_crop(im,0,ni, bj-bj,bj, 0,nk);
    im_jhi.fill(value);
  }

  if (bk>0)
  {
    vil3d_image_view<T> im_klo=vil3d_crop(im,0,ni, 0,nj, 0,bk);
    im_klo.fill(value);
    vil3d_image_view<T> im_khi=vil3d_crop(im,0,ni, 0,nj, nk-bk,bk);
    im_khi.fill(value);
  }
}

#endif // vil3d_fill_border_h_
