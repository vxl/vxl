// This is mul/vimt/algo/vimt_find_peaks.h
#ifndef vimt_find_peaks_h_
#define vimt_find_peaks_h_
//:
//  \file
//  \brief Find peaks in image
//  \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>

//: True if pixel at *im is strictly above 8 neighbours
template <class T>
inline bool vimt_is_peak_3x3(const T* im, int i_step, int j_step)
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

//: Return image co-ordinates of all points in image strictly above their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_peaks_3x3(vcl_vector<vgl_point_2d<int> >& peaks,
                                      const vil2_image_view<T>& image,
                                      unsigned plane=0, bool clear_list=true)
{
  if (clear_list) peaks.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  int istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (int j=1;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (int i=1;i<ni;++i,pixel+=istep)
      if (vimt_is_peak_3x3(pixel,istep,jstep)) peaks.push_back(vgl_point_2d<int>(i,j));
  }
}

//: Return world co-ordinates of all points in image strictly above their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_world_peaks_3x3(vcl_vector<vgl_point_2d<double> >& peaks,
                                      const vimt_image_2d_of<T>& image,
                                      unsigned plane=0, bool clear_list=true)
{
  if (clear_list) peaks.resize(0);
  const vil2_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  int istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni;++i,pixel+=istep)
      if (vimt_is_peak_3x3(pixel,istep,jstep)) peaks.push_back(im2w(i,j));
  }
}

//: Return world co-ordinates of all points in image strictly above their 8 neighbours
// \param peak_pos: Position of each peak
// \param peak_value: Value at peak
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_world_peaks_3x3(vcl_vector<vgl_point_2d<double> >& peak_pos,
                                      vcl_vector<T>& peak_value,
                                      const vimt_image_2d_of<T>& image,
                                      unsigned plane=0, bool clear_list=true)
{
  if (clear_list) { peak_pos.resize(0); peak_value.resize(0); }
  const vil2_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  int istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni;++i,pixel+=istep)
      if (vimt_is_peak_3x3(pixel,istep,jstep))
      {
        peak_pos.push_back(im2w(i,j));
        peak_value.push_back(*pixel);
      }
  }
}

//: Return image co-ordinates of maximum value in image
//  (Or first one found if multiple equivalent maxima)
template <class T>
inline
vgl_point_2d<int> vimt_find_max(const vil2_image_view<T>& im, unsigned plane=0)
{
  vgl_point_2d<int> p(0,0);
  T max_val = im(0,0,plane);
  unsigned ni=im.ni(),nj=im.nj();
  int istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep();
  for (int j=0;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (int i=0;i<ni;++i,pixel+=istep)
      if (*pixel>max_val)
      {
        max_val = *pixel;
        p = vgl_point_2d<int>(i,j);
      }
  }
  return p;
}

//: Return world co-ordinates of maximum value in image
//  (Or first one found if multiple equivalent maxima)
template <class T>
inline
vgl_point_2d<double> vimt_find_max(const vimt_image_2d_of<T>& image,unsigned plane=0)
{
  vgl_point_2d<int> im_p = vimt_find_max(image.image(),plane);
  return image.world2im().inverse()(im_p.x(),im_p.y());
}


#endif // vimt_find_peaks_h_
