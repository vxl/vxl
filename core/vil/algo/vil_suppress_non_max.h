// This is vil2/algo/vil2_suppress_non_max.h
#ifndef vil2_suppress_non_max_h_
#define vil2_suppress_non_max_h_
//:
//  \file
//  \brief Suppress all non-maximal points in image
//  \author Tim Cootes


#include <vil2/vil2_image_view.h>
#include <vil2/vil2_fill.h>

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
//  on a raised plateau, then the pixel with the largest i,j will not be
//  suppressed (because the algorithm works in place).  Thus plateaus
//  can occasionally give spurious responses.
//  The alternative approach, to fill a new destination image, will not
//  return any pixels if there are two or more identically valued pixels
//  at a peak.
template <class T>
inline void vil2_suppress_non_max_3x3(vil2_image_view<T>& image,
                                      T threshold=0, T non_max_value=0)
{
  unsigned ni=image.ni(),nj=image.nj();
  assert(image.nplanes()==1);
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  T* row = image.top_left_ptr()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep)
  {
    T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep)
      if (*pixel<threshold || !vil2_is_peak_3x3(pixel,istep,jstep)) 
        *pixel = non_max_value;
  }

  // Border pixels assumed not to be local maxima
  vil2_fill_row(image,0,non_max_value);
  vil2_fill_row(image,nj-1,non_max_value);
  vil2_fill_col(image,0,non_max_value);
  vil2_fill_col(image,ni-1,non_max_value);
}

#endif

