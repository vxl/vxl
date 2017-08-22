#include <testlib/testlib_register.h>

DECLARE(test_bstm_multi_block_metadata);
DECLARE(test_bstm_multi_tree_util);
DECLARE(test_space_time_scene_parser);
DECLARE(test_space_time_scene);

void register_tests() {
  REGISTER(test_bstm_multi_block_metadata);
  REGISTER(test_bstm_multi_tree_util);
  REGISTER(test_space_time_scene_parser);
  REGISTER(test_space_time_scene);
}

DEFINE_MAIN;
