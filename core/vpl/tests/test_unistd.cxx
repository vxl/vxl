#include <vcl_fstream.h>

#include <testlib/testlib_test.h>

#include <vpl/vpl.h>
#include <sys/types.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_string.h>

#ifdef VCL_WIN32
#define ROOT_PATH "C:/TEMP"
#else
#define ROOT_PATH "/tmp"
#endif

static void test_unistd()
{
  vpl_mkdir(ROOT_PATH "/vpltest", 0777);
  vpl_chdir(ROOT_PATH "/vpltest");

  {
    vcl_ofstream f("file");
    f << 1234;
  }
  {
    vcl_ifstream f(ROOT_PATH "/vpltest/file");
    int s;
    f >> s;
    TEST("Create file in directory", s, 1234);
  }
  vpl_unlink("file");
  {
    vcl_ifstream f(ROOT_PATH "/vpltest/file");
    TEST("Unlink", f.good(), false);
  }

  vpl_chdir(ROOT_PATH);
  vpl_rmdir(ROOT_PATH "/vpltest");

  vpl_usleep(2000000); // 2 seconds

  vcl_string var("VPL_PUTENV_TEST");
  vcl_string value("GOOD");

  vpl_putenv((var + "=" + value).c_str());
  TEST("putenv", vcl_strcmp(vcl_getenv("VPL_PUTENV_TEST"), "GOOD"), 0);
}

TESTMAIN(test_unistd);
