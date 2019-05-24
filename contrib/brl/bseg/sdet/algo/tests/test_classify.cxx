#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <sdet/algo/sdet_classify.h>

static void test_classify()
{
  TEST("classify", true, true);
}

TESTMAIN(test_classify);
