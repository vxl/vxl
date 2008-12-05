#include <testlib/testlib_register.h>

DECLARE( test_bvxm_densities );
DECLARE( test_param_estimation );
DECLARE( test_bvxm_parts );
DECLARE( test_bvxm_hierarchy_builder );
DECLARE( test_bvxm_hierarchy_detector );

void register_tests()
{
  //REGISTER( test_bvxm_densities );
  //REGISTER( test_param_estimation );
  //REGISTER( test_bvxm_parts );
  REGISTER( test_bvxm_hierarchy_builder );
  REGISTER( test_bvxm_hierarchy_detector );
}

DEFINE_MAIN;
