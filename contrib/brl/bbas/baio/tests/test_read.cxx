#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <baio/baio.h>
#include <vcl_iostream.h> //for vcl_cout
#include <vcl_fstream.h>  //for file open
#include <vcl_cstdlib.h>  //includes malloc
#include <vnl/vnl_random.h>
#include <vpl/vpl.h>


static void test_read()
{
  //create file to read
  const unsigned int buffSize = 1024*1024;
  vcl_string root_dir = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bbas/baio/tests/test_file_r.txt";
  char* tester = new char[buffSize];
  vnl_random rand;
  for (unsigned int i=0;i<buffSize;++i)
    tester[i]=(char)rand.lrand32(-127,127);

  //write to file with blocking
  vcl_ofstream outFile(test_file.c_str(), vcl_ios::out | vcl_ios::binary);
  outFile.write(tester, buffSize);
  outFile.close();

  //load from file asynch
  char* aio_buff = new char[buffSize];
  baio aio;
  aio.read(test_file, aio_buff, buffSize);
  int numFlops = 0;
  while (aio.status() == BAIO_IN_PROGRESS) numFlops += 4;
  aio.close_file();
  vcl_cout<<"Number of flops performed during ASYNC read: "<<numFlops<<vcl_endl;

  //test asynchros-ness
  TEST("read is asynchronous/status works", true, numFlops > 0);

  //Test same data read
  bool good = true;
  for (unsigned int i=0; i<buffSize; ++i) {
    if (aio_buff[i] != tester[i]) {
      good = false;
      vcl_cout<<(int) aio_buff[i]<<"... "<<(int) tester[i]<<vcl_endl;
    }
  }
  TEST("data read matches synchronous data ", true, good);

  //cleanup
  delete[] tester;
  delete[] aio_buff;
  vpl_unlink(test_file.c_str());
}

TESTMAIN(test_read);
