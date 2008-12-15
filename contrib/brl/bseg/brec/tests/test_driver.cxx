#include <testlib/testlib_register.h>

DECLARE( test_brec_densities );
DECLARE( test_param_estimation );
DECLARE( test_brec_parts );
DECLARE( test_brec_hierarchy_builder );
DECLARE( test_brec_hierarchy_detector );

void register_tests()
{
  REGISTER( test_brec_densities );
  REGISTER( test_param_estimation );
  REGISTER( test_brec_parts );
  REGISTER( test_brec_hierarchy_builder );
  REGISTER( test_brec_hierarchy_detector );
}

DEFINE_MAIN;
