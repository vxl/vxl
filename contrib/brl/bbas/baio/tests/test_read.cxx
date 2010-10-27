#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <baio/baio.h>
#include <vcl_iostream.h> //for vcl_cout
#include <vcl_fstream.h>  //for file open
#include <stdlib.h>  //includes malloc


static void test_read_helper(char* buffer)
{
  //tests to see if char* persists
  //load from file asynch
  int buffSize = 1024;
  vcl_string root_dir = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bbas/baio/tests/test_file.txt"; 
  baio aio;
  aio.read(test_file, buffer, buffSize);
  int numFlops = 0;
  while(aio.status() == BAIO_IN_PROGRESS) {
    numFlops += 4;
  }
  
  buffer = aio.buffer();
}

static void test_read()
{
  int buffSize = 1024;
  vcl_string root_dir = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bbas/baio/tests/test_file.txt"; 

  //load from file with blocking
  char tester[buffSize];
  int charCount = 0;
  vcl_string line; 
  vcl_ifstream myfile(test_file.c_str());
  if (myfile.is_open()) {
    while ( myfile.good() ) {
      getline (myfile,line);
      line += "\n";
      for(int i=0 ; i<line.length(); i++) {
        if(charCount > buffSize-1)
          break;
        tester[charCount] = line[i];
        charCount++;
      }
    }
    myfile.close();
  }
  
  //load from file asynch
  char* aio_buff = new char[buffSize];
  baio aio;
  aio.read(test_file, aio_buff, buffSize);
  int numFlops = 0;
  while(aio.status() == BAIO_IN_PROGRESS) {
    numFlops += 4;
  }
  vcl_cout<<"Number of flops performed during ASYNC read: "<<numFlops<<vcl_endl;
  
  //test asynchros-ness
  TEST("read is asynchronous/status works ", true, numFlops > 0);

  //Test same data read  
  bool good = true;
  for(int i=0; i<buffSize; i++) {
    if(aio_buff[i] != tester[i]) {
      good = false;
      vcl_cout<<(int) aio_buff[i]<<"... "<<(int) tester[i]<<vcl_endl;
    }
  }
  TEST("data read matches synchronous data ", true, good);
  
  //cleanup
  if(aio_buff) delete aio_buff;
  
}

TESTMAIN(test_read);
