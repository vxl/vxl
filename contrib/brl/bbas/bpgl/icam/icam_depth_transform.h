// This is bbas/bpgl/icam/icam_depth_transform.h
#ifndef icam_depth_transform_h_
#define icam_depth_transform_h_
//:
// \file
// \brief An image to image transformation with respect to a depth map
// The transformation between an image with a depth map
// and an image with a camera specified by K, R and t. The "from" image
// coordinates are transformed by inverse K_from, i.e.,
//
// [u'_from]               [u_from]
// [v'_from] = (K_from)^-1 [v_from]
// [   1   ]               [   1  ]
//
// and then mapped according to
//
//             [u'_from]        t0
//          R0 [v'_from] + --------------
//             [  1    ]  Z(u_from,v_from)
// u'_to = -------------------------------
//             [u'_from]        t2
//          R2 [v'_from] + --------------
//             [  1    ]  Z(u_from,v_from)
//
//where R0 and R2 are the first and third rows of the to_camera rotation matrix
//and t0 and t2 are first and third components of the to_camera translation.
// and similarly for v'_to, with R1 and t1 in the numerator. The transformed
// coordinates are mapped back to the target image by
//
// [u_to]          [u'_to]
// [v_to] =   K_to [v'_to]
// [ 1  ]          [  1  ]
//
// \author J.L. Mundy
// \date Sept 04, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>

class icam_depth_transform
{
 public:
  icam_depth_transform(): scale_factors_(vnl_vector<double>()) {}

  //: Construct with the same calibration matrix
  // \p adjust_to_fl determines if the to_focal_length can be varied in a search
  icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K,
                       vil_image_view<double> const& depth,
                       vgl_rotation_3d<double>  rot,
                       vgl_vector_3d<double> const& trans,
                       bool adjust_to_fl = false);
  //: Construct with separate from and to calibration matrices
  icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K_from,
                       vnl_matrix_fixed<double, 3, 3> const& K_to,
                       vil_image_view<double> const& depth,
                       vgl_rotation_3d<double>  rot,
                       vgl_vector_3d<double> const& trans,
                       bool adjust_to_fl = false);
  //: Construct with separate from and to calibration matrices
  icam_depth_transform(vnl_matrix_fixed<double, 3, 3> const& K_from,
                       double to_fl, double to_pu, double to_pv,
                       vil_image_view<double> const& depth,
                       vgl_rotation_3d<double>  rot,
                       vgl_vector_3d<double> const& trans,
                       bool adjust_to_fl = false);
  ~icam_depth_transform() = default;
  void set_scale_factors(vnl_vector<double> const& scales)
  { scale_factors_ = scales; }
  vnl_vector<double>& scale_factors() {return scale_factors_;}
  void set_adjust_to_fl(bool adjust) {adjust_to_fl_=adjust;}
  bool adjust_to_fl() {return adjust_to_fl_;}
  void set_calibration_matrix(vnl_matrix_fixed<double, 3, 3> const& K)
  { this->set_k(K); }
  void set_calibration_matrix(vnl_matrix_fixed<double, 3, 3> const& K_from,
                              vnl_matrix_fixed<double, 3, 3> const& K_to)
  { this->set_k(K_from, K_to); }
  void set_depth_map(vil_image_view<double> const& depth)
    {depth_ = depth; invert_depth(depth);}
  void set_rotation(vgl_rotation_3d<double> const& rot) {rot_=rot;}
  void set_translation(vgl_vector_3d<double> const& trans) {trans_ = trans;}
  vnl_matrix_fixed<double, 3, 3> calibration_matrix() {return K_to();}
  vnl_matrix_fixed<double, 3, 3>& from_calibration_matrix_inv()
  { return K_from_inv_; }
  vnl_matrix_fixed<double, 3, 3> to_calibration_matrix()
  { return K_to(); }

  //: the elements of the K_to
  double to_fl() {return to_fl_;}
  double to_pu() {return to_pu_;}
  double to_pv() {return to_pv_;}

  vil_image_view<double>& depth_map() {return depth_ ;}
  vgl_rotation_3d<double> rotation() {return vgl_rotation_3d<double>(rot_);}
  vgl_vector_3d<double>& translation() {return trans_;}

  bool transform(double from_u, double from_v, double& to_u, double& to_v) const;
  bool transform(vgl_point_2d<double> const& from_p,
                 vgl_point_2d<double>& to_p ) const
  { double u, v; bool r = transform(from_p.x(), from_p.y(), u, v); to_p.set(u,v); return r; }

  //: support for cost function
  unsigned n_params() {if (adjust_to_fl_)return 7; return 6;}

  //:
  // params are [a.vx, a.vy, a.vz, tx, ty, tz]
  // where a.v is a Rodrigues vector (a angle of rotation, v rotation axis)
  // the projection for the "from" camera is K[R, t]
  void set_params(vnl_vector<double> const& params);

  //:
  // params are [a.vx, a.vy, a.vz, tx, ty, tz]
  // where a.v is a Rodrigues vector (a angle of rotation, v rotation axis)
  // the projection for the "from" camera is K[R, t]
  vnl_vector<double> params();

 private:
  // === utility functions ===

  void set_k(vnl_matrix_fixed<double, 3, 3> const& K);
  void set_k(vnl_matrix_fixed<double, 3, 3> const& K_from,
             vnl_matrix_fixed<double, 3, 3> const& K_to);
  //: convert to matrix
  vnl_matrix_fixed<double,3,3> K_to();

  //:compute 1/Z(u,v) for efficiency
  void invert_depth(vil_image_view<double> const& depth);

  // === members ===

  bool adjust_to_fl_;
  vnl_matrix_fixed<double,3,3> K_from_inv_;
  vil_image_view<double> depth_;
  vil_image_view<float> inv_depth_;
  //: element of the 'to' calibration matrix
  double to_fl_;
  //: element of the 'to' calibration matrix
  double to_pu_;
  //: element of the 'to' calibration matrix
  double to_pv_;
  //: exterior transform parameter
  vgl_rotation_3d<double> rot_;
  //: exterior transform parameter
  vgl_vector_3d<double> trans_;
  //: equalize the dynamic range of camera parameters (typically all 1.0)
  vnl_vector<double> scale_factors_;
  // caches
  void cache_k();
  void cache_r();
  //: elements of (K_from)^-1 ( the inverse calibration matrix)
  float k00_, k02_, k11_, k12_;
  //: elements of the rotation matrix, cached for efficiency
  float r00_, r01_, r02_, //R0
        r10_, r11_, r12_, //R1
        r20_, r21_, r22_; //R2
};

#endif // icam_depth_transform_h_
