#include <iostream>
#include <cmath>
#include <testlib/testlib_register.h>


DECLARE( test_similarity_from_matches );
DECLARE( test_homography2d_est );
DECLARE( test_shift2d_est );
DECLARE( test_irls );
DECLARE( test_linear_reg );
DECLARE( test_lms_lts );
DECLARE( test_m_est_obj );
DECLARE( test_orthogonal_regression );
DECLARE( test_ran_sam_search );
DECLARE( test_ransac_obj );
DECLARE( test_robust_util );
DECLARE( test_muse_table );

void
register_tests()
{
  REGISTER( test_similarity_from_matches );
  REGISTER( test_homography2d_est );
  REGISTER( test_shift2d_est );
  REGISTER( test_irls );
  REGISTER( test_linear_reg );
  REGISTER( test_lms_lts );
  REGISTER( test_m_est_obj );
  REGISTER( test_orthogonal_regression );
  REGISTER( test_ran_sam_search );
  REGISTER( test_ransac_obj );
  REGISTER( test_robust_util );
  REGISTER( test_muse_table );
}

DEFINE_MAIN;

#include <vnl/vnl_random.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double noise( double sigma )
{
  static vnl_random rand;
  return rand.normal() * sigma;
}

bool close( double a, double b )
{
  return std::fabs(a-b) < 1e-5;
}
