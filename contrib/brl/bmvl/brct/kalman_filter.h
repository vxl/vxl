//:
// \file
// \author Kongbin Kang
// \brief interface for the kalman_filter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef brct_kalman_filter_h_
#define brct_kalman_filter_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vgl/vgl_point_3d.h>

class kalman_filter
{
#if 0
 public:
  //: initialize the kalman filter with dt
  virtual kalman_state inc(double dt);
#endif // 0
 public:
  vcl_vector<vgl_point_2d<double> > get_back_projection();
  vnl_matrix<double> get_predicted_curve();
  vnl_double_3 get_next_motion(vnl_double_3 v);
  vcl_vector<vgl_point_2d<double> > get_pre_observes();
  vcl_vector<vgl_point_2d<double> > get_cur_observes();
  vcl_vector<vgl_point_2d<double> > get_next_observes();
  void read_data(char* fname);
  vcl_vector<vgl_point_3d<double> > get_local_pts();

  //: initialize the kalman filter states
  void init();
  void inc();
  vnl_double_2 projection(const vnl_double_3x4 &P, const vnl_double_3 &X);
  void prediction();

  //: constructors
  //
  kalman_filter();
  kalman_filter(char* fname);
  virtual ~kalman_filter();

 protected:
  //: update the matched points in the next frame using closest neighbour.
  void update_observes(const vnl_double_3x4 &P, int iframe);
  void init_velocity();

  //: set linearized observation matrix
  vnl_matrix_fixed<double, 2, 6> get_H_matrix(vnl_double_3x4 &P, vnl_double_3 &Y);

  //: computer projective matrix from predicted position
  vnl_double_3x4 get_projective_matrix(vnl_double_3 &v);

  void init_covariant_matrix();
  void init_cam_intrinsic();

  void init_observes(vcl_vector<vnl_matrix<double> > &input);

  void init_state_vector();
  void init_transit_matrix();

 private:
   //: position and confidence of feature samples
  vcl_vector<vnl_double_3> curve_3d_;
  vcl_vector<double> prob_;

  vcl_vector<vnl_matrix<double> > observes_;

  //: each element of the vector represents a projection of the same 3D curves.
  vcl_vector<vdgl_digital_curve_sptr> curves_;

  vcl_vector<vnl_double_3> motions_;
  //: current frame position in history pool
  int cur_pos_;
  int queue_size_;
  int num_points_;

  //: how much the queue has been used
  int memory_size_;

  //: transit matrix
  vnl_matrix_fixed<double, 6, 6> A_;

  //: state vector
  vnl_vector_fixed<double, 6> X_;

  //: covariant matrix of state vector
  vnl_matrix_fixed<double, 6, 6> Q_;

  //: constrain kalman gain matrix
  vnl_matrix_fixed<double, 6, 2> G_;

  //: initial covariant matrix of state vector
  vnl_matrix_fixed<double, 2, 2> R_;

  //: covariant matrix of 2D projection
  vnl_matrix_fixed<double, 6, 6> Q0_;

  //: camera intrinsic parameters
  vnl_double_3x3 K_;
  double dt_;
};

#endif // brct_kalman_filter_h_
