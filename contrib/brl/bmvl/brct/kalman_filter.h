//:
// \file
// \brief interface for the kalman_filter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef brct_kalman_filter_h_
#define brct_kalman_filter_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vgl/vgl_point_3d.h>

class kalman_filter
{
 public:
  //: initialize the kalman filter with
  //virtual kalman_state inc(double dt);
 public:
  void read_data(char* fname);
  vcl_vector<vgl_point_3d<double> > kalman_filter::get_local_pts();

  //: initialize the kalman filter states
  void init();
  void inc();
  void update_covariant();
  vnl_vector_fixed<double, 2> projection(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X);
  void prediction();

  //: constructors
  //
  kalman_filter();
  kalman_filter(char* fname);
  virtual ~kalman_filter();

 protected:
  void init_velocity();
  void adjust_state_vector(vnl_vector_fixed<double, 2> const& pred, vnl_vector_fixed<double, 2> const& meas);

  //: set linearized observation matrix
  void set_H_matrix(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X);

  //: computer projective matrix from predicted position
  vnl_double_3x4 get_projective_matrix();

  void init_covariant_matrix();
  void init_cam_intrinsic();
  void init_observes(vcl_vector<vnl_matrix<double> > &input);
  void init_state_vector();
  void init_transit_matrix();

 private:
  vcl_vector<vnl_vector_fixed<double, 3> > Xl_;
  vcl_vector<vnl_matrix<double> > observes_;

  //: each element of the vector represents a projection of the same 3D curves.
  vcl_vector<vdgl_digital_curve_sptr> curves_;

  vcl_vector<vnl_double_3x4> motions_;
  //: current frame position in history pool
  int cur_pos_;
  int queue_size_;
  int num_points_;
  int memory_size_;

  //: transit matrix
  vnl_matrix_fixed<double, 6, 6> A_ ;

  //: state vector
  vnl_vector_fixed<double, 6> X_;

  //: state vector predicted
  vnl_vector_fixed<double, 6> X_pred_;

  //: linearized stat vector projective matrix
  vnl_matrix_fixed<double, 2, 6> H_;

  //: covariant matrix of state vector
  vnl_matrix_fixed<double, 6, 6> P_;

  //: constrain weighting matrix
  vnl_matrix_fixed<double, 6, 2> K_;

  //: initial covariant matrix of state vector
  vnl_matrix_fixed<double, 2, 2> R_;

  //: covariant matrix of 2D projection
  vnl_matrix_fixed<double, 6, 6> Q_;

  //: camera intrinsic parameters
  vnl_matrix_fixed<double, 3, 3> M_in_;
  double dt_ ;
};

#endif // brct_kalman_filter_h_
