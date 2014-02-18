#include <testlib/testlib_register.h>

DECLARE( test_octree_kernel_operator );
DECLARE( test_octree_neighbors );
//DECLARE( test_pca_kernels );//TEST EXHIBITS PATH ERRORS
//DECLARE( test_taylor_basis );//TEST FAILS FIX_ME
//DECLARE( test_gauss3D_steerable_filters );//TEST FAILS FIX_ME

void register_tests()
{
  REGISTER( test_octree_kernel_operator );
  REGISTER( test_octree_neighbors );
  //  REGISTER( test_pca_kernels );
  //  REGISTER( test_taylor_basis );
  //  REGISTER( test_gauss3D_steerable_filters );
}


DEFINE_MAIN;
