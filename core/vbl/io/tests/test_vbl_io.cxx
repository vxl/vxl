 
// First define testmain

#include <vbl/vbl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)
#include <vbl/io/tests/test_smart_ptr_io.cxx>
#include <vbl/io/tests/test_bounding_box_io.cxx>
#include <vbl/io/tests/test_array_1d_io.cxx>
#include <vbl/io/tests/test_array_2d_io.cxx>
#include <vbl/io/tests/test_array_3d_io.cxx>
#include <vbl/io/tests/test_user_info_io.cxx>
#include <vbl/io/tests/test_sparse_array_io.cxx>
#include <vbl/io/tests/golden_test_vbl_io.cxx>



#undef TESTMAIN
#define TESTMAIN(x) int main() { \
vbl_test_start(#x); x(); return vbl_test_summary(); }

void run_test_vbl_io()
{
  test_smart_ptr_io();
  test_bounding_box_double_io();
  test_array_1d_io();
  test_array_2d_io();
  test_array_3d_io();
  test_user_info_io();
  test_sparse_array_io();
  golden_test_vbl_io();
}


TESTMAIN(run_test_vbl_io);
