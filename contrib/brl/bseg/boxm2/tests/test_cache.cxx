//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

// for stats
#include <vul/vul_timer.h>
#include <vcl_sys/time.h>
#include <vcl_cstdio.h>
//#include <unistd.h>

void test_nn_cache()
{
  //init cache
  vcl_string scene_file=boxm2_test_utils::save_test_simple_scene();
  boxm2_scene scene(scene_file);
  boxm2_nn_cache cache(&scene);

  //simulate a render
  vcl_cout<<"loading initial BLOCK and ALPHA"<<vcl_endl;
  vul_timer t; t.mark();
  /* boxm2_block* blk = */ cache.get_block(boxm2_block_id(0,0,0));
  /* boxm2_data<BOXM2_ALPHA>* dat = */ cache.get_data<BOXM2_ALPHA>(boxm2_block_id(0,0,0));
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;

  ////do some fake processing
  t.mark();
  while (t.all() < 1000.0f) ;

  t.mark();
  /* blk = */ cache.get_block(boxm2_block_id(0,1,0));
  /* dat = */ cache.get_data<BOXM2_ALPHA>(boxm2_block_id(0,1,0));
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;

  ////do some fake processing
  t.mark();
  while (t.all() < 1000.0f) ;

  t.mark();
  /* blk = */ cache.get_block(boxm2_block_id(1,1,0));
  /* dat = */ cache.get_data<BOXM2_ALPHA>(boxm2_block_id(1,1,0));
  vcl_cout<<"cache return time: "<<t.all()<<vcl_endl;
}


void test_dumb_cache()
{
#if 0
  // init cache
  vcl_string scene_file=boxm2_test_utils::save_test_simple_scene();
  boxm2_scene scene(scene_file);

  boxm2_dumb_cache dcache(&scene);

  // check a few block values
  boxm2_block_sptr blk(dcache.get_block(boxm2_block_id(0,0,0)) );

  /*boxm2_data<BOXM2_ALPHA>*/  boxm2_data_base_sptr alph = dcache.get_data<BOXM2_ALPHA>(boxm2_block_id(0,0,0));
  boxm2_data_base_sptr mog = dcache.get_data<BOXM2_MOG3_GREY>(boxm2_block_id(0,0,0));
  vgl_vector_3d<unsigned> nums = blk->sub_block_num();
  // get another
  blk = dcache.get_block(boxm2_block_id(0,0,0));
  mog = dcache.get_data<BOXM2_MOG3_GREY>(boxm2_block_id(0,1,0));
  TEST("block carries same meta data:", nums==blk->sub_block_num(), true);
#endif
}


void test_cache()
{
  //ensure blocks 0.0.0 - 1.1.1 are saved to disk
  boxm2_test_utils::save_test_scene_to_disk();

  test_nn_cache();
  test_dumb_cache();

  //clean up .bin blocks created above
  boxm2_test_utils::delete_test_scene_from_disk();
}


TESTMAIN(test_cache);
