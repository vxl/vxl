// This is gel/vifa/tests/test_driver.cxx
#include <testlib/testlib_register.h>

DECLARE(test_coll_lines_params);
DECLARE(test_group_pgram_params);
DECLARE(test_norm_params);
DECLARE(test_parallel_params);
DECLARE(test_int_face_attr_common_params);
DECLARE(test_image_histogram);
DECLARE(test_region_proc);
DECLARE(test_int_face_attr);
DECLARE(test_int_faces_attr);
DECLARE(test_int_faces_adj_attr);

void
register_tests()
{
  REGISTER(test_coll_lines_params);
  REGISTER(test_group_pgram_params);
  REGISTER(test_norm_params);
  REGISTER(test_parallel_params);
  REGISTER(test_int_face_attr_common_params);
  REGISTER(test_image_histogram);
  REGISTER(test_region_proc);
  REGISTER(test_int_face_attr);
  REGISTER(test_int_faces_attr);
  REGISTER(test_int_faces_adj_attr);
}

DEFINE_MAIN;
