// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)

#include "test_image_3d_of.cxx"
#include "test_transform_3d.cxx"
#include "test_trilin_interp_3d.cxx"
#include "test_sample_profile_3d.cxx"
#include "test_gaussian_pyramid_builder_3d.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() \
  { testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_mil()
{
  test_image_3d_of();
  test_transform_3d();
  test_trilin_interp_3d();
  test_sample_profile_3d();
  test_gaussian_pyramid_builder_3d();
}

TESTLIB_DEFINE_MAIN(run_test_mil);
