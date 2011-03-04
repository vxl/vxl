#include <testlib/testlib_register.h>

DECLARE( test_octree_kernel_operator );
DECLARE( test_octree_neighbors );
DECLARE( test_pca_kernels );
DECLARE( test_taylor_basis );
void register_tests()
{
  REGISTER( test_octree_kernel_operator );
  REGISTER( test_octree_neighbors );
  REGISTER( test_pca_kernels );
  REGISTER( test_taylor_basis );
}


DEFINE_MAIN;
