#include <testlib/testlib_register.h>

DECLARE(test_smart_ptr_io);
DECLARE(test_bounding_box_io);
DECLARE(test_array_1d_io);
DECLARE(test_array_2d_io);
DECLARE(test_array_3d_io);
DECLARE(test_sparse_array_io);
DECLARE(test_triple_io);
DECLARE(golden_test_vbl_io);

void
register_tests()
{
  REGISTER(test_smart_ptr_io);
  REGISTER(test_bounding_box_io);
  REGISTER(test_array_1d_io);
  REGISTER(test_array_2d_io);
  REGISTER(test_array_3d_io);
  REGISTER(test_sparse_array_io);
  REGISTER(test_triple_io);
  REGISTER(golden_test_vbl_io);
}

DEFINE_MAIN;
