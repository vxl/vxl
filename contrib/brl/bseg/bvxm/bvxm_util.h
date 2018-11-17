#ifndef bvxm_util_h_
#define bvxm_util_h_
//:
// \file
// \brief Various utility methods and classes.
// \author Daniel Crispell (dec@lems.brown.edu)
// \date Jan 22, 2008
//
// \verbatim
//  Modifications
//   07 Mar.2008 - Ozge C. Ozcanli - Added support for VIL_PIXEL_FORMAT_RGB_FLOAT and VIL_PIXEL_FORMAT_FLOAT to the img_to_slab method
//                                   Assumes the float image is already scaled to [0,1] range
//   Aug. 20, 2008 - Ibrahim Eden - Added the method static vil_image_view<float> multiply_image_with_gaussian_kernel(vil_image_view<float> img, double gaussian_sigma);
//   Sept 03, 2008 - Ibrahim Eden - Added the method static vil_image_view<vxl_byte> detect_edges ...
//   Sept 11, 2008 - Ibrahim Eden - Added the method static void edge_distance_transform ...
//   Dec. 12, 2008 - Ozge C. Ozcanli - Added the method static void warp_slab_nearest_neighbor ...
//   Febr 12, 2009 - Ibrahim Eden - Added the method static void convert_edge_statistics_to_probability ...
//   July 14, 2009 - Ibrahim Eden - Moved edge related functions to bvxm_edge_util
// \endverbatim

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "grid/bvxm_voxel_slab.h"
#include "bvxm_world_params.h"

#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_vector_fixed.h>

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_save.h> // for debug saving

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

class bvxm_util
{
 public:

  static bool read_cameras(const std::string& filename,
                           std::vector<vnl_double_3x3> &Ks,
                           std::vector<vnl_double_3x3> &Rs,
                           std::vector<vnl_double_3x1> &Ts);

  static bool write_cameras(const std::string& filename,
                            std::vector<vnl_double_3x3> &Ks,
                            std::vector<vnl_double_3x3> &Rs,
                            std::vector<vnl_double_3x1> &Ts);

  static void logical_and(bvxm_voxel_slab<bool> const& s1,
                          bvxm_voxel_slab<bool> const& s2,
                          bvxm_voxel_slab<bool> &result);

  template <class T, class M>
  static void warp_slab_bilinear(bvxm_voxel_slab<M> const& slab_in,
                                 vgl_h_matrix_2d<double> invH,
                                 bvxm_voxel_slab<T> &slab_out);

  template <class T>
  static void warp_slab_nearest_neighbor(bvxm_voxel_slab<T> const& slab_in,
                                         vgl_h_matrix_2d<double> invH,
                                         bvxm_voxel_slab<T> &slab_out);

  template <class T, unsigned N>
  static bool img_to_slab(vil_image_view_base_sptr image,
                          bvxm_voxel_slab<vnl_vector_fixed<T,N> > &slab);

  template <class T>
  static bool img_to_slab(vil_image_view_base_sptr image,
                          bvxm_voxel_slab<T> &slab);

  template <class T, unsigned N>
  static bool slab_to_img(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const &slab,
                          vil_image_view_base_sptr image);

  template <class T>
  static bool slab_to_img(bvxm_voxel_slab<T> const& slab,
                          vil_image_view_base_sptr image);

  template <class T, class M>
  static void multiply_slabs(bvxm_voxel_slab<M> const& s1,
                             bvxm_voxel_slab<T> const& s2,
                             bvxm_voxel_slab<T> &product);

  template <class T>
  static void add_slabs(bvxm_voxel_slab<T> const& s1,
                        bvxm_voxel_slab<T> const& s2,
                        bvxm_voxel_slab<T> &sum);

  template <class T>
  static void threshold_slab_above(bvxm_voxel_slab<T> const& slab,
                                   T const& thresh,
                                   bvxm_voxel_slab<bool> &mask);

  template <class T>
  static void smooth_gaussian(bvxm_voxel_slab<T> &slab,
                              float stdx, float stdy);

  template <class T>
  static void write_slab_as_image(bvxm_voxel_slab<T> const& slab_in,std::string filename);

  template <class T, unsigned N>
  static void write_slab_as_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const& slab_in,std::string filename);

  template <class T>
  static T sum_slab(bvxm_voxel_slab<T> const& slab);

  template <class T>
  static bool generate_test_boxes(T box_min_x, T box_min_y, T box_min_z,
                                  T box_dim_x, T box_dim_y, T box_dim_z,
                                  T world_dim_x, T world_dim_y, T world_dim_z,
                                  std::vector<vgl_box_3d<T> >& boxes, bool gen_2box = true);

  template <class T>
  static std::vector<vgl_point_3d<T> > corners_of_box_3d(vgl_box_3d<T> box);

  // finds out if a given box intersect the polygon
  template <class T>
  static bool intersection(vgl_box_3d<T> const& b, std::vector<vgl_point_3d<T> > const& poly)
  { return vgl_intersection(b,poly).size() > 0; }

  static vil_image_view_base_sptr downsample_image_by_two(const vil_image_view_base_sptr& img);

  static vpgl_camera_double_sptr downsample_camera(const vpgl_camera_double_sptr& camera,unsigned int scale);

  static int convert_uncertainty_from_meters_to_pixels(float uncertainty, const vpgl_lvcs_sptr& lvcs, const vpgl_camera_double_sptr& camera);

  static vpgl_camera_double_sptr downsample_persp_camera(const vpgl_camera_double_sptr& camera,unsigned int scale);

 protected:

  static void bilinear_weights(const vgl_h_matrix_2d<double>& invH,
                               unsigned nx_out,
                               unsigned ny_out,
                               vnl_matrix<unsigned> &xvals,
                               vnl_matrix<unsigned> &yvals,
                               vnl_matrix<float> &weights);
};

template <class T, class M>
void bvxm_util::warp_slab_bilinear(bvxm_voxel_slab<M> const& slab_in,
                                   vgl_h_matrix_2d<double> invH, bvxm_voxel_slab<T> &slab_out)
{
  // smoothing radius of filter
  // TODO: is gaussian convolution with std = projected_size the right amount to get us to Nyquist res?
  float xstd = 0.0f, ystd = 0.0f;

  vnl_matrix_fixed<double,3,3> Hd = invH.get_matrix();
#if 0 // normalize homogeneous coordinates
  // test if slab_in's projection is higher resolution than slab out.
  // if so, we need to smooth slab_in
  // choose a pixel near the center of slab_out
  vnl_vector_fixed<double,3> test_pix0(slab_out.nx()/2.0, slab_out.ny()/2.0, 1);
  vnl_vector_fixed<double,3> test_pix1(test_pix0[0]+1,test_pix0[1]+1,1);
  vnl_vector_fixed<double,3> test_pix_out0 = Hd*test_pix0;
  vnl_vector_fixed<double,3> test_pix_out1 = Hd*test_pix1;

  float max_projection_size = 20.0f;

  // if xsize is too big something is wrong - probably an invalid homography
  double xsize = std::fabs((test_pix_out1[0] / test_pix_out1[2]) - (test_pix_out0[0] / test_pix_out0[2]));
  double ysize = std::fabs((test_pix_out1[1] / test_pix_out1[2]) - (test_pix_out0[1] / test_pix_out0[2]));
  if ( (xsize > 2.0) && (xsize < max_projection_size) )
    xstd = (float)xsize/2.0f;
  if ( (ysize > 2.0) && (ysize < max_projection_size) )
    ystd = (float)ysize/2.0f;
  std::cout << "xsize = " << xsize << " ysize = " << ysize << std::endl;
#endif // 0

  bvxm_voxel_slab<M> slab_in_smooth;
  slab_in_smooth.deep_copy(slab_in);
  smooth_gaussian(slab_in_smooth, xstd, ystd);

  // perform bilinear interpolation.
  vnl_matrix_fixed<float,3,3> H;
  // convert H to a float matrix
  vnl_matrix_fixed<float,3,3>::iterator Hit = H.begin();
  vnl_matrix_fixed<double,3,3>::iterator Hdit = Hd.begin();
  for (; Hit != H.end(); ++Hit, ++Hdit)
    *Hit = (float)(*Hdit);

  slab_out.fill(T(0.0)); // this should work whether T is a vector_fixed or a scalar

  typename bvxm_voxel_slab<T>::iterator out_it = slab_out.begin();

  // if z > 1, it would be more efficient to put the z loop as the inner-most.
  // z will probably be 1 most of the time though, so leave it here for now.
  for (unsigned z=0; z<slab_out.nz(); ++z)
  {
    for (unsigned y=0; y<slab_out.ny(); ++y)
    {
      for (unsigned x=0; x<slab_out.nx(); ++x, ++out_it)
      {
        vnl_vector_fixed<float,3> pix_in_homg = H*vnl_vector_fixed<float,3>((float)x,(float)y,1.0f);
        // normalize homogeneous coordinate

        float pix_in_x = pix_in_homg[0] / pix_in_homg[2];
        float pix_in_y = pix_in_homg[1] / pix_in_homg[2];
        // calculate weights and pixel values
        unsigned x0 = (unsigned)std::floor(pix_in_x);
        unsigned x1 = (unsigned)std::ceil(pix_in_x);
        float x0_weight = (float)x1 - pix_in_x;
        float x1_weight = 1.0f - (float)x0_weight;
        unsigned y0 = (unsigned)std::floor(pix_in_y);
        unsigned y1 = (unsigned)std::ceil(pix_in_y);
        float y0_weight = (float)y1 - pix_in_y;
        float y1_weight = 1.0f - (float)y0_weight;
        vnl_vector_fixed<unsigned,4>xvals(x0,x0,x1,x1);
        vnl_vector_fixed<unsigned,4>yvals(y0,y1,y0,y1);
        vnl_vector_fixed<float,4> weights(x0_weight*y0_weight,
                                          x0_weight*y1_weight,
                                          x1_weight*y0_weight,
                                          x1_weight*y1_weight);

        for (unsigned i=0; i<4; ++i) {
          // check if input pixel is inbounds
          if (xvals[i] < slab_in_smooth.nx() &&
              yvals[i] < slab_in_smooth.ny()) {
            // pixel is good
            (*out_it) += slab_in_smooth(xvals[i],yvals[i],z)*weights[i];
          }
        }
      } //x
    } // y
  } // z
  return;
}

template <class T>
void bvxm_util::warp_slab_nearest_neighbor(bvxm_voxel_slab<T> const& slab_in,
                                           vgl_h_matrix_2d<double> invH, bvxm_voxel_slab<T> &slab_out)
{
  vnl_matrix_fixed<double,3,3> Hd = invH.get_matrix();
  vnl_matrix_fixed<float,3,3> H;
  // convert H to a float matrix
  vnl_matrix_fixed<float,3,3>::iterator Hit = H.begin();
  vnl_matrix_fixed<double,3,3>::iterator Hdit = Hd.begin();
  for (; Hit != H.end(); ++Hit, ++Hdit)
    *Hit = (float)(*Hdit);
  //JLM DEBUG
#if 0
  vnl_vector_fixed<float, 3> h00(0.0f,0.0f,1.0f), h01(0.0f,slab_out.ny(),1.0f);
  vnl_vector_fixed<float, 3> h10(slab_out.nx(),0.0f, 1.0f),
    h11(slab_out.nx(),slab_out.ny(), 1.0f);
  vnl_vector_fixed<float, 3> th00=H*h00, th01=H*h01, th10=H*h10, th11=H*h11;
  float x00 = th00[0]/th00[2], y00 = th00[1]/th00[2];
  float x01 = th01[0]/th01[2], y01 = th01[1]/th01[2];
  float x10 = th10[0]/th10[2], y10 = th10[1]/th10[2];
  float x11 = th11[0]/th11[2], y11 = th11[1]/th11[2];
  std::cout << "Warp slab\n"
           << '(' << 0.0 << ' ' << 0.0 << ")->(" << x00 << ' ' << y00 << ")\n"
           << '(' << 0.0 << ' ' << slab_out.ny() << ")->(" << x01 << ' ' << y01 << ")\n"
           << '(' << slab_out.nx() << ' ' << 0.0 << ")->(" << x10 << ' ' << y10 << ")\n"
           << '(' << slab_out.nx() << ' ' << slab_out.ny() << ")->(" << x11 << ' ' << y11 << ")\n";
#endif

  //slab_out.fill(T(0.0)); // this should work whether T is a vector_fixed or a scalar

  typename bvxm_voxel_slab<T>::iterator out_it = slab_out.begin();

  // if z > 1, it would be more efficient to put the z loop as the inner-most.
  // z will probably be 1 most of the time though, so leave it here for now.
  for (unsigned z=0; z<slab_out.nz(); ++z)
  {
    for (unsigned y=0; y<slab_out.ny(); ++y)
    {
      for (unsigned x=0; x<slab_out.nx(); ++x, ++out_it)
      {
        vnl_vector_fixed<float,3> pix_in_homg = H*vnl_vector_fixed<float,3>((float)x,(float)y,1.0f);
        // normalize homogeneous coordinate
        float pix_in_x = pix_in_homg[0] / pix_in_homg[2];
        float pix_in_y = pix_in_homg[1] / pix_in_homg[2];
        // cast to the nearest integer value
        int x0 = (int)std::floor(pix_in_x+0.5f);
        int y0 = (int)std::floor(pix_in_y+0.5f);
        if (x0 >= 0 && y0 >= 0 && x0 < (int)slab_in.nx() && y0 < (int)slab_in.ny()) {
          // pixel is good
          (*out_it) = slab_in(x0,y0,z);
        }
      } //x
    } // y
  } // z
  return;
}

//: img_to_slab for a multi-band (eg color) image.
//  if input image is a float image, assumes that it is already scaled to [0,1] range
template <class T, unsigned N>
bool bvxm_util::img_to_slab(vil_image_view_base_sptr const image, bvxm_voxel_slab<vnl_vector_fixed<T,N> > &slab)
{
  // check slab is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) ) {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues.
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr())) {
      if (img_view->nplanes() == N) {
        std::vector<vil_image_view<unsigned char>::const_iterator> img_its;
        for (unsigned p=0; p<N; ++p) {
          vil_image_view<unsigned char>::const_iterator plane_it = img_view->begin() + (p*img_view->planestep());
          img_its.push_back(plane_it);
        }
        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it) {
          for (unsigned p=0; p<N; ++p) {
            (*slab_it)[p] = (T)(*(img_its[p]) / 255.0);
            img_its[p] += img_view->istep();
          }
        }
      }
      else {
        std::cerr << "error: img_to_slab (multi-dimensional): nplanes = " << img_view->nplanes() <<", but N = " << N << '\n';
        return false;
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
  }
  // take care of pixel format issues.
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE)
  {
    if (vil_image_view<vil_rgb<unsigned char> > *img_view = dynamic_cast<vil_image_view< vil_rgb<unsigned char> >*>(image.ptr())) {
      vil_image_view<vxl_byte> plane_view = vil_view_as_planes(*img_view);

      if (img_view->nplanes() == 1) {
        std::vector<vil_image_view<unsigned char>::const_iterator> img_its;
        for (unsigned p=0; p<N; ++p) {
          vil_image_view<unsigned char>::const_iterator plane_it = plane_view.begin() + (p*plane_view.planestep());
          img_its.push_back(plane_it);
        }
        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it) {
          for (unsigned p=0; p<N; ++p) {
            (*slab_it)[p] = (T)(*(img_its[p]) / 255.0);
            img_its[p] += plane_view.istep();
          }
        }
      }
      else {
        std::cerr << "error: img_to_slab (multi-dimensional): nplanes = " << img_view->nplanes() <<", but N = 1\n";
        return false;
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>* >(image.ptr())) {
      if (img_view->nplanes() == N) {
        std::vector<vil_image_view<float>::const_iterator> img_its;
        for (unsigned p=0; p<N; ++p) {
          vil_image_view<float>::const_iterator plane_it = img_view->begin() + (p*img_view->planestep());
          img_its.push_back(plane_it);
        }
        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it) {
          for (unsigned p=0; p<N; ++p) {
            (*slab_it)[p] = (T)(*(img_its[p]));  // no divide by 255, we copy the image to the slab directly, assuming image is already scaled to [0,1] range
          }
        }
      }
      else {
        std::cerr << "error: img_to_slab (multi-dimensional): nplanes = " << img_view->nplanes() <<", but N = " << N << '\n';
        return false;
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_FLOAT)
  {
    if (vil_image_view<vil_rgb<float> > *img_view = dynamic_cast<vil_image_view< vil_rgb<float> >*>(image.ptr()))
    {
      vil_image_view<float> plane_view = vil_view_as_planes(*img_view);

      if (img_view->nplanes() == 1) {
        std::vector<vil_image_view<float>::const_iterator> img_its;
        for (unsigned p=0; p<N; ++p) {
          vil_image_view<float>::const_iterator plane_it = plane_view.begin() + (p*plane_view.planestep());
          img_its.push_back(plane_it);
        }
        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it) {
          for (unsigned p=0; p<N; ++p) {
            (*slab_it)[p] = (T)(*(img_its[p]));  // no divide by 255, we copy the image to the slab directly, assuming image is already scaled to [0,1] range
          }
        }
      }
      else {
        std::cerr << "error: img_to_slab (multi-dimensional): nplanes = " << img_view->nplanes() <<", but N = 1\n";
        return false;
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
  }
  else {
    std::cerr << "img_to_slab(scalar): unsupported pixel type\n";
  }

  return true;
}

//:  if input image is a float image, assumes that it is already scaled to [0,1] range
template <class T>
bool bvxm_util::img_to_slab(vil_image_view_base_sptr const image, bvxm_voxel_slab<T> &slab)
{
  // check slab is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) ) {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  // for now assume 8 bit grayscale
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr())) {
      if (img_view->nplanes() > 1) {
        vil_image_view<vxl_byte> img_view_grey(img_view->ni(),img_view->nj(),1);
        vil_convert_planes_to_grey(*img_view,img_view_grey);
        vil_image_view<unsigned char>::const_iterator img_it = img_view_grey.begin();
        typename bvxm_voxel_slab<T>::iterator slab_it = slab.begin();
        for (; img_it != img_view_grey.end(); ++img_it, ++slab_it) {
          *slab_it = (T)(*img_it / 255.0);
        }
      }
      else {
        vil_image_view<unsigned char>::const_iterator img_it = img_view->begin();
        typename bvxm_voxel_slab<T>::iterator slab_it = slab.begin();
        for (; img_it != img_view->end(); ++img_it, ++slab_it) {
          *slab_it = (T)(*img_it / 255.0);
        }
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    // we may want to convert a color image to grayscale
    if (vil_image_view<vil_rgb<unsigned char> > *img_view_rgb = dynamic_cast<vil_image_view<vil_rgb<unsigned char> >*>(image.ptr())) {
      vil_image_view<float> img_view_grey(img_view_rgb->ni(),img_view_rgb->nj(),1);
      if (img_view_rgb->nplanes() == 1) {
        vil_image_view<vxl_byte> img_view_plane = vil_view_as_planes(*img_view_rgb);
        vil_convert_planes_to_grey(img_view_plane,img_view_grey);
      }
      else {
        std::cerr << "error: pixel format RGB, but nplanes = " << img_view_rgb->nplanes() << '\n';
        return false;
      }
      vil_image_view<float>::const_iterator img_it = img_view_grey.begin();
      typename bvxm_voxel_slab<T>::iterator slab_it = slab.begin();
      for (; img_it != img_view_grey.end(); ++img_it, ++slab_it) {
        *slab_it = (T)(*img_it / 255.0);
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>* >(image.ptr())) {
      vil_image_view<float>::const_iterator img_it = img_view->begin();
      typename bvxm_voxel_slab<T>::iterator slab_it = slab.begin();
      for (; img_it != img_view->end(); ++img_it, ++slab_it) {
        *slab_it = (T)(*img_it); // no divide by 255, we copy the image to the slab directly, assuming image is already scaled to [0,1] range
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
    }
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_FLOAT) {
    if (vil_image_view<vil_rgb<float> > *img_view_rgb = dynamic_cast<vil_image_view<vil_rgb<float> >* >(image.ptr())) {
      vil_image_view<float> img_view_grey(img_view_rgb->ni(),img_view_rgb->nj(),1);
      if (img_view_rgb->nplanes() == 1) {
        vil_image_view<float> img_view_plane = vil_view_as_planes(*img_view_rgb);
        vil_convert_planes_to_grey(img_view_plane,img_view_grey);
      }
      else {
        std::cerr << "error: pixel format RGB, but nplanes = " << img_view_rgb->nplanes() << '\n';
        return false;
      }
      vil_image_view<float>::const_iterator img_it = img_view_grey.begin();
      typename bvxm_voxel_slab<T>::iterator slab_it = slab.begin();
      for (; img_it != img_view_grey.end(); ++img_it, ++slab_it) {
        *slab_it = (T)(*img_it); // no divide by 255, we copy the image to the slab directly, assuming image is already scaled to [0,1] range
      }
    }
    else {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
    }
  }
  else {
    std::cerr << "img_to_slab(scalar): unsupported pixel type\n";
  }

  return true;
}

template <class T, unsigned N>
bool bvxm_util::slab_to_img(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const &slab, vil_image_view_base_sptr image)
{
  // check image is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
  {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  switch (image->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE:
    if (image->nplanes() ==N)
    {
      if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr()))
      {
        std::vector<vil_image_view<unsigned char>::iterator> img_its;
        for (unsigned p=0; p<N; ++p)
        {
          vil_image_view<unsigned char>::iterator plane_it = img_view->begin() + (p*img_view->planestep());
          img_its.push_back(plane_it);
        }

        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it)
        {
          for (unsigned p=0; p<N; ++p)
          {
            *(img_its[p]) = (unsigned char)(((*slab_it)[p] * 255.0) + 0.5);
            ++(img_its[p]);
          }
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
        return false;
      }
    }
    else
    {
      std::cerr << "error in slab_to_image: incorrect number of image planes\n";
      return false;
    }
    break;

  case VIL_PIXEL_FORMAT_FLOAT:
    if (image->nplanes() ==N)
    {
      if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(image.ptr()))
      {
        std::vector<vil_image_view<float>::iterator> img_its;
        for (unsigned p=0; p<N; ++p)
        {
          vil_image_view<float>::iterator plane_it = img_view->begin() + (p*img_view->planestep());
          img_its.push_back(plane_it);
        }

        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it)
        {
          for (unsigned p=0; p<N; ++p)
          {
            *(img_its[p]) = (float)(((*slab_it)[p] * 255.0) + 0.5);
            ++(img_its[p]);
          }
        }
      }
      else
      {
        std::cerr << "error in slab_to_image: failed to cast image_view_base to image_view\n";
        return false;
      }
    }
    else
    {
      std::cerr << "error in slab_to_image: incorrect number of image planes\n";
      return false;
    }
    break;

  case VIL_PIXEL_FORMAT_RGB_BYTE:

    if (vil_image_view<vil_rgb<unsigned char> > *img_view = dynamic_cast<vil_image_view< vil_rgb<unsigned char> >*>(image.ptr()))
    {
      vil_image_view<vxl_byte> plane_view = vil_view_as_planes(*img_view);

      if (img_view->nplanes() == 1)
      {
        std::vector<vil_image_view<unsigned char>::iterator> img_its;
        for (unsigned p=0; p<N; ++p)
        {
          vil_image_view<unsigned char>::iterator plane_it = plane_view.begin() + (p*plane_view.planestep());
          img_its.push_back(plane_it);
        }
        typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab.begin();
        for (; slab_it != slab.end(); ++slab_it)
        {
          for (unsigned p=0; p<N; ++p)
          {
            *(img_its[p]) = (unsigned char)(((*slab_it)[p] * 255.0) + 0.5);
            ++(img_its[p]);
          }
        }
      }
      else
      {
        std::cerr << "error: slab_to_img (multi-dimensional): nplanes = " << img_view->nplanes() <<", but N = 1\n";
        return false;
      }
    }
    else
    {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }

    break;

  default:
    std::cerr << "img_to_slab: unsupported pixel type\n";
    return false;
    break;
  }

  return true;
}

template <class T>
bool bvxm_util::slab_to_img(bvxm_voxel_slab<T> const& slab, vil_image_view_base_sptr image)
{
  // check image is preallocated to correct size
  if ( (slab.nx() != image->ni()) || (slab.ny() != image->nj()) )
  {
    std::cerr << "error: slab and image are different sizes.\n";
    return false;
  }

  // take care of pixel format issues. might want to specialize this function for rgb, etc
  switch (image->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE:
    if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr()))
    {
      vil_image_view<unsigned char>::iterator img_it = img_view->begin();
      typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
      for (; img_it != img_view->end(); ++img_it, ++slab_it) {
        *img_it = (unsigned char)((*slab_it * 255.0) + 0.5);
      }
    }
    else
    {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
    break;

  case VIL_PIXEL_FORMAT_FLOAT:
    if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(image.ptr()))
    {
      vil_image_view<float>::iterator img_it = img_view->begin();
      typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
      for (; img_it != img_view->end(); ++img_it, ++slab_it) {
        *img_it = (float)(*slab_it);
      }
    }
    else
    {
      std::cerr << "error: failed to cast image_view_base to image_view\n";
      return false;
    }
    break;

  default:
    std::cerr << "img_to_slab: unsupported pixel type\n";
    return false;
    break;
  }

  return true;
}

template <class T>
T bvxm_util::sum_slab(bvxm_voxel_slab<T> const& slab)
{
  T sum = 0;
  typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
  for (; slab_it != slab.end(); ++slab_it) {
    sum += *slab_it;
  }
  return sum;
}

template <class T>
void bvxm_util::add_slabs(bvxm_voxel_slab<T> const& s1, bvxm_voxel_slab<T> const& s2, bvxm_voxel_slab<T> &sum)
{
  // check sizes
  if ( (sum.nx() != s1.nx()) || (sum.nx() != s2.nx()) ||
       (sum.ny() != s1.ny()) || (sum.ny() != s2.ny()) ||
       (sum.nz() != s1.nz()) || (sum.nz() != s2.nz()) ) {
    std::cerr << "error: sizes of slabs to multiply do not match.\n";
    return;
  }

  typename bvxm_voxel_slab<T>::const_iterator s1_it = s1.begin(), s2_it = s2.begin();
  typename bvxm_voxel_slab<T>::iterator sum_it = sum.begin();
  for (; sum_it != sum.end(); ++s1_it, ++s2_it, ++sum_it) {
    *sum_it = *s1_it + *s2_it;
  }
  return;
}

template <class T, class M>
void bvxm_util::multiply_slabs(bvxm_voxel_slab<M> const& s1, bvxm_voxel_slab<T> const& s2, bvxm_voxel_slab<T> &product)
{
  // check sizes
  if ( (product.nx() != s1.nx()) || (product.nx() != s2.nx()) ||
       (product.ny() != s1.ny()) || (product.ny() != s2.ny()) ||
       (product.nz() != s1.nz()) || (product.nz() != s2.nz()) ) {
    std::cerr << "error: sizes of slabs to multiply do not match.\n";
    return;
  }

  typename bvxm_voxel_slab<M>::const_iterator s1_it = s1.begin();
  typename bvxm_voxel_slab<T>::const_iterator s2_it = s2.begin();
  typename bvxm_voxel_slab<T>::iterator prod_it = product.begin();
  for (; prod_it != product.end(); ++s1_it, ++s2_it, ++prod_it) {
    *prod_it = *s1_it * *s2_it;
  }
  return;
}

template <class T>
void bvxm_util::threshold_slab_above(bvxm_voxel_slab<T> const& slab, T const& thresh, bvxm_voxel_slab<bool> &mask)
{
  // check sizes
  if ( (mask.nx() != slab.nx()) || (mask.ny() != slab.ny()) ) {
    std::cerr << "error: sizes of input and output slabs do not match.\n";
    return;
  }

  typename bvxm_voxel_slab<T>::const_iterator slab_it = slab.begin();
  typename bvxm_voxel_slab<bool>::iterator mask_it = mask.begin();
  for (; slab_it != slab.end(); ++slab_it, ++mask_it) {
    *mask_it = (*slab_it >= thresh);
  }
  return;
}

template <class T>
void bvxm_util::smooth_gaussian(bvxm_voxel_slab<T> &slab, float stdx, float stdy)
{
  if ( (stdx < 0) || (stdy < 0) ) {
    std::cerr << "error: smooth_gaussian called with negative std. deviation!\n"
             << "stdx = " << stdx << "  stdy = " << stdy << std::endl;
    return;
  }

  if ( (stdx == 0) && (stdy == 0) )
    return;

  // create 1d kernels
  unsigned kernel_radius_x = (unsigned)(stdx*3);
  unsigned kernel_size_x = 2*kernel_radius_x + 1;
  vnl_vector<float> kernel_1dx(kernel_size_x);

  // fill in kernel
  for (unsigned i=0; i<kernel_size_x; ++i) {
    kernel_1dx[i] = (float)(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/stdx) * std::exp(-(((i-kernel_radius_x)*(i-kernel_radius_x))/(2.0*stdx*stdx))));
  }
  // normalize kernel in case taps don't sum to exactly one
  kernel_1dx = kernel_1dx / kernel_1dx.sum();

  unsigned kernel_radius_y = (unsigned)(stdy*3);
  unsigned kernel_size_y = 2*kernel_radius_y + 1;
  vnl_vector<float> kernel_1dy(kernel_size_y);
  // fill in kernel
  for (unsigned i=0; i<kernel_size_y; ++i) {
    kernel_1dy[i] = (float)(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/stdy) * std::exp(-(((i-kernel_radius_y)*(i-kernel_radius_y))/(2.0*stdy*stdy))));
  }
  // normalize kernel in case taps don't sum to exactly one
  kernel_1dy = kernel_1dy / kernel_1dy.sum();

  bvxm_voxel_slab<T> slab_work(slab.nx(),slab.ny(),slab.nz());
  if (stdx > 0)
  {
    // create temporary slab
    // convolve rows
    for (unsigned y=0; y<slab.ny(); ++y)
    {
      for (unsigned x=0; x<=slab.nx() - kernel_size_x; ++x) {
        T sum = slab(x,y) * kernel_1dx[0];
        for (unsigned k=1; k<kernel_size_x; ++k) {
          sum += slab(x+k,y) * kernel_1dx[k];
        }
        slab_work(x+kernel_radius_x,y) = sum;
      }
      // left edge
      for (unsigned x=0; x<kernel_radius_x; ++x) {
        T sum(0.0);
        for (unsigned k=kernel_radius_x - x; k<kernel_size_x; ++k) {
          sum += slab(x+k-kernel_radius_x,y) * kernel_1dx[k];
        }
        slab_work(x,y) = sum;
      }
      // right edge
      for (unsigned x=slab.nx()-kernel_radius_x; x<slab.nx(); ++x) {
        T sum(0.0);
        for (unsigned k=0; k<(slab.nx() - x + kernel_radius_x); ++k) {
          sum += slab(x+k-kernel_radius_x,y) * kernel_1dx[k];
        }
        slab_work(x,y) = sum;
      }
    }
  }
  else {
    // stdx was zero, just use original slab.
    slab_work.deep_copy(slab);
  }

  if (stdy > 0.0)
  {
    // convolve columns
    for (unsigned x=0; x<slab.nx(); ++x) {
      for (unsigned y=0; y<=slab.ny() - kernel_size_y; ++y) {
        T sum = slab_work(x,y) * kernel_1dy[0];
        for (unsigned k=1; k<kernel_size_y; ++k) {
          sum += slab_work(x,y+k) * kernel_1dy[k];
        }
        slab(x,y+kernel_radius_y) = sum;
      }
      // top edge
      for (unsigned y=0; y<kernel_radius_y; ++y) {
        T sum(0.0);
        for (unsigned k=kernel_radius_y - y; k<kernel_size_y; ++k) {
          sum += slab_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
        }
        slab(x,y) = sum;
      }
      // bottom edge
      for (unsigned y=slab.ny()-kernel_radius_y; y<slab.ny(); ++y) {
        T sum(0.0);
        for (unsigned k=0; k<(slab.ny() - y + kernel_radius_y); ++k) {
          sum += slab_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
        }
        slab(x,y) = sum;
      }
    }
  }
  else {
    slab.deep_copy(slab_work);
  }
  return;
}

// used for debugging
template <class T>
void bvxm_util::write_slab_as_image(bvxm_voxel_slab<T> const& slab_in,std::string filename)
{
  vil_image_view<T> img(slab_in.nx(),slab_in.ny(),1);
  typename vil_image_view<T>::iterator img_it = img.begin();
  typename bvxm_voxel_slab<T>::const_iterator slab_it = slab_in.begin();
  for (; img_it != img.end(); ++img_it, ++slab_it) {
    *img_it = (*slab_it) + 0;
  }
  vil_save(img,filename.c_str());

  return;
}

// used for debugging
template <class T, unsigned N>
void bvxm_util::write_slab_as_image(bvxm_voxel_slab<vnl_vector_fixed<T,N> > const& slab_in,std::string filename)
{
  vil_image_view<T> img(slab_in.nx(),slab_in.ny(),N);
  std::vector<typename vil_image_view<T>::iterator> img_its;
  for (unsigned p=0; p < N; ++p) {
    typename vil_image_view<T>::iterator plane_it = img.begin() + (p*img.planestep());
    img_its.push_back(plane_it);
  }
  typename bvxm_voxel_slab<vnl_vector_fixed<T,N> >::const_iterator slab_it = slab_in.begin();
  for (; slab_it != slab_in.end(); ++slab_it) {
    for (unsigned p=0; p<N; ++p) {
      *(img_its[p]) = (*slab_it)[p];
      ++(img_its[p]);
    }
  }
  vil_save(img,filename.c_str());

  return;
}

template <class T>
bool bvxm_util::generate_test_boxes(T box_min_x, T box_min_y, T box_min_z,
                                    T box_dim_x, T box_dim_y, T box_dim_z,
                                    T world_dim_x, T world_dim_y, T world_dim_z,
                                    std::vector<vgl_box_3d<T> >& boxes, bool gen_2box)
{
  // create the big box at the bottom
  T max_x = box_min_x + box_dim_x;
  T max_y = box_min_y + box_dim_y;
  T max_z = box_min_z + box_dim_z;
  if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z)) {
    std::cerr << "generate_boxes() -- the box is out of world boundaries!\n";
    return false;
  }

  vgl_box_3d<T> box(box_min_x, box_min_y, box_min_z, max_x, max_y, max_z);
  boxes.push_back(box);

  if (gen_2box) {
    // create the top box
    vgl_point_3d<T> centroid = box.centroid();
    // make the top box 2/3 of the size of the previous one
    T dimx = (box.max_x() - box.min_x())/2;
    T dimy = (box.max_y() - box.min_y())/2;
    T dimz = (box.max_z() - box.min_z())/2;
    centroid.set(centroid.x(), centroid.y(), box.max_z() + dimz/2.0);
    vgl_box_3d<T> top_box = vgl_box_3d<T> (centroid, dimx, dimy, dimz, vgl_box_3d<T>::centre);
    // translate it a bit
    vgl_point_3d<T> top_centroid = top_box.centroid();
    top_box.set_centroid(vgl_point_3d<T>(top_centroid.x()+dimx/3., top_centroid.y()+dimx/3., top_centroid.z()));
    // check if the box in the world completely
    max_x = top_box.max_x();
    max_y = top_box.max_y();
    max_z = top_box.max_z();
    // stop if the new box is getting out of the boundaries
    if ((max_x > world_dim_x) || (max_y > world_dim_y) || (max_z > world_dim_z))
      return false;
    boxes.push_back(top_box);
  }
  else {
    vgl_box_3d<T> top_box = vgl_box_3d<T>();
    boxes.push_back(top_box);
  }

  return true;
}

template <class T>
std::vector<vgl_point_3d<T> >
bvxm_util::corners_of_box_3d(vgl_box_3d<T> box)
{
  std::vector<vgl_point_3d<T> > corners;

  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y(), box.max_z()));
  corners.push_back(vgl_point_3d<T> (box.min_x()+box.width(), box.min_y(), box.max_z()));
  corners.push_back(box.max_point());
  corners.push_back(vgl_point_3d<T> (box.min_x(), box.min_y()+box.height(), box.max_z()));
  return corners;
}

#endif // bvxm_util_h_
