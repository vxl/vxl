#ifndef mfpf_sample_region_h_
#define mfpf_sample_region_h_

//:
// \file
// \brief Sample vector from image, assuming istep==np, pstep=1
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <cstddef>
#include <mbl/mbl_chord.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Sample vector from image, assuming istep==np, pstep=1
//  Assumes vector already correct size, and region completely
//  within image.  Assumes vec has been sized to np*n_pixels.
template <class T>
inline void mfpf_sample_region(const T* image,
                               std::ptrdiff_t jstep, unsigned np,
                               const std::vector<mbl_chord>& roi,
                               vnl_vector<double>& vec)
{
  double *v = vec.data_block();
  std::vector<mbl_chord>::const_iterator c=roi.begin();
  for (;c!=roi.end();++c)
  {
    const T* im_row  = image+c->y()*jstep;
    const T* pix     = im_row + c->start_x()*np;
    const T* pix_end = im_row+(c->end_x()+1)*np;
    for (;pix!=pix_end;++pix,++v) *v=double(*pix);
  }
}

#endif // #define mfpf_sample_region_h_
