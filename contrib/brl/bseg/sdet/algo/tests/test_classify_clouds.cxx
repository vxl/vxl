#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sdet/algo/sdet_classify_clouds.h>

static void test_classify_clouds()
{
  TEST("classify clouds", true, true);
}

TESTMAIN(test_classify_clouds);
