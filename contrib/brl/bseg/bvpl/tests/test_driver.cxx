#include <testlib/testlib_register.h>

DECLARE( test_subgrid );
DECLARE( test_bvpl_kernels );
DECLARE( test_neighb_oper );

void register_tests()
{
  //REGISTER( test_subgrid );
  //REGISTER( test_bvpl_kernels );
  REGISTER( test_neighb_oper );
}


DEFINE_MAIN;
