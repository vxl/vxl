// This is mul/vimt/algo/vimt_find_peaks.h
#ifndef vimt_find_peaks_h_
#define vimt_find_peaks_h_
//:
// \file
// \brief Find peaks in image
// \author Tim Cootes, VP (Sept03)

#include <vimt/vimt_image_2d_of.h>

//: True if pixel at *im is strictly above 8 neighbours
template <class T>
inline bool vimt_is_peak_3x3(const T* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  T v = *im;
  return v>im[i_step] &&
         v>im[-i_step] &&
         v>im[j_step] &&
         v>im[-j_step] &&
         v>im[i_step+j_step] &&
         v>im[i_step-j_step] &&
         v>im[j_step-i_step] &&
         v>im[-i_step-j_step];
}

//: True if pixel at *im is strictly above its neighbours in a 2*radius+1 neighbourhood
template <class T>
inline bool vimt_is_peak(const T* im, int radius, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  T v = *im;
  for (int i=-radius; i<radius+1; i++)
    for (int j=-radius; j<radius+1; j++)
      if (i!=0 || j!=0)
        if (v<=im[i_step*i+j_step*j]) return false;      // One of the 
  return true;
}


//: Return image co-ordinates of all points in image strictly above their 8 neighbours
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_peaks_3x3(vcl_vector<vgl_point_2d<unsigned> >& peaks,
                                      const vil_image_view<T>& image,
                                      unsigned plane=0, bool clear_list=true)
{
  if (clear_list) peaks.resize(0);
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep)
      if (vimt_is_peak_3x3(pixel,istep,jstep)) peaks.push_back(vgl_point_2d<unsigned>(i,j));
  }
}

//: Return image co-ordinates of all points in image strictly above their 8 neighbours
// \param peak_value: Value at peak
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_peaks_3x3(vcl_vector<vgl_point_2d<unsigned> >& peaks,
                                      vcl_vector<T>& peak_value,
                                      const vil_image_view<T>& image,
                                      unsigned plane=0, bool clear_list=true)
{
  if (clear_list) { peaks.resize(0); peak_value.resize(0); }
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep)
      if (vimt_is_peak_3x3(pixel,istep,jstep))
      { 
        peaks.push_back(vgl_point_2d<unsigned>(i,j));
        peak_value.push_back(*pixel);
      }
  }
}

//: Return image co-ordinates of all points in image strictly above their neighbours
//  in a 2*radius+1 x 2*radius+1 neighbourhood of pixels (e.g. r=2 equivalent to 5x5)
// \param peak_value: Value at peak
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_peaks(vcl_vector<vgl_point_2d<unsigned> >& peaks,
                                  vcl_vector<T>& peak_value,
                                  const vil_image_view<T>& image,
                                  unsigned radius=1,
                                  unsigned plane=0, bool clear_list=true)
{
  if (clear_list) { peaks.resize(0); peak_value.resize(0); }
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+plane*image.planestep()+radius*istep+radius*jstep;
  for (unsigned j=radius;j<nj-radius;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=radius;i<ni-radius;++i,pixel+=istep)
      if (vimt_is_peak(pixel,radius,istep,jstep))
      { 
        peaks.push_back(vgl_point_2d<unsigned>(i,j));
        peak_value.push_back(*pixel);
      }
  }
}

//: Return image co-ordinates of all points in image strictly above their neighbours
//  in a 2*radius+1 x 2*radius+1 neighbourhood of pixels (e.g. r=2 equivalent to 5x5)
//  Additionally, only peaks of the value higher than threshold (thresh) are returned.
// \param peak_value: Value at peak
// \param clear_list: If true (the default) then empty list before adding new examples
template <class T>
inline void vimt_find_image_peaks(vcl_vector<vgl_point_2d<unsigned> >& peaks,
                                  vcl_vector<T>& peak_value,
                                  const vil_image_view<T>& image,
                                  T thresh, unsigned radius=1,
                                  unsigned plane=0, bool clear_list=true)
{
  if (clear_list) { peaks.resize(0); peak_value.resize(0); }
  unsigned ni=image.ni(),nj=image.nj();
  vcl_ptrdiff_t istep = image.istep(),jstep=image.jstep();
  // Getting to the location of the starting point in the image (radius,radius)
  const T* row = image.top_left_ptr()+plane*image.planestep()+radius*istep+radius*jstep;
  for (unsigned j=radius;j<nj-radius;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=radius;i<ni-radius;++i,pixel+=istep)
    {
      if (*pixel>thresh)
      {
        if (vimt_is_peak(pixel,radius,istep,jstep))
        { 
          peaks.push_back(vgl_point_2d<unsigned>(i,j));
          peak_value.push_back(*pixel);
        }
      }
    }
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
  const vil_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep)
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
  const vil_image_view<T>& im = image.image();
  vimt_transform_2d im2w = image.world2im().inverse();
  unsigned ni=im.ni(),nj=im.nj();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep()+istep+jstep;
  for (unsigned j=1;j<nj-1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni-1;++i,pixel+=istep)
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
vgl_point_2d<unsigned> vimt_find_max(const vil_image_view<T>& im, unsigned plane=0)
{
  vgl_point_2d<unsigned> p(0,0);
  T max_val = im(0,0,plane);
  unsigned ni=im.ni(),nj=im.nj();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const T* row = im.top_left_ptr()+plane*im.planestep();
  for (unsigned j=0;j<nj;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=0;i<ni;++i,pixel+=istep)
      if (*pixel>max_val)
      {
        max_val = *pixel;
        p = vgl_point_2d<unsigned>(i,j);
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
  vgl_point_2d<unsigned> im_p = vimt_find_max(image.image(),plane);
  return image.world2im().inverse()(im_p.x(),im_p.y());
}


#endif // vimt_find_peaks_h_
