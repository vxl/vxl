#pragma once
#include <iostream>
#include <map>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <boxm2/vecf/boxm2_vecf_orbit_params.h>
class boxm2_vecf_estimate_camera_from_canthus{
public:
  boxm2_vecf_estimate_camera_from_canthus():canthus_line_length_(95.0),iris_nominal_(5.5),n_dlib_orbit_parts_(22),n_dlib_alfw_landmarks_(22){
    H_ = vnl_matrix_fixed<double,3,3>(0.0);
    scale_ = 1.0; psi_ = phi_ = 0.0; z0_ = 0;
  }
  void add_dlib_part(vgl_point_2d<double> pt,std::string label){
    dlib_part_map_[label] = pt;
  }

  bool estimate_camera(vgl_vector_2d<double> t = vgl_vector_2d<double>(0,0));
  bool parse_files(std::string& left_dlib_path,std::string& right_dlib_path, std::string& alfw_path);
  vgl_h_matrix_2d<double> homography() {return H_;}
private:

  std::map<std::string,vgl_point_2d<double> > dlib_part_map_;
  vgl_h_matrix_2d<double> H_;
  const double canthus_line_length_; //these values are constant across subjects
  const double iris_nominal_ ;
  const unsigned n_dlib_orbit_parts_ ;
  const unsigned n_dlib_alfw_landmarks_;
  double scale_, phi_,psi_;
  double z0_;
  vgl_vector_2d<double> t_;

};
