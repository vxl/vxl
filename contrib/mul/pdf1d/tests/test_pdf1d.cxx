// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)

#include <pdf1d/tests/test_weighted_epanech_kernel_pdf.cxx>
#include <pdf1d/tests/test_gaussian_kernel_pdf.cxx>
#include <pdf1d/tests/test_gaussian.cxx>
#include <pdf1d/tests/test_exponential.cxx>
#include <pdf1d/tests/test_mixture.cxx>
#include <pdf1d/tests/test_compare_to_pdf_ks.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_pdf1d()
{
  test_weighted_epanech_kernel_pdf();
  test_gaussian_kernel_pdf();
  test_gaussian();
  test_exponential();
  test_mixture();
  test_compare_to_pdf_ks();
}


TESTMAIN(run_test_pdf1d);

