#include <testlib/testlib_register.h>

DECLARE( test_histogram );
DECLARE( test_mutual_info );
DECLARE( test_watershed );
DECLARE( test_fourier );
DECLARE( test_homography );
DECLARE( test_lucas_kanade );
DECLARE( test_Horn_Schunck );
DECLARE( test_quadtree );
DECLARE( test_max_scale_response );
DECLARE( test_mask );
DECLARE( test_label_equivalence );
DECLARE( test_extrema );
DECLARE( test_filter_bank );
DECLARE( test_gain_offset_solver );
DECLARE( test_nitf_ops );
DECLARE( test_phase_correlation );

void
register_tests()
{
  REGISTER( test_histogram );
  REGISTER( test_mutual_info );
  REGISTER( test_watershed );
  REGISTER( test_fourier );
  REGISTER( test_homography );
  REGISTER( test_lucas_kanade );
  REGISTER( test_Horn_Schunck );
  REGISTER( test_quadtree );
  REGISTER( test_max_scale_response );
  REGISTER( test_mask );
  REGISTER(test_label_equivalence);
  REGISTER( test_extrema );
  REGISTER( test_filter_bank );
  REGISTER( test_gain_offset_solver );
  REGISTER( test_nitf_ops );
  REGISTER( test_phase_correlation );

}

DEFINE_MAIN;
