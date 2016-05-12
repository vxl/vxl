#include <testlib/testlib_register.h>

DECLARE( test_refine );
DECLARE( test_image_pyramid );
DECLARE( test_process_mains );
DECLARE( test_weighted_em   );
DECLARE( test_kernel_filter );
DECLARE( test_kernel_vector_filter );
DECLARE( test_ocl_cholesky );
DECLARE( test_ocl_levenberg_marquardt );
DECLARE( test_ocl_lm_phongs_model );
DECLARE( test_opencl_cache2 );

void register_tests()
{
  REGISTER(test_refine);
  REGISTER( test_image_pyramid );
  REGISTER( test_process_mains );
  REGISTER( test_weighted_em   );
  REGISTER( test_kernel_filter );
  REGISTER( test_kernel_vector_filter );
  REGISTER( test_ocl_cholesky );
  REGISTER( test_ocl_levenberg_marquardt );
  REGISTER( test_ocl_lm_phongs_model );
  REGISTER( test_opencl_cache2 );
}

DEFINE_MAIN;
