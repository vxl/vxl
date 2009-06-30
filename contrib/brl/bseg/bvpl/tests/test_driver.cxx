#include <testlib/testlib_register.h>

DECLARE( test_subgrid );
DECLARE( test_bvpl_kernels );
DECLARE( test_neighb_oper );
DECLARE( test_bvpl_kernel_functors );
void register_tests()
{
  REGISTER( test_subgrid );
  REGISTER( test_bvpl_kernels );
  REGISTER( test_neighb_oper );
  REGISTER( test_bvpl_kernel_functors);
}


DEFINE_MAIN;
