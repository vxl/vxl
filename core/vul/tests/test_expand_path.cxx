// This is core/vul/tests/test_expand_path.cxx
#include <vpl/vpl.h>
#include <vul/vul_expand_path.h>
#include <vcl_fstream.h>
#include <testlib/testlib_test.h>


void test_expand_path()
{
  // vul_expand_path
  TEST("expand path", vul_expand_path("/abc///d/e/../"), "/abc/d");

#if defined(VCL_WIN32) && VXL_USE_WIN_WCHAR_T
  TEST("expand path (wchar_t)", vul_expand_path(L"/abc///d/e/../"), L"/abc/d");
#endif
}

TEST_MAIN(test_expand_path);
