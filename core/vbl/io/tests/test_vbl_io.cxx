 
// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)
#include "test_smart_ptr_io.cxx"
#include "test_bounding_box_io.cxx"
#include "test_array_1d_io.cxx"
#include "test_array_2d_io.cxx"
#include "test_array_3d_io.cxx"
#include "test_sparse_array_io.cxx"
#include "golden_test_vbl_io.cxx"


#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() { \
testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_vbl_io()
{
  test_smart_ptr_io();
  test_bounding_box_double_io();
  test_array_1d_io();
  test_array_2d_io();
  test_array_3d_io();
  test_sparse_array_io();
  golden_test_vbl_io();
}


TESTLIB_DEFINE_MAIN(run_test_vbl_io);
