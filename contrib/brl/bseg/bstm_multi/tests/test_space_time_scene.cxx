//:
// \file Test
// \author Raphael Kargon
// \date 29-Jul-2017

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <bstm/bstm_block.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/space_time_scene.h>

void test_space_time_scene()
{
  // Test template instantiation
  space_time_scene<bstm_multi_block> scene_multi;
  space_time_scene<bstm_block> scene_bstm;
}


TESTMAIN( test_space_time_scene );
