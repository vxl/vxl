#include "testlib/testlib_register.h"

DECLARE(test_f_utils_io);
DECLARE(test_match_graph_io);
DECLARE(test_match_tree_io);
DECLARE(test_match_utils_io);

void
register_tests()
{
  REGISTER(test_f_utils_io);
  REGISTER(test_match_graph_io);
  REGISTER(test_match_tree_io);
  REGISTER(test_match_utils_io);
}

DEFINE_MAIN;
