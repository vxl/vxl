#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_spline.h>
#include <rgrl/rgrl_trans_spline.h>
#include <rgrl/rgrl_est_spline.h>
#include <rgrl/rgrl_est_quadratic.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_est_rigid.h>
#include <rgrl/rgrl_trans_reduced_quad.h>
#include <rgrl/rgrl_est_reduced_quad2d.h>
#include <rgrl/rgrl_mask.h>

#include "test_util.h"

MAIN( test_convergence )
{
  START( "various convergence objects" );

  // there should be a test to make sure that all convergence objects converge when we are registering 
  // identical data sets

  // currently, there is a chunk of code in each of rgrl_convergence_on_median_error and rgrl_convergence_on_weighted_error {.cxx}
  // that looks like: 
  // bool converged = false;
  //  if (prev_status && current_view.regions_converged(prev_view) ) { 
  //  ... 
  // converged = vcl_abs( (new_error-old_error) / new_error ) < 1e-3; 
  // ... }

  // one problem that I see is: converged will always be false after the first iteration
  // is this really a warranted assumption?
  // although the error would most likely never be == to 0.0f on a real data set, 
  // i could imagine a threshold incorporated such as: 
  // bool converged = false;
  //  if (new_error < tolerance_/200) converged = true;
  //  else if (prev_status && current_view.regions_converged(prev_view) ) { 
  //  ... 
  // converged = vcl_abs( (new_error-old_error) / new_error ) < 1e-3; 
  // ... }  
  // however; i'm not sur ethat tolerance_/200 is a good measure of convergence

  SUMMARY();
}
