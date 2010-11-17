//:
// \file
// \author vishal JAin
// \date 05/17/2010
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block_id.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vnl/vnl_random.h>

void test_data()
{
  //test xml file
  vcl_string root_dir  = testlib_root_dir();

  const int array_size=10;
  float * farray=new float[array_size];
  vnl_random rand;
  for(unsigned i =0;i<array_size;i++)
    farray[i]=rand.drand32(0,1);

  unsigned char * buffer=reinterpret_cast<unsigned char *>(farray);
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float),boxm2_block_id());

  boxm2_array_1d<float> *  out_test_data=test_data.data();

  bool flag=true;
  for(unsigned i=0;i<array_size;i++)
    if(farray[i]!=(*out_test_data)[i])flag=false;

  TEST("boxm2_data successfully created", true, flag);
}



TESTMAIN(test_data);
