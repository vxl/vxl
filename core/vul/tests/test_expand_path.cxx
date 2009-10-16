// This is core/vul/tests/test_expand_path.cxx
#include <vpl/vpl.h>
#include <vul/vul_expand_path.h>
#include <vcl_fstream.h>
#include <testlib/testlib_test.h>


void test_expand_path()
{
  // vul_expand_path
  TEST("expand path", vul_expand_path("/abc///d/e/../"), "/abc/d");
}


#if defined(VCL_WIN32) && defined(VXL_SUPPORT_WIN_UNICODE)

void test_expand_path_wchar_ext()
{
  // vul_expand_path
  TEST("expand path (wchar_t)", vul_expand_path(L"/abc///d/e/../"), L"/abc/d");
}

#endif

//TESTMAIN(test_expand_path);
int test_expand_path(int, char*[])
{
  testlib_test_start("test_expand_path");

  test_expand_path();

#if defined(VCL_WIN32) && defined(VXL_SUPPORT_WIN_UNICODE)
  test_expand_path_wchar_ext();
#endif

  return testlib_test_summary();
}
