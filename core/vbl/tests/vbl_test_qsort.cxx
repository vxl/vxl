#include <iostream>
#include <testlib/testlib_test.h>
#include <vbl/vbl_qsort.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static
void vbl_test_qsort()
{
  std::cout << "\n\n\n"
           << "****************\n"
           << " Test vbl_qsort\n"
           << "****************\n";
  std::vector<double> v(10);
  for (int i=0; i<10; ++i) v[i] = 0.1*i*i - i + 1; // parabola with top (5,-1.5)
  vbl_qsort_ascending(v);
  for (int i=1; i<10; ++i)
    TEST("qsort ascending", v[i-1] <= v[i], true);
  vbl_qsort_descending(v);
  for (int i=1; i<10; ++i)
    TEST("qsort descending", v[i-1] >= v[i], true);
}

TESTMAIN(vbl_test_qsort);
