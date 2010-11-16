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

void test_io()
{
  //test xml file
  vcl_string root_dir  = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bseg/boxm2/tests/test.xml";

  //1. construct a dummy block byte stream
  char bsize[1000];
  long size = 1000;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  vcl_memcpy(bsize, &size, sizeof(long));
  vcl_memcpy(bsize+8, &init_level, sizeof(int));
  vcl_memcpy(bsize+12, &max_level, sizeof(int));
  vcl_memcpy(bsize+16, &max_mb, sizeof(int));
  boxm2_block test_block(bsize);
  test_block.set_byte_count(50);
  boxm2_sio_mgr::save_block(test_file, &test_block);

  //2. load block
  boxm2_block* test_copy = boxm2_sio_mgr::load_block(test_file, boxm2_block_id(0,0,0));
  vcl_cout<<"   init level: "<<test_copy->init_level()
          <<"   max level : "<<test_copy->max_level()
          <<"   max_mb    : "<<test_copy->max_mb()<<vcl_endl;

  //3. test
}


TESTMAIN(test_io);
