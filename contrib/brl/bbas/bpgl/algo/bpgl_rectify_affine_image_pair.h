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
#include <vnl/vnl_matrix_fixed.h>
class bpgl_rectify_affine_image_pair
{
 public:
  bpgl_rectify_affine_image_pair(){}
  bpgl_rectify_affine_image_pair(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                                 vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1):
  resc0_(resc0), acam0_(acam0), resc1_(resc1), acam1_(acam1){
    aF_ = vpgl_affine_fundamental_matrix<double> (acam0_, acam1_);
    bool good = this->convert_to_float();
  }

  bool load_images_and_cams(std::string const& image0_path, std::string const& cam0_path,
                            std::string const& image1_path, std::string const& cam1_path);

  bool set_images_and_cams(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                           vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1){
    resc0_ = resc0; acam0_ = acam0; resc1_ = resc1; acam1_ = acam1;
    aF_ = vpgl_affine_fundamental_matrix<double>(acam0_, acam1_);
    return this->convert_to_float();
  }
  
  //: accessors
  const vpgl_affine_camera<double>& acam0()const{return acam0_;}
  const vpgl_affine_camera<double>& acam1()const{return acam1_;}
  const vpgl_affine_camera<double>& rect_acam0()const{return rect_acam0_;}
  const vpgl_affine_camera<double>& rect_acam1()const{return rect_acam1_;}
  const vil_image_view<float>& input_float_view0()const{return fview0_;}
  const vil_image_view<float>& input_float_view1()const{return fview1_;}
  const vil_image_view<float>& rectified_fview0() const{return rect_fview0_;}
  const vil_image_view<float>& rectified_fview1() const{return rect_fview1_;}
  //: utility methods
  static bool load_affine_camera(std::string const& cam_path, vpgl_affine_camera<double>& acam);
  bool convert_to_float();
  bool compute_rectification(vgl_box_3d<double>& scene_box, size_t n_points = 1000);
  void compute_warp_dimensions_offsets();
  void warp_image(vil_image_view<float> fview,vnl_matrix_fixed<double, 3, 3> const& H,
                  vil_image_view<float>& fwarp);
  void warp_pair();
 private:
  vil_image_resource_sptr resc0_;
  vil_image_resource_sptr resc1_;
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
