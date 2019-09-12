// This is brl/bbas/bpgl/algo/bpgl_rectify_affine_image_pair.h
#ifndef bpgl_rectify_affine_image_pair_h_
#define bpgl_rectify_affine_image_pair_h_
//:
// \file/
// \brief rectify two images into a stero pair given affine cameras for each
// \author J. L. Mundy
// \date February 6, 2019
#include <iostream>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vnl/vnl_matrix_fixed.h>

struct rectify_params{
  rectify_params(): min_disparity_z_(NAN), n_points_(1000), upsample_scale_(1.0), invalid_pixel_val_(0.0f) {}

  double min_disparity_z_; // horizontal plane where disparity at each pixel is minimum
  size_t n_points_;            // number of points used to create correspondences
  double upsample_scale_;      // scale factor to upsample rectified images
  float invalid_pixel_val_;
};

//
// requires two images and associated affine cameras. Class can load the data from files if needed.
// the output is a pair of images that have common epipolar lines along image rows. The difference in
// column coordinates of corresponding points is minimized.
//
class bpgl_rectify_affine_image_pair
{
 public:
  bpgl_rectify_affine_image_pair() {}

  bpgl_rectify_affine_image_pair(vil_image_view_base_sptr const& view0, vpgl_affine_camera<double> const& acam0,
                                 vil_image_view_base_sptr const& view1, vpgl_affine_camera<double> const& acam1)
  {
    bool good = this->set_images_and_cams(view0,acam0,view1,acam1);
  }

  bpgl_rectify_affine_image_pair(vil_image_view<unsigned char> const& view0, vpgl_affine_camera<double> const& acam0,
                                 vil_image_view<unsigned char> const& view1, vpgl_affine_camera<double> const& acam1)
  {
    bool good = this->set_images_and_cams(view0,acam0,view1,acam1);
  }

  bpgl_rectify_affine_image_pair(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                                 vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1)
  {
    bool good = this->set_images_and_cams(resc0,acam0,resc1,acam1);
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
  bool set_images_and_cams(vil_image_view_base_sptr const& view0, vpgl_affine_camera<double> const& acam0,
                           vil_image_view_base_sptr const& view1, vpgl_affine_camera<double> const& acam1);

  bool set_images_and_cams(vil_image_view<unsigned char> const& view0, vpgl_affine_camera<double> const& acam0,
                           vil_image_view<unsigned char> const& view1, vpgl_affine_camera<double> const& acam1)
  {
    return set_images_and_cams(vil_new_image_resource_of_view(view0), acam0, vil_new_image_resource_of_view(view1), acam1);
  }

  bool set_images_and_cams(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                           vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1);

  //: accessors
  const vpgl_affine_camera<double>& acam0()const{return acam0_;}
  const vpgl_affine_camera<double>& acam1()const{return acam1_;}
  const vpgl_affine_camera<double>& rect_acam0()const{return rect_acam0_;}
  const vpgl_affine_camera<double>& rect_acam1()const{return rect_acam1_;}
  const vil_image_view<float>& input_float_view0()const{return fview0_;}
  const vil_image_view<float>& input_float_view1()const{return fview1_;}
  const vil_image_view<float>& rectified_fview0() const{return rect_fview0_;}
  const vil_image_view<float>& rectified_fview1() const{return rect_fview1_;}
  vnl_matrix_fixed<double, 3, 3> H0() const {return H0_;}
  vnl_matrix_fixed<double, 3, 3> H1() const {return H1_;}

  //: utility methods
  static bool load_affine_camera(std::string const& cam_path, vpgl_affine_camera<double>& acam);
  void set_params(rectify_params const& params) { params_ = params; }

  bool process(vgl_box_3d<double>const& scene_box)
  {
    // if min_disparity_z_ is NAN then 1/2 the midpoint of scene_box z is used
    if(!compute_rectification(scene_box, params_.n_points_, params_.min_disparity_z_))
      return false;
    warp_pair();
    return true;
  }

  // for debug purposes
  bool load_images_and_cams(std::string const& image0_path, std::string const& cam0_path,
                            std::string const& image1_path, std::string const& cam1_path);

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
  rectify_params params_;
  vil_image_view<float> fview0_;
  vil_image_view<float> fview1_;
  vil_image_view<float> rect_fview0_;
  vil_image_view<float> rect_fview1_;

  vpgl_affine_camera<double> acam0_;
  vpgl_affine_camera<double> acam1_;
  vpgl_affine_camera<double> rect_acam0_;
  vpgl_affine_camera<double> rect_acam1_;

  vpgl_affine_fundamental_matrix<double> aF_;
  // H0, H1 include a translation to keep
  // image coordinates positive definite
  vnl_matrix_fixed<double, 3, 3> H0_;
  vnl_matrix_fixed<double, 3, 3> H1_;

  double du_off_;
  double dv_off_;
  size_t out_ni_;
  size_t out_nj_;
};

#endif // bpgl_rectify_affine_image_pair_h_
