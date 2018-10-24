// This is mul/vimt/algo/vimt_find_troughs.h
#ifndef vimt_find_troughs_h_
#define vimt_find_troughs_h_
//:
// \file
// \brief Find troughs in image
// \author Tim Cootes

#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_point_2d.h>

//: True if pixel at \p *im is strictly below its 8 neighbours
//  The image pixel (first argument) should \e not be a border pixel!
template <class T>
inline bool vimt_is_trough_3x3(const T* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
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
inline void vimt_find_image_troughs_3x3(std::vector<vgl_point_2d<unsigned> >& troughs,
                                        const vil_image_view<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  std::ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep))
        troughs.emplace_back(i,j);
  }
}

//: Return image co-ordinates of all points in image strictly below their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_troughs_3x3(std::vector<vgl_point_2d<unsigned> >& troughs,
                                        std::vector<T>& trough_value,
                                        const vil_image_view<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  std::ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep))
      {
        troughs.emplace_back(i,j);
        trough_value.push_back(*pixel);
      }
  }
}


//: Return world co-ordinates of all points in image strictly below their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_world_troughs_3x3(std::vector<vgl_point_2d<double> >& troughs,
                                        const vimt_image_2d_of<T>& image,
                                        unsigned plane=0, bool clear_list=true)
{
  if (clear_list) troughs.resize(0);
  const vil_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  std::ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep) // do not run over border
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep) // do not run over border
      if (vimt_is_trough_3x3(pixel,istep,jstep)) troughs.push_back(im2w(i,j));
  }
}

//: Return image co-ordinates of minimum value in image
//  (Or first one found if multiple equivalent minima)
template <class T>
inline
vgl_point_2d<unsigned> vimt_find_min(const vil_image_view<T>& im, unsigned plane=0)
{
  vgl_point_2d<unsigned> p(0,0);
  T min_val = im(0,0,plane);
  unsigned ni=im.ni(),nj=im.nj();
  std::ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep();
  for (unsigned j=0;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=0;i<ni;++i,pixel+=istep)
      if (*pixel<min_val)
      {
        min_val = *pixel;
        p = vgl_point_2d<unsigned>(i,j);
      }
  }
  return p;
}

//: Return world co-ordinates of minimum value in image
//  (Or first one found if multiple equivalent minima)
template <class T>
inline
vgl_point_2d<double> vimt_find_min(const vimt_image_2d_of<T>& image,unsigned plane=0)
{
  vgl_point_2d<unsigned> im_p = vimt_find_min(image.image(),plane);
  return image.world2im().inverse()(im_p.x(),im_p.y());
}

#endif // vimt_find_troughs_h_
