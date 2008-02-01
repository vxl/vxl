// This is mul/vimt/algo/vimt_find_troughs.h
#ifndef vimt_find_troughs_h_
#define vimt_find_troughs_h_
//:
// \file
// \brief Find troughs in image
// \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>

//: True if pixel at *im is strictly below its 8 neighbours
//  The image pixel (first argument) should *not* be a border pixel!
template <class T>
inline bool vimt_is_trough_3x3(const T* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  T v = *im;
  if (v>=im[i_step]) return false;
  if (v>=im[-i_step]) return false;
  if (v>=im[j_step]) return false;
  if (v>=im[-j_step]) return false;
  if (v>=im[i_step+j_step]) return false;
  if (v>=im[i_step-j_step]) return false;
  if (v>=im[j_step-i_step]) return false;
  if (v>=im[-i_step-j_step]) return false;
  return true;
}

//: Return image co-ordinates of all points in image strictly below their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_troughs_3x3(vcl_vector<vgl_point_2d<unsigned> >& troughs,
                                        const vil_image_view<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep))
        troughs.push_back(vgl_point_2d<unsigned>(i,j));
  }
}

//: Return image co-ordinates of all points in image strictly below their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_troughs_3x3(vcl_vector<vgl_point_2d<unsigned> >& troughs,
                                      vcl_vector<T>& trough_value,
                                        const vil_image_view<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep))
      {
        troughs.push_back(vgl_point_2d<unsigned>(i,j));
        trough_value.push_back(*pixel);
      }
  }
}


//: Return world co-ordinates of all points in image strictly below their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_world_troughs_3x3(vcl_vector<vgl_point_2d<double> >& troughs,
                                        const vimt_image_2d_of<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  const vil_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep)) troughs.push_back(im2w(i,j));
  }
}


#endif // vimt_find_troughs_h_
