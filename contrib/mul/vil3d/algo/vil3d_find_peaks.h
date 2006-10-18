// This is core/vil/algo/vil3d_find_peaks.h
#ifndef vil3d_find_peaks_h_
#define vil3d_find_peaks_h_
//:
// \file
// \brief Find peaks in image
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>

//: True if value v is strictly above 8 neighbours of *im in i and j
template <class T>
inline bool vil3d_is_above_8nbrs(T v, const T* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
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

//: True if *im is strictly above 26 neighbours in i,j,k
template <class T>
inline bool vil3d_is_peak26(const T* im, vcl_ptrdiff_t i_step,
                            vcl_ptrdiff_t j_step, vcl_ptrdiff_t k_step)
{
  if (!vil3d_is_above_8nbrs(im[0],im,i_step,j_step)) return false;
  if (*im<=im[k_step]) return false;
  if (!vil3d_is_above_8nbrs(im[0],im+k_step,i_step,j_step)) return false;
  if (*im<=im[-k_step]) return false;
  if (!vil3d_is_above_8nbrs(im[0],im-k_step,i_step,j_step)) return false;
  return true;
}

//: Return position of all points in image strictly above their 26 neighbours
//  Compute position of all local peaks (pi[k],pj[k]) above given threshold value.
// \param clear_list  If true (the default) then empty lists before adding new examples
template <class T>
inline void vil3d_find_peaks_26(vcl_vector<vgl_point_3d<int> >& peaks,
                               const vil3d_image_view<T>& image,
                               const T& min_thresh,
                               bool clear_list=true)
{
  if (clear_list) peaks.resize(0);

  const unsigned ni1=image.ni()-1,nj1=image.nj()-1,nk1=image.nk()-1;
  const vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep(),kstep=image.kstep();
  const T* plane = image.origin_ptr()+istep+jstep+kstep;
  for (unsigned k=1;k<nk1;++k,plane+=kstep)
  {
    const T* row = plane;
    for (unsigned j=1;j<nj1;++j,row+=jstep)
    {
      const T* pixel = row;
      for (unsigned i=1;i<ni1;++i,pixel+=istep)
        if (*pixel>=min_thresh && vil3d_is_peak26(pixel,istep,jstep,kstep))
        { peaks.push_back(vgl_point_3d<int>(i,j,k)); }
    }
  }
}

#endif // vil3d_find_peaks_h_
