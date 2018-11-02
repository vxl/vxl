//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/basic/boxm2_array_2d.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>

static void test_one_d()
{
  //test 1d array
  char* buff = new char[100];
  for(int i=0; i<100; i++) buff[i] = (char) i;
  boxm2_array_1d<char> charArr(100, buff);

  int index = 0;
  boxm2_array_1d<char>::iterator iter;
  for(iter = charArr.begin(); iter != charArr.end(); ++iter) {
    if( charArr[index] != (*iter) ) {
      TEST("boxm2_array_1d failed iterator/indexing", false, true);
      return;
    }
    index++;
  }
  TEST("boxm2_array_1d passed iterator/indexing", true, true);

  //clean up original memory
  if(buff) delete[] buff;
}

static void test_two_d()
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  auto* buff = new unsigned char[200 * 16];
  for(int i=0; i<200*16; i++) buff[i] = 0;
  for(int i=0; i<200; i++) buff[16*i] = (unsigned char) i;
  auto* cbuff = (uchar16*) (buff);
  boxm2_array_2d<uchar16> intArr(20, 10, cbuff);

  boxm2_array_2d<uchar16>::iterator iter = intArr.begin();
  for(int i=0; i<20; i++) {
    for(int j=0; j<10; j++) {
      if(intArr[i][j] != (*iter)) {
        TEST("boxm2_array_2d failed iterator/indexing", false, true);
        return;
      }
      iter++;
    }
  }
  TEST("boxm2_array_2d passed iterator/indexing", true, true);

  //clean up original memory
  if(buff) delete[] buff;
}

static void test_three_d()
{
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  //make 192x192x64 uchar16 block
  auto* buff  = new unsigned char[192 * 192 * 64 * 16];
  for(int i=0; i<192 * 192 * 64 * 16; i++) buff[i] = 0;
  auto*       cbuff = (uchar16*) buff;
  for(int i=0; i<192; i++) {
    for(int j=0; j<192; j++) {
      for(int k=0; k<64; k++) {
        int index = k + (j + i*192)*64;
        cbuff[index][0] = i;
        cbuff[index][1] = j;
        cbuff[index][2] = k;
      }
    }
  }

  ////now create high level array object
  boxm2_array_3d<uchar16> three_d(192, 192, 64, cbuff);
  boxm2_array_3d<uchar16>::iterator iter = three_d.begin();
  for(int i=0; i<192; i++) {
    for(int j=0; j<192; j++) {
      for(int k=0; k<64; k++) {
        bool good = three_d[i][j][k] == three_d(i,j,k);
        good = good && ( three_d(i,j,k) == (*iter) );
        good = good && (*iter)[0] == i && (*iter)[1] == j && (*iter)[2] == k;
        if(!good) {
          TEST("boxm2_array_3d failed iterator/indexing", false, true);
          return;
        }
        iter++;
      }
    }
  }
  TEST("boxm2_array_3d passed iterator/indexing", true, true);

  if(buff) delete[] buff;
}

void test_wrappers()
{
  //TEST WRAPPERS
  test_one_d();
  test_two_d();
  test_three_d();
}


TESTMAIN( test_wrappers );
