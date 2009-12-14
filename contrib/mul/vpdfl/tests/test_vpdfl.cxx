// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)

#include "test_mixture.cxx"
#include "test_gaussian.cxx"
#include "test_axis_gaussian.cxx"
#include "test_pc_gaussian.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() { RUN_TEST_FUNC(x); }

void run_test_vpdfl()
{
  test_mixture();
  test_axis_gaussian();
  test_gaussian();
  test_pc_gaussian();
}


TESTLIB_DEFINE_MAIN(run_test_vpdfl);
