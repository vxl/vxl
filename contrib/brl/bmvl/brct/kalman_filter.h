// kalman_filter.h: interface for the kalman_filter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KALMAN_FILTER_H__D477F484_3759_4092_8BCA_C3276CD534D9__INCLUDED_)
#define AFX_KALMAN_FILTER_H__D477F484_3759_4092_8BCA_C3276CD534D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include "kalman_state.h"

class kalman_filter  
{
public:
  //: initialize the kalman filter with
  //virtual kalman_state inc(double dt);
public:
	void read_data(char* fname);
	inc();
	update_covariant();
	vnl_vector_fixed<double, 2> projection(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X);
	prediction();
	kalman_filter();
	virtual ~kalman_filter();

protected:
	adjust_state_vector(vnl_vector_fixed<double, 2> &pred, vnl_vector_fixed<double, 2> &meas);
  //: set linearized observation matrix
	set_H_matrix(vnl_double_3x4 &P, vnl_vector_fixed<double, 3> &X);

  //: computer projective matrix from predicted position
  vnl_double_3x4	get_projective_matrix();

	init_covariant_matrix();
	init_cam_intrinsic();
	init_observes(vcl_vector<vnl_matrix<double> > &input);
	init_state_vector();
	init_transit_matrix();

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

#endif // !defined(AFX_KALMAN_FILTER_H__D477F484_3759_4092_8BCA_C3276CD534D9__INCLUDED_)
