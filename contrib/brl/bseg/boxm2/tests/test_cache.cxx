//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <iostream>
#include <cstdio>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

// for stats
#include <vul/vul_timer.h>
#include <vcl_sys/time.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//#include <unistd.h>

void test_cache()
{
  //init cache
  boxm2_scene_sptr scene;
  boxm2_test_utils::create_test_simple_scene(scene);
  boxm2_lru_cache::create(scene);
  boxm2_cache_sptr cache = boxm2_lru_cache::instance();
  //simulate a render
  std::cout<<"loading initial BLOCK and ALPHA"<<std::endl;
  vul_timer t;
  t.mark();
  boxm2_block* blk =  cache->get_block(scene, boxm2_block_id(0,0,0));


  TEST("checking block id",blk->block_id(), boxm2_block_id(0,0,0) );
}


TESTMAIN(test_cache);
