// This is core/vil/algo/vil_find_peaks.h
#ifndef vil_find_peaks_h_
#define vil_find_peaks_h_
//:
// \file
// \brief Find peaks in image
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>

//: True if pixel at *im is strictly above 8 neighbours
template <class T>
inline bool vil_is_peak_3x3(const T* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
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

//: Return (pi,pj) for all points in image strictly above their 8 neighbours
//  Compute position of all local peaks (pi[k],pj[k]) above given threshold value.
// \param clear_list  If true (the default) then empty lists before adding new examples
template <class T>
inline void vil_find_peaks_3x3(vcl_vector<unsigned>& pi,
                               vcl_vector<unsigned>& pj,
                               const vil_image_view<T>& image,
                               const T& min_thresh,
                               bool clear_list=true)
{
  if (clear_list) {
    pi.resize(0);
    pj.resize(0);
  }
  const unsigned ni1=image.ni()-1,nj1=image.nj()-1;
  const vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+istep+jstep;
  for (unsigned j=1;j<nj1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni1;++i,pixel+=istep)
      if (*pixel>=min_thresh && vil_is_peak_3x3(pixel,istep,jstep))
      { pi.push_back(i); pj.push_back(j); }
  }
}

#endif // vil_find_peaks_h_
