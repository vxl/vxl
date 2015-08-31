#include <testlib/testlib_register.h>

DECLARE( test_points );
DECLARE( test_wt_mat_2d );
DECLARE( test_aligners );
DECLARE( test_shape_model );
DECLARE( test_curves );
DECLARE( test_param_limiters );
DECLARE( test_cubic_bezier );

void register_tests()
{
  REGISTER( test_points );
  REGISTER( test_wt_mat_2d );
  REGISTER( test_aligners );
  REGISTER( test_shape_model );
  REGISTER( test_curves );
  REGISTER( test_param_limiters );
  REGISTER( test_cubic_bezier );
}


DEFINE_MAIN;
