// This is core/vil2/algo/vil2_suppress_non_max.h
#ifndef vil2_suppress_non_max_h_
#define vil2_suppress_non_max_h_
//:
// \file
// \brief Suppress all non-maximal points in image
// \author Tim Cootes

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_fill.h>
#include <vcl_cassert.h>

//: True if pixel at *im is strictly above 8 neighbours
template <class T>
inline bool vil2_is_peak_3x3(const T* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  T v = *im;
  if (v<=im[i_step]) return false;
  if (v<=im[-i_step]) return false;
  if (v<=im[j_step]) return false;
  if (v<=im[-j_step]) return false;
  if (v<=im[i_step+j_step]) return false;
  if (v<=im[i_step-j_step]) return false;
  if (v<=im[j_step-i_step]) return false;
  if (v<=im[-i_step-j_step]) return false;
  return true;
}

//: Suppress all non-maximal (non peaks) pixels in the image
//  If image(i,j) is strictly larger than all neighbouring pixels,
//  and is above the threshold, then it is retained. All other
//  pixels are set to non_max_value.
//
//  non_max_value must be below the threshold (so the default value of
//  zero is inappropriate if the image contains peaks of interest with 
//  negative values)
//
//  Note that where there are neighbouring pixels with identical values
//  on a raised plateau, then all the pixels on the plateau will be
//  suppressed.  This can cause some peaks to be missed.  The effect
//  can be reduced by using float images and pre-smoothing slightly.
template <class T>
inline void vil2_suppress_non_max_3x3(const vil2_image_view<T>& src_im,
                                      vil2_image_view<T>& dest_im,
                                      T threshold=0, T non_max_value=0)
{
  unsigned ni=src_im.ni(),nj=src_im.nj();
  assert(src_im.nplanes()==1);

  dest_im.set_size(ni,nj,1);

  vcl_ptrdiff_t istep = src_im.istep(),jstep=src_im.jstep();
  vcl_ptrdiff_t distep = dest_im.istep(),djstep=dest_im.jstep();
  const T* row = src_im.top_left_ptr()+istep+jstep;
  T* drow = dest_im.top_left_ptr()+distep+djstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep,drow+=djstep)
  {
    const T* pixel = row;
    T* dpixel = drow;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep,dpixel+=distep)
    {
      if (*pixel<threshold || !vil2_is_peak_3x3(pixel,istep,jstep))
        *dpixel = non_max_value;
      else
        *dpixel = *pixel;
    }
  }

  // Border pixels assumed not to be local maxima
  vil2_fill_row(dest_im,0,non_max_value);
  vil2_fill_row(dest_im,nj-1,non_max_value);
  vil2_fill_col(dest_im,0,non_max_value);
  vil2_fill_col(dest_im,ni-1,non_max_value);
}

#endif // vil2_suppress_non_max_h_
