#include <testlib/testlib_register.h>

DECLARE( test_subgrid );
DECLARE( test_bvpl_kernels );

void register_tests()
{
  REGISTER( test_subgrid );
  REGISTER( test_bvpl_kernels );
}


DEFINE_MAIN;
