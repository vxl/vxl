#include <vcl_fstream.h>

#include <vpl/vpl_unistd.h>

#define Assert(x) do { if (x) cerr << "test PASSED\n"; else cerr << "test FAILED [" #x "]\n"; } while (0)


int main(int, char **)
{
  vpl_mkdir("/tmp/vpltest", 0777);
  vpl_chdir("/tmp/vpltest");

  {
    ofstream f("file");
    f << 1234;
  }
  {
    ifstream f("/tmp/vpltest/file");
    int s;
    f >> s;
    Assert(s == 1234);
  }
  vpl_unlink("file");
  {
    ifstream f("/tmp/vpltest/file");
    Assert(!f.good());
  }

  vpl_chdir("/tmp");
  vpl_rmdir("/tmp/vpltest");

  return 0;
}
