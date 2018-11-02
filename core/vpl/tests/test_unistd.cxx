// This is core/vpl/tests/test_unistd.cxx
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <cctype>
#include <iostream>
#include <cerrno>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <testlib/testlib_test.h>

#include <vul/vul_string.h>
#include <vpl/vpl.h>
#include <vpl/vpl_fileno.h>
#include <vpl/vpl_fdopen.h>
#if defined(_MSC_VER)
  #include <winsock2.h>
#endif


#ifdef _WIN32
#define ROOT_PATH "C:/"
#else
#define ROOT_PATH "/tmp"
#endif

namespace
{
  char my_tolower(char c)
  {
    return std::tolower(c);
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
      std::cerr << "errno: " << errno
#if defined(_MSC_VER)
               << "WSAErr: " << WSAGetLastError()
#endif
               << std::endl;
      std::perror("Failed to run gethostname(): ");
    }
    TEST_NEAR("vpl_gethostname reports no success", retval, 0, 0);
    std::string hostname_cmake = std::string(argv[1]);
    std::string hostname_vpl = std::string(hostname);
    //can't use vul_string_downcase because vul not built yet
    std::transform(hostname_cmake.begin(), hostname_cmake.end(), hostname_cmake.begin(), my_tolower);
    std::transform(hostname_vpl.begin(), hostname_vpl.end(), hostname_vpl.begin(), my_tolower);

    std::cout << "${SITE} = " << hostname_cmake << ", hostname = " << hostname_vpl << std::endl;
#ifdef DEBUG // temporarily commented out ... - PVr
    TEST("vpl_gethostname() agrees with CMake", hostname_cmake, hostname_vpl);
#endif
  }

  vpl_mkdir(ROOT_PATH "/vpltest", 0777);
  vpl_chdir(ROOT_PATH "/vpltest");

  {
    std::ofstream f("file");
    f << 1234;
  }
  {
    std::ifstream f(ROOT_PATH "/vpltest/file");
    int s;
    f >> s;
    TEST("Create file in directory", s, 1234);
  }
  vpl_unlink("file");
  {
    std::ifstream f(ROOT_PATH "/vpltest/file");
    TEST("Unlink", f.good(), false);
  }
  {
    // write data to a file

    std::FILE * fp = std::fopen ("file", "w");
    TEST("fopen non-NULL FILE", fp != nullptr, true);
    int data[1] = { 99 };
    std::size_t n = std::fwrite (data, sizeof(data[0]), 1, fp);
    std::cout << "fwrite return value: " << n << std::endl;
    TEST("fwrite return value", n, 1);
    std::fclose (fp);
  }
  {
    // read data from file after FILE -> fd -> FILE conversion

    std::FILE * fp1 = std::fopen ("file", "r");
    TEST("fopen non-NULL FILE", fp1 != nullptr, true);
    int fd = vpl_fileno (fp1);
    TEST("fileno positive", fd >= 0, true);
    std::cout << "file number: " << fd << std::endl;
    std::FILE * fp2 = vpl_fdopen (fd, "r");
    TEST("fdopen non-NULL FILE", fp2 != nullptr, true);
    int data[1] = { 0 };
    std::size_t n = std::fread (data, sizeof(data[0]), 1, fp2);
    TEST("fread return value", n, 1);
    std::cout << "fread return value: " << n << std::endl
             << "data[0]: " << data[0] << std::endl;
    TEST("fileno/fdopen", data[0], 99);
    std::fclose(fp2);
    if (std::feof(fp1)) // Visual Studio 8 seems to link the two FILE ptrs and
      std::fclose(fp1); // aborts if fp2 (and hence fp1) is already closed.
    vpl_unlink ("file");
  }

  vpl_chdir(ROOT_PATH);
  vpl_rmdir(ROOT_PATH "/vpltest");

  std::string var("VPL_PUTENV_TEST");
  std::string value("GOOD");

  vpl_putenv((var + "=" + value).c_str());
  TEST("putenv", std::strcmp(std::getenv("VPL_PUTENV_TEST"), "GOOD"), 0);

  std::cout << "sleeping for 2 seconds..." << std::flush;
  vpl_usleep(2000000); // 2 seconds
  std::cout << "\b\b\b, done\n\n"
           << "Current PID: " << vpl_getpid() << std::endl;
}

TESTMAIN_ARGS(test_unistd);
