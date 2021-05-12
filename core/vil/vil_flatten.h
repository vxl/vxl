// This is core/vil/vil_flatten.h
#ifndef vil_flatten_h_
#define vil_flatten_h_
//:
// \file
// \author Ian Scott.

#include <vector>
#include "vil_image_view.h"
#include "vil_crop.h"
#include "vil_plane.h"


//: Rearrange multiple planes into a multiple tiles of a single-plane image.
// A ni x nj x nplanes images will be arranged into an ni x (nj*nplanes) x 1 image.
// A view transformation will be used if possible.
// \relatesalso vil_image_view
template<class T>
inline vil_image_view<T> vil_flatten_planes(const vil_image_view<T> &im)
{
  if (im.nplanes() == 1) return im;
  if (im.jstep() * im.nj() == im.planestep())
  {
    return vil_image_view<T>(im.memory_chunk(),
                             im.top_left_ptr(),
                             im.ni(), im.nj() * im.nplanes(), 1,
                             im.istep(),im.jstep(),im.ni() * im.nj() * im.nplanes());
  }

  vil_image_view<T> ret(im.ni(), im.nj() * im.nplanes(), 1);
  for (unsigned p=0, n=im.nplanes(); p<n; ++p)
  {
    vil_image_view<T> tile = vil_crop(ret, 0, im.ni(), im.nj()*p, im.nj());
    vil_copy_reformat(vil_plane(im, p), tile);
  }

  return ret;
}

//: Return a copy of a vil_image_view collapsed into one dimension.
// Row-major order (C-style); iterate plane, row, column.
// Consecutive elements of a row are adjacent in the resulting vector.
// [ (plane0, row0, col0), (p0, r0, c1), (p0, r0, c2) ... ]
template <class T>
inline std::vector<T> vil_flatten_row_major(const vil_image_view<T> &im)
{
  std::vector<T> vec(im.size(), T(0));
  size_t vec_i = 0;
  for (unsigned int p=0; p<im.nplanes(); ++p)  // plane
  {
    for (unsigned int j=0; j<im.nj(); ++j)  // row
    {
      for (unsigned int i=0; i<im.ni(); ++i)  // column
      {
        vec[vec_i] = im(i,j,p);
        ++vec_i;
      }
    }
  }
  return vec;
}

//: Return a copy of a vil_image_view collapsed into one dimension.
// Column-major order (Fortran-style); iterate plane, column, row.
// Consecutive elements of a column are adjacent in the resulting vector.
// [ (plane0, row0, col0), (p0, r1, c0), (p0, r2, c0) ... ]
template <class T>
inline std::vector<T> vil_flatten_column_major(const vil_image_view<T> &im)
{
  std::vector<T> vec(im.size(), T(0));
  size_t vec_i = 0;
  for (unsigned int p=0; p<im.nplanes(); ++p)  // plane
  {
    for (unsigned int i=0; i<im.ni(); ++i)  // column
    {
      for (unsigned int j=0; j<im.nj(); ++j)  // row
      {
        vec[vec_i] = im(i,j,p);
        ++vec_i;
      }
    }
  }
  return vec;
}

#endif // vil_flatten_h_
