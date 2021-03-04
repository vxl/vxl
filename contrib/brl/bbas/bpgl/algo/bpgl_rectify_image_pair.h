// This is brl/bbas/bpgl/algo/bpgl_rectify_image_pair.h
#ifndef bpgl_rectify_image_pair_h_
#define bpgl_rectify_image_pair_h_
//:
// \file/
// \brief rectify two images into a stereo pair given cameras for each
// \author J. L. Mundy
// \date February 15, 2020
// \modification Noah Johnson January 14, 2021
//
// a generalization of bpgl_rectify_affine_image_pair
//
#include <iostream>
#include <utility>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include "rectify_params.h"


//
// Rectification is split up into two parts, both of which are exposed as public methods.
// compute_rectification() computes rectification homographies for a camera pair.
// warp_image() warps an image using a homography.
// process() will run both parts sequentially for you.
// The output (rectified_fview0 and rectified_fview1) is a pair of images that
// have common epipolar lines along image rows. The difference in column
// coordinates of corresponding points is minimized.
//
template <class CAMT>
class bpgl_rectify_image_pair
{
 public:

  //: constructor
  bpgl_rectify_image_pair(double min_disparity_z = NAN, size_t n_points = 1000,
                          double upsample_scale = 1.0, float invalid_pixel_val = 0.0f,
                          double min_overlap_fraction = 0.25, int window_padding = 0,
                          size_t kernel_margin = 0)
  {
    this->set_valid_types();

    // if min_disparity_z_ is NAN then 1/2 the midpoint of scene_box z is used
    params_.min_disparity_z_ = min_disparity_z;
    params_.n_points_ = n_points;
    params_.upsample_scale_ = upsample_scale;
    params_.invalid_pixel_val_ = invalid_pixel_val;
    params_.min_overlap_fraction_ = min_overlap_fraction;
    params_.window_padding_ = window_padding;
    params_.kernel_margin_ = kernel_margin;
  }

  //: setters
  void set_params(rectify_params const& params) { params_ = params; }

  void set_images(vil_image_view_base_sptr const& view0,
                  vil_image_view_base_sptr const& view1);

  void set_images(vil_image_view<unsigned char> const& view0,
                  vil_image_view<unsigned char> const& view1)
  {
    this->set_images(vil_new_image_resource_of_view(view0), vil_new_image_resource_of_view(view1));
  }

  void set_images(vil_image_resource_sptr const& resc0,
                  vil_image_resource_sptr const& resc1);

  void set_cameras(CAMT const& cam0, CAMT const& cam1)
  {
    cam0_ = cam0;
    cam1_ = cam1;
  }

  void set_homographies(vnl_matrix_fixed<double, 3, 3>& H0,
                        vnl_matrix_fixed<double, 3, 3>& H1,
                        size_t out_ni, size_t out_nj)
  {
    H0_ = H0;
    H1_ = H1;
    out_ni_ = out_ni;
    out_nj_ = out_nj;
  }

  //: accessors
  const vgl_box_2d<int>& rect_window0() const {return rect_window0_;}
  const vgl_box_2d<int>& rect_window1() const {return rect_window1_;}
  const CAMT& cam0() const {return cam0_;}
  const CAMT& cam1() const {return cam1_;}
  const vil_image_view<float>& input_float_view0() const {return fview0_;}
  const vil_image_view<float>& input_float_view1() const {return fview1_;}
  const vil_image_view<float>& rectified_fview0() const {return rect_fview0_;}
  const vil_image_view<float>& rectified_fview1() const {return rect_fview1_;}
  vnl_matrix_fixed<double, 3, 3> H0() const {return H0_;}
  vnl_matrix_fixed<double, 3, 3> H1() const {return H1_;}
  std::pair<size_t, size_t> rectified_dims() {return std::pair<size_t, size_t>(out_ni_, out_nj_);}

  //: utility methods
  bool load_camera(std::string const& cam_path, CAMT& cam);

  void warp_image(vil_image_view<float> fview,
                  vnl_matrix_fixed<double, 3, 3> const& H,
                  vil_image_view<float>& fwarp,
                  size_t out_ni, size_t out_nj,
                  vgl_box_2d<int> const& rectified_window = vgl_box_2d<int>(),
                  size_t margin = 0);

  CAMT rectify_camera(CAMT cam, vnl_matrix_fixed<double, 3, 3> H);

  vgl_box_2d<int> rectify_window(vgl_box_2d<int> const& window,
                                 vnl_matrix_fixed<double, 3, 3> H,
                                 size_t ni, size_t nj,
                                 int padding = 0);

  // part 1 of rectification
  void compute_rectification(vgl_box_3d<double>const& scene_box,
                             int ni0 = -1, int nj0 = -1,
                             int ni1 = -1, int nj1 = -1);

  // (optional) part 1.5 of rectification
  void rectify_window_pair(vgl_box_2d<int> const& target_window,
                           int min_disparity,
                           int max_disparity);

  // part 2 of rectification
  void warp_image_pair();

  // process method puts the pieces together for you
  // assumes set_images() and set_cameras() have been called
  void process(vgl_box_3d<double> const& scene_box,
               vgl_box_2d<int> const& target_window = vgl_box_2d<int>(),
               int min_disparity = 0, int max_disparity = 0)
  {
    this->compute_rectification(scene_box);
    this->rectify_window_pair(target_window, min_disparity, max_disparity);
    this->warp_image_pair();
  }

  // overloaded process methods allow functional approach
  void process(vil_image_view_base_sptr const& view_sptr0,
               vil_image_view_base_sptr const& view_sptr1,
               CAMT& cam0, CAMT& cam1,
               vgl_box_3d<double>const& scene_box,
               vgl_box_2d<int> const& target_window = vgl_box_2d<int>(),
               int min_disparity = 0, int max_disparity = 0)
  {
    this->set_images(view_sptr0, view_sptr1);
    this->set_cameras(cam0, cam1);
    this->process(scene_box, target_window, min_disparity, max_disparity);
  }
  void process(vil_image_view<unsigned char> const& view0,
               vil_image_view<unsigned char> const& view1,
               CAMT& cam0, CAMT& cam1,
               vgl_box_3d<double>const& scene_box,
               vgl_box_2d<int> const& target_window = vgl_box_2d<int>(),
               int min_disparity = 0, int max_disparity = 0)
  {
    this->set_images(vil_new_image_resource_of_view(view0), vil_new_image_resource_of_view(view1));
    this->set_cameras(cam0, cam1);
    this->process(scene_box, target_window, min_disparity, max_disparity);
  }
  void process(vil_image_resource_sptr const& resc0,
               vil_image_resource_sptr const& resc1,
               CAMT& cam0, CAMT& cam1,
               vgl_box_3d<double>const& scene_box,
               vgl_box_2d<int> const& target_window = vgl_box_2d<int>(),
               int min_disparity = 0, int max_disparity = 0)
  {
    this->set_images(resc0, resc1);
    this->set_cameras(cam0, cam1);
    this->process(scene_box, target_window, min_disparity, max_disparity);
  }

 protected:

  void compute_warp_dimensions_offsets(int ni0, int nj0,
                                       int ni1, int nj1);

 private:

  void set_valid_types();
  bool valid_type(std::string const& type);
  std::vector<std::string> valid_cam_types_;
  double scale_to_input_size(int ni0, int nj0,
                             int ni1, int nj1);
  rectify_params params_;
  vil_image_view<float> fview0_;
  vil_image_view<float> fview1_;
  vil_image_view<float> rect_fview0_;
  vil_image_view<float> rect_fview1_;

  vgl_box_2d<int> rect_window0_;
  vgl_box_2d<int> rect_window1_;

  CAMT cam0_;
  CAMT cam1_;

  // H0, H1 include a translation to keep
  // image coordinates positive definite
  vnl_matrix_fixed<double, 3, 3> H0_;
  vnl_matrix_fixed<double, 3, 3> H1_;

  double du_off_;
  double dv_off_;
  size_t out_ni_;
  size_t out_nj_;
};

#define BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE(T) extern "please include bpgl/bpgl_rectify_image_pair.hxx first"
#endif // bpgl_rectify_image_pair_h_
