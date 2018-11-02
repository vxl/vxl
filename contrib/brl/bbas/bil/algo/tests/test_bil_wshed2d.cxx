#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_bil_wshed2d()
{
  std::cout << "2D Watershed Transform\n";
}

TESTMAIN(test_bil_wshed2d);
