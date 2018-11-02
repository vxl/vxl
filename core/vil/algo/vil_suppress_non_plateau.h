// This is core/vil/algo/vil_suppress_non_plateau.h
#ifndef vil_suppress_non_plateau_h_
#define vil_suppress_non_plateau_h_
//:
// \file
// \brief Suppress all non-plateau points in image
// \author Tim Cootes, Kevin de Souza

#include <vil/vil_image_view.h>
#include <vil/algo/vil_find_plateaus.h>
#include <vil/vil_fill.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Suppress all non-plateau pixels in the image.
//  If image(i,j) is greater than or equal to all neighbouring pixels,
//  and is above the threshold, then it is retained. All other
//  pixels are set to non_max_value.
//
//  non_max_value must be below the threshold (so the default value of
//  zero is inappropriate if the image contains plateaus of interest with
//  negative values)
//
// \sa vil_suppress_non_max_3x3()
// \relatesalso vil_image_view
template <class T>
inline void vil_suppress_non_plateau_3x3(const vil_image_view<T>& src_im,
                                         vil_image_view<T>& dest_im,
                                         T threshold=0, T non_max_value=0)
{
  unsigned ni=src_im.ni(),nj=src_im.nj();
  assert(src_im.nplanes()==1);

  dest_im.set_size(ni,nj,1);

  std::ptrdiff_t istep = src_im.istep(),jstep=src_im.jstep();
  std::ptrdiff_t distep = dest_im.istep(),djstep=dest_im.jstep();
  const T* row = src_im.top_left_ptr()+istep+jstep;
  T* drow = dest_im.top_left_ptr()+distep+djstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep,drow+=djstep)
  {
    const T* pixel = row;
    T* dpixel = drow;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep,dpixel+=distep)
    {
      if (*pixel<threshold || !vil_is_plateau_3x3(pixel,istep,jstep))
        *dpixel = non_max_value;
      else
        *dpixel = *pixel;
    }
  }

  // Border pixels assumed not to be local plateaus
  vil_fill_row(dest_im,0,non_max_value);
  vil_fill_row(dest_im,nj-1,non_max_value);
  vil_fill_col(dest_im,0,non_max_value);
  vil_fill_col(dest_im,ni-1,non_max_value);
}

#endif // vil_suppress_non_plateau_h_
