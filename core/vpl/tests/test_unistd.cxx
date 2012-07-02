// This is core/vpl/tests/test_unistd.cxx
#include <vcl_fstream.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_cstdio.h>
#include <vcl_string.h>
#include <vcl_cctype.h>
#include <vcl_cerrno.h>
#include <vcl_algorithm.h>

#include <testlib/testlib_test.h>

#include <vul/vul_string.h>
#include <vpl/vpl.h>
#include <vpl/vpl_fileno.h>
#include <vpl/vpl_fdopen.h>
#if defined(VCL_VC)
  #include <winsock2.h>
#endif


#ifdef VCL_WIN32
#define ROOT_PATH "C:/"
#else
#define ROOT_PATH "/tmp"
#endif

namespace
{
  char my_tolower(char c)
  {
    return vcl_tolower(c);
  }
}

static void test_unistd(int argc, char *argv[])
{
  // Test vpl_gethostname
  TEST("Expecting one cmdline argument", argc, 2);

  if (argc>=2)
  {
    char hostname[256];
    int retval = vpl_gethostname(hostname, 255);
    if (retval != 0)
    {
      vcl_cerr << "errno: " << errno
#if defined(VCL_VC)
               << "WSAErr: " << WSAGetLastError()
#endif
               << vcl_endl;
      vcl_perror("Failed to run gethostname(): ");
    }
    TEST_NEAR("vpl_gethostname reports no success", retval, 0, 0);
    vcl_string hostname_cmake = vcl_string(argv[1]);
    vcl_string hostname_vpl = vcl_string(hostname);
    //can't use vul_string_downcase because vul not built yet
    vcl_transform(hostname_cmake.begin(), hostname_cmake.end(), hostname_cmake.begin(), my_tolower);
    vcl_transform(hostname_vpl.begin(), hostname_vpl.end(), hostname_vpl.begin(), my_tolower);

    vcl_cout << "${SITE} = " << hostname_cmake << ", hostname = " << hostname_vpl << vcl_endl;
#ifdef DEBUG // temporarily commented out ... - PVr
    TEST("vpl_gethostname() agrees with CMake", hostname_cmake, hostname_vpl);
#endif
  }

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
    vcl_size_t n = vcl_fwrite (data, sizeof(data[0]), 1, fp);
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
    vcl_size_t n = vcl_fread (data, sizeof(data[0]), 1, fp2);
    TEST("fread return value", n, 1);
    vcl_cout << "fread return value: " << n << vcl_endl
             << "data[0]: " << data[0] << vcl_endl;
    TEST("fileno/fdopen", data[0], 99);
    vcl_fclose(fp2);
    if (vcl_feof(fp1)) // Visual Studio 8 seems to link the two FILE ptrs and
      vcl_fclose(fp1); // aborts if fp2 (and hence fp1) is already closed.
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
  vcl_cout << "\b\b\b, done\n\n"
           << "Current PID: " << vpl_getpid() << vcl_endl;
}

TESTMAIN_ARGS(test_unistd);
