//:
// \file
// \author Vishal Jain
// \date June 9, 2013
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_lru_cache2.h>
#include <boxm2/io/boxm2_cache2.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

// for stats
#include <vul/vul_timer.h>
#include <vcl_sys/time.h>
#include <vcl_cstdio.h>
//#include <unistd.h>

void test_lru_cache2()
{
  //init cache
  vcl_string scene_file1=boxm2_test_utils::save_test_simple_scene("test1.xml");
  vcl_string scene_file2=boxm2_test_utils::save_test_simple_scene("test2.xml");
  boxm2_scene_sptr scene1 = new boxm2_scene(scene_file1);
  boxm2_scene_sptr scene2 = new boxm2_scene(scene_file2);
  boxm2_lru_cache2::create(scene1);
  boxm2_cache2_sptr cache = boxm2_cache2::instance();
  //simulate a render
  vcl_cout<<"loading initial BLOCK and ALPHA"<<vcl_endl;
  vul_timer t; 
  t.mark();
  boxm2_block* blk1 =  cache->get_block(scene1, boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_ALPHA>* dat1 =  cache->get_data<BOXM2_ALPHA>(scene1,boxm2_block_id(0,0,0));
  vcl_cout<<"cache (scene1) return time: "<<t.all()<<vcl_endl;

  //////do some fake processing
  //t.mark();
  //while (t.all() < 1000.0f) ;

  t.mark();
  boxm2_block* blk2 =  cache->get_block(scene2, boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_ALPHA>* dat2 =  cache->get_data<BOXM2_ALPHA>(scene2,boxm2_block_id(0,0,0));
  vcl_cout<<"cache (scene2) return time: "<<t.all()<<vcl_endl;

  cache->clear_cache();
  //////do some fake processing
  //t.mark();
  //while (t.all() < 1000.0f) ;

  //t.mark();
  ///* blk = */ cache.get_block(boxm2_block_id(1,1,0));
  ///* dat = */ cache.get_data<BOXM2_ALPHA>(boxm2_block_id(1,1,0));
  //vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;
}




void test_cache2()
{
  test_lru_cache2();
}


TESTMAIN(test_cache2);
