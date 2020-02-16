// This is brl/bbas/bpgl/algo/bpgl_rectify_image_pair.h
#ifndef bpgl_rectify_image_pair_h_
#define bpgl_rectify_image_pair_h_
//:
// \file/
// \brief rectify two images into a stero pair given cameras for each
// \author J. L. Mundy
// \date February 15, 2020
//
// a generalization of bpgl_rectify_affine_image_pair
//
#include <iostream>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_box_3d.h>
#include "rectify_params.h"

//
// requires two images and associated affine cameras. Class can load the data from files if needed.
// the output is a pair of images that have common epipolar lines along image rows. The difference in
// column coordinates of corresponding points is minimized.
//
template <class CAMT>
class bpgl_rectify_image_pair
{
 public:
   bpgl_rectify_image_pair() { set_valid_types(); }

  bpgl_rectify_image_pair(vil_image_view_base_sptr const& view0, CAMT const& cam0,
                          vil_image_view_base_sptr const& view1, CAMT const& cam1)
  {
    bool good = this->set_images_and_cams(view0,cam0,view1,cam1);
    set_valid_types();
  }

  bpgl_rectify_image_pair(vil_image_view<unsigned char> const& view0, CAMT const& cam0,
                          vil_image_view<unsigned char> const& view1, CAMT const& cam1)
  {
    bool good = this->set_images_and_cams(view0,cam0,view1,cam1);
    set_valid_types();
  }

  bpgl_rectify_image_pair(vil_image_resource_sptr const& resc0, CAMT const& cam0,
                                 vil_image_resource_sptr const& resc1, CAMT const& cam1)
  {
    bool good = this->set_images_and_cams(resc0,cam0,resc1,cam1);
    set_valid_types();
  }

  //: set parameter values
  void set_param_values(double min_disparity_z = NAN, size_t n_points = 1000,
                        double upsample_scale =1.0, float invalid_pixel_val = 0.0f){
    params_.min_disparity_z_ = min_disparity_z;
    params_.n_points_ = n_points;
    params_.upsample_scale_ = upsample_scale;
    params_.invalid_pixel_val_ = invalid_pixel_val;
  }

  //: set images & cameras
  bool set_images_and_cams(vil_image_view_base_sptr const& view0, CAMT const& cam0,
                           vil_image_view_base_sptr const& view1, CAMT const& cam1);

  bool set_images_and_cams(vil_image_view<unsigned char> const& view0, CAMT const& cam0,
                           vil_image_view<unsigned char> const& view1, CAMT const& cam1)
  {
    return set_images_and_cams(vil_new_image_resource_of_view(view0), cam0, vil_new_image_resource_of_view(view1), cam1);
  }

  bool set_images_and_cams(vil_image_resource_sptr const& resc0, CAMT const& cam0,
                           vil_image_resource_sptr const& resc1, CAMT const& cam1);

  //: accessors
  const CAMT& cam0()const{return cam0_;}
  const CAMT& cam1()const{return cam1_;}
  const CAMT& rect_cam0()const{return rect_cam0_;}
  const CAMT& rect_cam1()const{return rect_cam1_;}
  const vil_image_view<float>& input_float_view0()const{return fview0_;}
  const vil_image_view<float>& input_float_view1()const{return fview1_;}
  const vil_image_view<float>& rectified_fview0() const{return rect_fview0_;}
  const vil_image_view<float>& rectified_fview1() const{return rect_fview1_;}
  vnl_matrix_fixed<double, 3, 3> H0() const {return H0_;}
  vnl_matrix_fixed<double, 3, 3> H1() const {return H1_;}

  //: utility methods
  bool load_camera(std::string const& cam_path, CAMT& cam);
  void set_params(rectify_params const& params) { params_ = params; }

  bool process(vgl_box_3d<double>const& scene_box)
  {
    // if min_disparity_z_ is NAN then 1/2 the midpoint of scene_box z is used
    if(!compute_rectification(scene_box, params_.n_points_, params_.min_disparity_z_))
      return false;
    warp_pair();
    return true;
  }
  //: rectified image dimensions
  void rectified_dims(size_t& width, size_t& height){width = out_ni_; height = out_nj_;}
 protected:

  // protected utility methods
  bool compute_rectification(vgl_box_3d<double> const& scene_box, size_t n_points = 1000, double average_z = NAN);
  void compute_warp_dimensions_offsets();
  void warp_image(vil_image_view<float> fview,
                  vnl_matrix_fixed<double, 3, 3> const& H,
                  vil_image_view<float>& fwarp,
                  size_t out_ni, size_t out_nj);
  void warp_pair();

 private:
  void set_valid_types();
  bool valid_type(std::string const& type);
  std::vector<std::string > valid_cam_types_;
  rectify_params params_;
  double scale_to_input_size();
  vil_image_view<float> fview0_;
  vil_image_view<float> fview1_;
  vil_image_view<float> rect_fview0_;
  vil_image_view<float> rect_fview1_;

  CAMT cam0_;
  CAMT cam1_;
  CAMT rect_cam0_;
  CAMT rect_cam1_;

  // H0, H1 include a translation to keep
  // image coordinates positive definite
  vnl_matrix_fixed<double, 3, 3> H0_;
  vnl_matrix_fixed<double, 3, 3> H1_;

  double du_off_;
  double dv_off_;
  size_t out_ni_;
  size_t out_nj_;
};
#define BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE(T) extern "please include bpgl/bpgl_rectify_image_pair.txx first"
#endif // bpgl_rectify_image_pair_h_
