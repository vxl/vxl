#include <testlib/testlib_register.h>

DECLARE(test_vertex_2d);
DECLARE(test_zero_chain);
DECLARE(test_edge_2d);
DECLARE(test_one_chain);
DECLARE(test_face_2d);
DECLARE(test_two_chain);
DECLARE(test_block);
DECLARE(test_cycle_processor);
DECLARE(vtol_test_refcounting);
DECLARE(vtol_test_timing);

DECLARE(test_extract_topology);

void
register_tests()
{
  REGISTER(test_vertex_2d);
  REGISTER(test_zero_chain);
  REGISTER(test_edge_2d);
  REGISTER(test_one_chain);
  REGISTER(test_face_2d);
  REGISTER(test_two_chain);
  REGISTER(test_block);
  REGISTER(test_cycle_processor);
  REGISTER(vtol_test_refcounting);
  REGISTER(vtol_test_timing);

  REGISTER(test_extract_topology);
}

DEFINE_MAIN;
