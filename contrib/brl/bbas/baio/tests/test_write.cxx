#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <baio/baio.h>
#include <vcl_iostream.h> //for vcl_cout
#include <vcl_fstream.h>  //for file open
#include <vcl_cstdlib.h>  //includes malloc
#include <vnl/vnl_random.h>
#include <vpl/vpl.h>

static void test_write()
{
  const unsigned int buffSize = 1024*1024;
  vcl_string root_dir = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bbas/baio/tests/test_file_w.txt";

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
  vcl_cout<<"Number of flops performed during ASYNC write: "<<num_flops<<vcl_endl;

  unsigned int charCount = 0;
  vcl_string line;
  vcl_ifstream myfile(test_file.c_str(),vcl_ios::binary);
  if (myfile.is_open()) {
    while ( myfile.good() ) {
      getline (myfile,line);
      line += "\n";
      for (unsigned int i=0 ; i<line.length(); ++i) {
        if (charCount+1 > buffSize)
          break;
        out_tester[charCount] = line[i];
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
      //vcl_cout<<(int) out_tester[i]<<"... "<<(int) in_tester[i]<<vcl_endl;
    }
  }
  TEST("data read matches data", good, true);
  
  //cleanup
  delete[] in_tester;
  delete[] out_tester;
  vpl_unlink(test_file.c_str());
}

TESTMAIN(test_write);
