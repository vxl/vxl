// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <mil/tests/test_transform_2d.cxx>
#include <mil/tests/test_image_2d_of.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_vsml()
{
  test_transform_2d();
  test_image_2d_of();
}


TESTMAIN(run_test_vsml);
