// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <mil/tests/test_transform_2d.cxx>
#include <mil/tests/test_image_2d_of.cxx>
#include <mil/tests/test_bilin_interp_2d.cxx>
#include <mil/tests/test_gauss_reduce_2d.cxx>
#include <mil/tests/test_gaussian_pyramid_builder_2d.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_vsml()
{
  test_transform_2d();
  test_image_2d_of();
  test_bilin_interp_2d();
  test_gauss_reduce_2d();
  test_gaussian_pyramid_builder_2d();
}


TESTMAIN(run_test_vsml);
