// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include "test_mixture.cxx"
#include "test_gaussian.cxx"
#include "test_axis_gaussian.cxx"
#include "test_pc_gaussian.cxx"

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_vpdfl()
{
  test_mixture();
  test_axis_gaussian();
  test_gaussian();
  test_pc_gaussian();
}


TESTMAIN(run_test_vpdfl);
