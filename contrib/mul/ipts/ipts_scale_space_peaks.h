// This is mul/ipts/ipts_scale_space_peaks.h
#ifndef ipts_scale_space_peaks_h_
#define ipts_scale_space_peaks_h_
//:
// \file
// \brief Find peaks in scale-space
// \author Tim Cootes

#include <vimt/algo/vimt_find_peaks.h>
#include <vgl/vgl_point_3d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vcl_cmath.h> // for sqrt(double)

//: True if value is strictly above *im and its 8 neighbours
template <class T>
inline bool ipts_is_above_3x3(T value, const T* im,
                              vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  if (value<=im[0]) return false;
  if (value<=im[i_step]) return false;
  if (value<=im[-i_step]) return false;
  if (value<=im[j_step]) return false;
  if (value<=im[-j_step]) return false;
  if (value<=im[i_step+j_step]) return false;
  if (value<=im[i_step-j_step]) return false;
  if (value<=im[j_step-i_step]) return false;
  if (value<=im[-i_step-j_step]) return false;
  return true;
}

//: Find local maxima in position and scale above given threshold
//  Points returned in a 3D point, given world coords + scale value
template<class T>
inline void ipts_scale_space_peaks_2d(vcl_vector<vgl_point_3d<double> >& peak_pts,
                               const vimt_image_2d_of<T>& image_below,
                               const vimt_image_2d_of<T>& image,
                               const vimt_image_2d_of<T>& image_above,
                               T threshold = 0,
                               bool clear_list = true)
{
  if (clear_list) { peak_pts.resize(0); }
  unsigned ni=image.image().ni(),nj=image.image().nj();
  vcl_ptrdiff_t istep = image.image().istep(),jstep=image.image().jstep();
  const T* row = &image.image()(1,1,0);

  const vil_image_view<T>& im_below = image_below.image();
  const vil_image_view<T>& im_above = image_above.image();

  vimt_transform_2d to_base  = image.world2im().inverse();
  vimt_transform_2d to_above = image_above.world2im() * to_base;
  vimt_transform_2d to_below = image_below.world2im() * to_base;

  // Estimate scaling factor between images (assumed isotropic)
  vgl_vector_2d<double> dx(1,0),dw;
  dw = image.world2im().delta(vgl_point_2d<double>(0,0),dx);
  double scale = 1.0/vcl_sqrt(dw.x()*dw.x()+dw.y()*dw.y());

  // Allow 2 pixel border to ensure avoid problems when
  // testing level above
  for (unsigned j=2;j<nj-2;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=2;i<ni-2;++i,pixel+=istep)
    {
      if (*pixel>threshold && vimt_is_peak_3x3(pixel,istep,jstep))
      {
        // (i,j) is local maxima at this level
        // Check it is also above all pixels nearby in level below
        vgl_point_2d<double> p0 = to_below(i,j);

        const T* pixel_below=&im_below(int(p0.x()+0.5),int(p0.y()+0.5));
        if (ipts_is_above_3x3(*pixel,pixel_below,
                                im_below.istep(),im_below.jstep())  )
        {
          // (i,j) is local maxima at the level below
          // Check it is also above all pixels nearby in level above
          vgl_point_2d<double> p1 = to_above(i,j);
          if (p1.x()>0.5 && p1.y()>0.5
              && p1.x()<im_above.ni()-2 && p1.y()<im_above.nj()-2)
          {
            const T* pixel_above=&im_above(int(p1.x()+0.5),int(p1.y()+0.5));
            if (ipts_is_above_3x3(*pixel,pixel_above,
                                  im_above.istep(),im_above.jstep()))
            {
              vgl_point_2d<double> p = to_base(i,j);
              peak_pts.push_back(vgl_point_3d<double>(p.x(),p.y(),scale));
            }
          }
        }
      }
    }
  }
}

//: Find local maxima in image0 and check against in image1
//  Returned points will be above all their neighbours and
//  those in nearby positions in image1
template<class T>
inline void ipts_scale_space_peaks_2d(vcl_vector<vgl_point_2d<double> >& peak_pts,
                               vcl_vector<T>& peak_values,
                               const vimt_image_2d_of<T>& image0,
                               const vimt_image_2d_of<T>& image1,
                               T threshold)
{
  vcl_vector<vgl_point_2d<double> > peak_pts0;
  vcl_vector<T> peak_values0;
  vimt_find_world_peaks_3x3(peak_pts0,peak_values0,image0);

  // Check that each point is above equivalent pixels in image1
  peak_pts.resize(0);
  peak_values.resize(0);
  vimt_transform_2d w2i1  = image1.world2im();
  unsigned ni2 = image1.image().ni()-2;
  unsigned nj2 = image1.image().nj()-2;
  vcl_ptrdiff_t istep = image1.image().istep(), jstep=image1.image().jstep();
  for (unsigned i=0;i<peak_pts0.size();++i)
  {
    if (peak_values0[i]>threshold)
    {
      vgl_point_2d<double> p1 = w2i1(peak_pts0[i]);
      if (p1.x()>0.5 && p1.y()>0.5 && p1.x()<ni2 && p1.y()<nj2)
      {
        const T* pixel=&image1.image()(int(p1.x()+0.5),int(p1.y()+0.5));
        if (ipts_is_above_3x3(peak_values0[i],pixel,istep,jstep))
        {
          peak_pts.push_back(peak_pts0[i]);
          peak_values.push_back(peak_values0[i]);
        }
      }
    }
  }
}

//: Find local maxima in image0 and check against in image1
//  Returned points will be above all their neighbours and
//  those in nearby positions in image1
template<class T>
inline void ipts_scale_space_peaks_2d(vcl_vector<vgl_point_3d<double> >& peak_pts,
                               const vimt_image& image0,
                               const vimt_image& image1,
                               T threshold)
{
  const vimt_image_2d_of<T>& im0 =
                dynamic_cast<const vimt_image_2d_of<T>&>(image0);
  const vimt_image_2d_of<T>& im1 =
                dynamic_cast<const vimt_image_2d_of<T>&>(image1);
  vcl_vector<vgl_point_2d<double> > peak_pts0;
  vcl_vector<T> peak_values0;

  ipts_scale_space_peaks_2d(peak_pts0,peak_values0,im0,im1,threshold);

  // Estimate scaling factor between images (assumed isotropic)
  vgl_vector_2d<double> dx(1,0),dw;
  dw = im0.world2im().delta(vgl_point_2d<double>(0,0),dx);
  double scale = 1.0/vcl_sqrt(dw.x()*dw.x()+dw.y()*dw.y());

  for (unsigned i=0;i<peak_pts0.size();++i)
  {
    vgl_point_3d<double> p(peak_pts0[i].x(),peak_pts0[i].y(),scale);
    peak_pts.push_back(p);
  }
}

//: Find local maxima in position and scale above a threshold
//  Points returned in a 3D point, given world coords + scale value
//  Note that image_pyr is assumed to contain images of type
//  vimt_image_2d_of<T> - threshold indicates the typing.
template<class T>
inline void ipts_scale_space_peaks_2d(vcl_vector<vgl_point_3d<double> >& peak_pts,
                               const vimt_image_pyramid& image_pyr,
                               T threshold)
{
  peak_pts.resize(0);

  // Look for peaks at the finest scale (comparing to those at the scale above)
  if (image_pyr.n_levels()>1)
    ipts_scale_space_peaks_2d(peak_pts,image_pyr(0),image_pyr(1),threshold);

  for (int L=image_pyr.lo()+1;L<image_pyr.hi();++L)
  {
    const vimt_image_2d_of<T>& im_below =
                dynamic_cast<const vimt_image_2d_of<T>&>(image_pyr(L-1));
    const vimt_image_2d_of<T>& image =
                dynamic_cast<const vimt_image_2d_of<T>&>(image_pyr(L));
    const vimt_image_2d_of<T>& im_above =
                dynamic_cast<const vimt_image_2d_of<T>&>(image_pyr(L+1));

    ipts_scale_space_peaks_2d(peak_pts,im_below,image,im_above,threshold,false);
  }
}

#endif // ipts_scale_space_peaks_h_
