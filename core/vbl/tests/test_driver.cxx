#include "testlib/testlib_register.h"

DECLARE(vbl_test_bounding_box);
DECLARE(vbl_test_quadruple);
DECLARE(vbl_test_triple);
DECLARE(vbl_test_qsort);
DECLARE(vbl_test_array);
DECLARE(vbl_test_bit_array);
DECLARE(vbl_test_sparse_array_2d);
DECLARE(vbl_test_sparse_array_3d);
DECLARE(vbl_test_batch_multimap);
DECLARE(vbl_test_batch_compact_multimap);
DECLARE(vbl_test_smart_ptr);
DECLARE(vbl_test_ref_count);
DECLARE(vbl_test_scoped_ptr);
DECLARE(vbl_test_shared_pointer);
DECLARE(vbl_test_local_minima);
DECLARE(vbl_test_attributes);
DECLARE(vbl_test_disjoint_sets);

void
register_tests()
{
  REGISTER(vbl_test_bounding_box);
  REGISTER(vbl_test_quadruple);
  REGISTER(vbl_test_triple);
  REGISTER(vbl_test_qsort);
  REGISTER(vbl_test_array);
  REGISTER(vbl_test_bit_array);
  REGISTER(vbl_test_sparse_array_2d);
  REGISTER(vbl_test_sparse_array_3d);
  REGISTER(vbl_test_batch_multimap);
  REGISTER(vbl_test_batch_compact_multimap);
  REGISTER(vbl_test_smart_ptr);
  REGISTER(vbl_test_ref_count);
  REGISTER(vbl_test_scoped_ptr);
  REGISTER(vbl_test_shared_pointer);
  REGISTER(vbl_test_local_minima);
  REGISTER(vbl_test_attributes);
  REGISTER(vbl_test_disjoint_sets);
}

DEFINE_MAIN;
