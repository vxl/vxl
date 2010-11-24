//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_dumb_cache.h>
#include <boxm2/boxm2_nn_cache.h>
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

void test_nn_cache()
{
  
  //init cache
  boxm2_nn_cache cache("", vgl_vector_3d<int>(2,2,2));
  
  vul_timer t; t.mark();
  boxm2_block* blk = cache.get_block(boxm2_block_id(0,0,0)); 
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;
  
  ////do some fake processing
  t.mark();
  while(t.all() < 1000.0f) ;
  
  t.mark(); 
  blk = cache.get_block(boxm2_block_id(0,1,0)); 
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;

  t.mark();
  blk = cache.get_block(boxm2_block_id(1,1,0)); 
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;
  
}


void test_dumb_cache()
{
   
  // init cache
  boxm2_dumb_cache dcache("");
  
  // check a few block values
  vgl_vector_3d<int> nums; 
  boxm2_block* blk = dcache.get_block(boxm2_block_id(0,0,1)); 
  boxm2_data<BOXM2_ALPHA>* alph = dcache.get_data<BOXM2_ALPHA>(boxm2_block_id(0,0,1));
  boxm2_data<BOXM2_MOG3_GREY>* mog = dcache.get_data<BOXM2_MOG3_GREY>(boxm2_block_id(0,0,1)); 
  nums = blk->sub_block_num(); 
  
  // get another
  blk = dcache.get_block(boxm2_block_id(0,1,0)); 
  alph = dcache.get_data<BOXM2_ALPHA>(boxm2_block_id(0,1,0)); 
  mog = dcache.get_data<BOXM2_MOG3_GREY>(boxm2_block_id(0,1,0)); 
  TEST("block carries same meta data: ", nums==blk->sub_block_num(), true); 
}


void test_cache()
{
  //ensure blocks 0.0.0 - 1.1.1 are saved to disk
  //boxm2_test_utils::save_test_scene_to_disk();
  
  //test_dumb_cache(); 
  test_nn_cache();
}


TESTMAIN(test_cache);
