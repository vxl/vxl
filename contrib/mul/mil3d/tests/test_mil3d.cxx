// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)

#include "test_image_3d_of.cxx"
#include "test_transform_3d.cxx"
#include "test_trilin_interp_3d.cxx"
#include "test_sample_profile_3d.cxx"

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_mil()
{
  test_image_3d_of();
  test_transform_3d();
  test_trilin_interp_3d();
  test_sample_profile_3d();
}

TESTMAIN(run_test_mil);
