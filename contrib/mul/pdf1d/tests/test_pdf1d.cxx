// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)

#include "test_weighted_epanech_kernel_pdf.cxx"
#include "test_gaussian_kernel_pdf.cxx"
#include "test_gaussian.cxx"
#include "test_exponential.cxx"
#include "test_mixture.cxx"
#include "test_compare_to_pdf_ks.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() \
  { testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_pdf1d()
{
  test_weighted_epanech_kernel_pdf();
  test_gaussian_kernel_pdf();
  test_gaussian();
  test_exponential();
  test_mixture();
  test_compare_to_pdf_ks();
}


TESTLIB_DEFINE_MAIN(run_test_pdf1d);

