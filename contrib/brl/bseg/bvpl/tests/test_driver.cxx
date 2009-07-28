#include <testlib/testlib_register.h>

DECLARE( test_subgrid );
DECLARE( test_bvpl_kernels );
DECLARE( test_neighb_oper );
DECLARE( test_bvpl_kernel_functors );
DECLARE( test_direction_to_color_map );
DECLARE( test_bvpl_vector_operator );

void register_tests()
{
  //REGISTER( test_subgrid );
  //REGISTER( test_bvpl_kernels );
  //REGISTER( test_neighb_oper );
  //REGISTER( test_bvpl_kernel_functors);
  //REGISTER( test_direction_to_color_map);
  REGISTER( test_bvpl_vector_operator);

}


DEFINE_MAIN;
