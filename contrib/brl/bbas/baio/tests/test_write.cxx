#include <iostream>
#include <fstream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <baio/baio.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <vpl/vpl.h>

static void test_write()
{
  const unsigned int buffSize = 1024*1024;
  std::string root_dir = testlib_root_dir();
  std::string test_file = root_dir + "/contrib/brl/bbas/baio/tests/test_file_w.txt";

  //load from file with blocking
  char* in_tester = new char[buffSize];
  char* out_tester = new char[buffSize];
  vnl_random rand;
  for (unsigned i=0;i<buffSize;++i)
    in_tester[i]=(char)rand.lrand32(-127,127);

  baio aio;
  aio.write(test_file,in_tester,buffSize);
  int num_flops = 0;
  while (aio.status() == BAIO_IN_PROGRESS) {
    num_flops += 4;
  }
  aio.close_file();
  std::cout<<"Number of flops performed during ASYNC write: "<<num_flops<<std::endl;

  unsigned int charCount = 0;
  std::string line;
  std::ifstream myfile(test_file.c_str(),std::ios::binary);
  if (myfile.is_open()) {
    while ( myfile.good() ) {
      getline (myfile,line);
      line += "\n";
      for (char i : line) {
        if (charCount+1 > buffSize)
          break;
        out_tester[charCount] = i;
        ++charCount;
      }
    }
    myfile.close();
  }

  //test asynchros-ness
  TEST("read is asynchronous/status write works", num_flops > 0, true);

  //Test same data read
  bool good = true;
  for (unsigned int i=0; i<buffSize; ++i) {
    if (out_tester[i] != in_tester[i]) {
      good = false;
      //std::cout<<(int) out_tester[i]<<"... "<<(int) in_tester[i]<<std::endl;
    }
  }
  TEST("data read matches data", good, true);

  //cleanup
  delete[] in_tester;
  delete[] out_tester;
  vpl_unlink(test_file.c_str());
}

TESTMAIN(test_write);
