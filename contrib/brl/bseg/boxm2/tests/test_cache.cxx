//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_dumb_cache.h>
#include <boxm2/boxm2_sio_mgr.h>
#include <boxm2/boxm2_block_id.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <sys/stat.h>

void test_cache()
{
  //test xml file 
  vcl_string root_dir  = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bseg/boxm2/tests/test.xml"; 


  boxm2_dumb_cache cache;
  vcl_cout<<"TEesting dumb cache"<<vcl_endl;
}


TESTMAIN(test_cache);
