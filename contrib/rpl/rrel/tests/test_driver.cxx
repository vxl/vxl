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
}

DEFINE_MAIN;


#include <mbl/mbl_mz_random.h>
#include <vcl_cmath.h>

double noise( double sigma )
{
  static mbl_mz_random rand;
  return rand.normal() * sigma;
}

bool close( double a, double b )
{
  return vcl_fabs( a - b ) < 1e-5;
}
