// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <vpdfl/tests/test_axis_gaussian.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_vpdfl()
{
  test_axis_gaussian();
}


TESTMAIN(run_test_vpdfl);
