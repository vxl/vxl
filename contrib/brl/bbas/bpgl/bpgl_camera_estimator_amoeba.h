#ifndef bpgl_camera_estimator_amoeba_h_
#define bpgl_camera_estimator_amoeba_h_

#include <iostream>
#include <sstream>
#include <limits>
#include "bpgl_camera_estimator.h"

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vnl/vnl_cost_function.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class func_>
class bpgl_camera_estimator_amoeba : public vnl_cost_function
{
 public:
  bpgl_camera_estimator_amoeba(bpgl_camera_estimator<func_> &cest,
                                const vil_image_view<float> &img_e,
                                const vpgl_camera_double_sptr& cam)
  : vnl_cost_function(2), cest_(cest), img_e_(img_e)
  {
    best_score = -std::numeric_limits<double>::min();

    // check the camera type
    if (cam->type_name() == "vpgl_perspective_camera") {
      cam_ = dynamic_cast<vpgl_perspective_camera<double>*>(cam.ptr());

      best_camera.set_calibration(cam_->get_calibration());
      best_camera.set_rotation(cam_->get_rotation());
      best_camera.set_camera_center(cam_->get_camera_center());

      vpgl_perspective_camera<double> cam_centered(cam_->get_calibration(),vgl_point_3d<double>(0.0,0.0,0.0),cam_->get_rotation());
      double im_center_x = 0.5*(double)img_e.ni();
      double im_center_y = 0.5*(double)img_e.nj();

      vgl_line_3d_2_points<double> ln_center = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y));
      vgl_line_3d_2_points<double> ln_center_x = cam_centered.backproject(vgl_point_2d<double>(im_center_x+1.0,im_center_y));
      vgl_line_3d_2_points<double> ln_center_y = cam_centered.backproject(vgl_point_2d<double>(im_center_x,im_center_y-1.0));

      vec_x = (ln_center_x.point2() - ln_center.point2());
      vec_y = (ln_center_y.point2() - ln_center.point2());

      normalize(vec_x);
      normalize(vec_y);

      cam_center = cam_->get_camera_center();
      cam_center = cam_center - 2.0*vec_x;
      cam_center = cam_center - 2.0*vec_y;
    }
    else {
      std::cout << "bvxm_expected_edge_functor::apply() -- The camera type: " << cam->type_name() << " is not implemented yet!" << std::endl;
    }
  }

  double get_result(vnl_vector<double>& x, vpgl_perspective_camera<double>* cam)
  {
    cam->set_calibration(best_camera.get_calibration());
    cam->set_camera_center(best_camera.get_camera_center());
    cam->set_rotation(best_camera.get_rotation());
    return best_score;
  }

  double f(const vnl_vector<double>& x) override
  {
    if (cam_) {
      vgl_point_3d<double> curr_center = (cam_center + (x[0]*vec_x)) + (x[1]*vec_y);

      vpgl_camera_double_sptr curr_cam_sptr = new vpgl_perspective_camera<double>(cam_->get_calibration(),curr_center,cam_->get_rotation());
      vpgl_perspective_camera<double>* curr_cam = static_cast<vpgl_perspective_camera<double>* >(curr_cam_sptr.as_pointer());

      cest_.estimate_rotation_iterative(img_e_,curr_cam);
      vil_image_view<float> img_eei(img_e_.ni(),img_e_.nj());

      cest_.get_expected_edge_image(curr_cam_sptr,&img_eei);
      double curr_score = cest_.edge_prob_cross_correlation(img_e_,img_eei);

#if 0
      vil_image_view<vxl_byte> img_eei_vb(img_eei.ni(),img_eei.nj(),1);
      brip_vil_float_ops::normalize_to_interval<float,vxl_byte>(img_eei,img_eei_vb,0.0f,255.0f);
      std::stringstream name; name << "./current_eei_" << curr_score << ".png";
      vil_save(img_eei_vb, name.str().c_str());
      std::cout << "x[0]: " << x[0] << " x[1]: " << x[1] << " ctr: " << curr_center << " score: " << curr_score << " best_score: " << best_score << '\n';
      std::cout.flush();
#endif
      if (curr_score > best_score) {
        best_score = curr_score;
        best_camera.set_camera_center(curr_cam->get_camera_center());
        best_camera.set_rotation(curr_cam->get_rotation());
      }
      return -curr_score;
    }
    return 0;
  }

  bpgl_camera_estimator<func_> &cest_;
  const vil_image_view<float> &img_e_;
  const vpgl_perspective_camera<double> *cam_;
  double best_score;
  vpgl_perspective_camera<double> best_camera;
  vgl_vector_3d<double> vec_x;
  vgl_vector_3d<double> vec_y;
  vgl_point_3d<double> cam_center;
};

#endif
