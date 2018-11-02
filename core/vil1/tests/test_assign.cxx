#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_load.h>
#include <testlib/testlib_test.h>

static void test_assign(int argc, char* argv[])
{
  vil1_image image;
  for (int i=1; i<argc; ++i) {
    vil1_image tmp( vil1_load(argv[i]) );
    std::cerr << "tmp = " << tmp << std::endl;
    image = tmp;
  }
}

TESTMAIN_ARGS(test_assign);
