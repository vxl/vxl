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
#include "test_utils.h"

//: file manip includes
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <sys/stat.h>

//: for stats
#include <vul/vul_timer.h>

void test_cache()
{
  //test xml file 
  vcl_string root_dir  = testlib_root_dir();
  vcl_string test_dir = root_dir + "/contrib/brl/bseg/boxm2/tests/"; 
  
  //ensure blocks 0.0.0 - 1.1.1 are saved to disk
  for(int i=0; i<2; i++) {
    for(int j=0; j<2; j++) {
      for(int k=0; k<2; k++) {
        boxm2_block b(boxm2_block_id(i,j,k), boxm2_test_utils::construct_block_test_stream());  
        boxm2_sio_mgr::save_block(test_dir, &b); 
      }
    }
  }
    
  boxm2_dumb_cache cache;
  vcl_cout<<"TEesting dumb cache"<<vcl_endl;
  
  
  //delete .bin files
  vul_file::delete_file_glob(test_dir + "*.bin");

  //clean up .bin files
  //get all of the cam and image files, sort them
  vcl_string binglob = test_dir + "id*.bin";
  vul_file_iterator file_it(binglob.c_str());
  while (file_it) {
    vcl_cout<<file_it()<<vcl_endl;
    ++file_it; 
  }
  
}


TESTMAIN(test_cache);
