// This is core/vpl/tests/test_unistd.h
#include <vcl_fstream.h>

#include <testlib/testlib_test.h>

#include <vpl/vpl.h>
#include <vpl/vpl_fileno.h>
#include <vpl/vpl_fdopen.h>

#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_string.h>

#ifdef VCL_WIN32
#define ROOT_PATH "C:/"
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
  {
    // write data to a file

    vcl_FILE * fp = vcl_fopen ("file", "w");
    TEST("fopen non-NULL FILE", fp != NULL, true);
    int data[1] = { 99 };
    int n = vcl_fwrite (data, sizeof(data[0]), 1, fp);
    vcl_cout << "fwrite return value: " << n << vcl_endl;
    TEST("fwrite return value", n, 1);
    vcl_fclose (fp);
  }
  {
    // read data from file after FILE -> fd -> FILE conversion

    vcl_FILE * fp1 = vcl_fopen ("file", "r");
    TEST("fopen non-NULL FILE", fp1 != NULL, true);
    int fd = vpl_fileno (fp1);
    TEST("fileno positive", fd >= 0, true);
    vcl_cout << "file number: " << fd << vcl_endl;
    vcl_FILE * fp2 = vpl_fdopen (fd, "r");
    TEST("fdopen non-NULL FILE", fp2 != NULL, true);
    int data[1] = { 0 };
    int n = vcl_fread (data, sizeof(data[0]), 1, fp2);
    TEST("fread return value", n, 1);
    vcl_cout << "fread return value: " << n << vcl_endl;
    vcl_cout << "data[0]: " << data[0] << vcl_endl;
    TEST("fileno/fdopen", data[0], 99);
    vcl_fclose (fp1);
    vcl_fclose (fp2);
    vpl_unlink ("file");
  }

  vpl_chdir(ROOT_PATH);
  vpl_rmdir(ROOT_PATH "/vpltest");

  vcl_string var("VPL_PUTENV_TEST");
  vcl_string value("GOOD");

  vpl_putenv((var + "=" + value).c_str());
  TEST("putenv", vcl_strcmp(vcl_getenv("VPL_PUTENV_TEST"), "GOOD"), 0);

  vcl_cout << "sleeping for 2 seconds..." << vcl_flush;
  vpl_usleep(2000000); // 2 seconds
  vcl_cout << "done" << vcl_endl;
}

TESTMAIN(test_unistd);
