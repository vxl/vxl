#include <vcl_fstream.h>

#include <vpl/vpl.h>
#include <sys/types.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>

#define Assert(x) do { if (x) vcl_cout << "test PASSED\n"; else vcl_cout << "test FAILED [" #x "]\n"; } while (0)

#ifdef VCL_WIN32
#define ROOT_PATH "C:/TEMP"
#else
#define ROOT_PATH "/tmp"
#endif

int main(int, char **)
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
    Assert(s == 1234);
  }
  vpl_unlink("file");
  {
    vcl_ifstream f(ROOT_PATH "/vpltest/file");
    Assert(!f.good());
  }

  vpl_chdir(ROOT_PATH);
  vpl_rmdir(ROOT_PATH "/vpltest");

  vpl_usleep(5000000); // 5 seconds

  vpl_putenv("VPL_PUTENV_TEST=GOOD");
  Assert(vcl_strcmp(vcl_getenv("VPL_PUTENV_TEST"), "GOOD") == 0);

  return 0;
}
