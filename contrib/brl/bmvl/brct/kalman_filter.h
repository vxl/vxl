//:
// \file
// \author Kongbin Kang
// \brief A class to attack structure from motion problem
//
//////////////////////////////////////////////////////////////////////

#ifndef brct_kalman_filter_h_
#define brct_kalman_filter_h_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
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
#include <bbas/bugl/bugl_gaussian_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <bbas/bugl/bugl_gaussian_point_3d.h>
#include <bbas/bugl/bugl_curve_3d.h>

class kalman_filter
{
 public:

  //: get backprojection for debugging
  vgl_point_2d<double> get_cur_epipole() const;
  vcl_vector<vnl_matrix<double> > get_back_projection() const;

  //: predict next curve.
  vnl_matrix<double> get_predicted_curve();
  vnl_double_3 get_next_motion(vnl_double_3 v);
  vcl_vector<vgl_point_2d<double> > get_pre_observes();
  vcl_vector<vgl_point_2d<double> > get_cur_observes();
  vcl_vector<vgl_point_2d<double> > get_next_observes();
  vcl_vector<vgl_point_3d<double> > get_local_pts(); // will be superseded
  bugl_curve_3d get_curve_3d();

  //: read all the data including time stampes, trackers.
  void read_data(const char* fname);

  //: read vishual tracker result out of a file
  vcl_vector<vdgl_digital_curve_sptr> read_tracker_file(char* fname);

  //: initialize the kalman filter states
  void init();
  void init_epipole(double x, double y);
  void inc();
  vnl_double_2 projection(const vnl_double_3x4 &P, const vnl_double_3 &X);

  //: constructors
  //
  kalman_filter();
  kalman_filter(const char* fname);
  virtual ~kalman_filter();

 protected:
  //: read time stamp
  vcl_vector<double> read_timestamp_file(char* fname);

  //: if the zero probability returned, the matched point is a outlier
  double matched_point_prob(vnl_double_2& z, vnl_double_2& z_pred);

  //: get time interval from ith frame to j-th frame
  vnl_matrix_fixed<double, 6, 6> get_transit_matrix(int i, int j);

  //: update the confidence for each 3d point
  void update_confidence();

  //: update the matched points in the next frame using closest neighbour.
  void update_observes(const vnl_double_3x4 &P, int iframe);
  void init_velocity();

  //: set linearized observation matrix
  vnl_matrix_fixed<double, 2, 6> get_H_matrix(vnl_double_3x4 &P, vnl_double_3 &Y);

  //: compute projective matrix from predicted position
  vnl_double_3x4 get_projective_matrix(const vnl_double_3 &v) const;

  void init_covariant_matrix();
  void init_cam_intrinsic();
  void init_state_3d_estimation();
  void init_transit_matrix();

 private:
   //: position and confidence of feature samples
  bugl_curve_3d curve_3d_;

  vcl_vector<double> prob_;

  //: used for matching point
  vcl_vector<vcl_vector<bugl_gaussian_point_2d<double> > > observes_;

  //: each element represents shooting times for this frame.
  vcl_vector<double> time_tick_;
  //: each element of the vector represents a projection of the same 3D curves.
  vcl_vector<vcl_vector<vdgl_digital_curve_sptr> > trackers_;

  vcl_vector<vnl_double_3> motions_;
  //: current frame position in history pool
  int cur_pos_;
  int queue_size_;
  int num_points_;

  //: how much the queue has been used
  int memory_size_;

  //: state vector
  vnl_vector_fixed<double, 6> X_;

  //: covariant matrix of state vector
  vnl_matrix_fixed<double, 6, 6> Q_;

  //: constraint kalman gain matrix
  vnl_matrix_fixed<double, 6, 2> G_;

  //: initial covariant matrix of state vector
  vnl_matrix_fixed<double, 2, 2> R_;

  //: covariant matrix of 2D projection
  vnl_matrix_fixed<double, 6, 6> Q0_;

  //: initial epipole
  vnl_double_2* e_;

  //: camera intrinsic parameters
  vnl_double_3x3 K_;

  //: used to denote outlier point in image
  static const double large_num_;
};

#endif // brct_kalman_filter_h_
