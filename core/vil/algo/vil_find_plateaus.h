// This is core/vil/algo/vil_find_plateaus.h
#ifndef vil_find_plateaus_h_
#define vil_find_plateaus_h_
//:
// \file
// \brief Find plateau points in image. Based on vil_find_peaks.h
// \author Tim Cootes, Kevin de Souza

#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: True if pixel at *im is greater than or equal to all 8 neighbours.
// \sa vil_is_peak_3x3()
template <class T>
inline bool vil_is_plateau_3x3(const T* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
  T v = *im;
  return v >= im[i_step]
      && v >= im[-i_step]
      && v >= im[j_step]
      && v >= im[-j_step]
      && v >= im[i_step+j_step]
      && v >= im[i_step-j_step]
      && v >= im[j_step-i_step]
      && v >= im[-i_step-j_step];
}

//: Return (pi,pj) for all points in image greater than or equal to all 8 neighbours.
//  Compute position of all local plateau points (pi[k],pj[k]) above given threshold value.
// \param clear_list  If true (the default) then empty lists before adding new examples
// \sa vil_find_peaks_3x3()
// \relatesalso vil_image_view
template <class T>
inline void vil_find_plateaus_3x3(std::vector<unsigned>& pi,
                                  std::vector<unsigned>& pj,
                                  const vil_image_view<T>& image,
                                  const T& min_thresh,
                                  bool clear_list=true)
{
  if (clear_list) {
    pi.resize(0);
    pj.resize(0);
  }
  const unsigned ni1=image.ni()-1,nj1=image.nj()-1;
  const std::ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+istep+jstep;
  for (unsigned j=1;j<nj1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni1;++i,pixel+=istep)
      if (*pixel>=min_thresh && vil_is_plateau_3x3(pixel,istep,jstep))
      {
        pi.push_back(i);
        pj.push_back(j);
      }
  }
}

#endif // vil_find_plateaus_h_
