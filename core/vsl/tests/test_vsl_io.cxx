#include <testlib/testlib_test.h>

// Test forward declarations.
#include <vsl/vsl_fwd.h>

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)
#include "test_polymorphic_io.cxx" // should come first
#include "test_arbitrary_length_int_conversion.cxx"
#include "test_binary_io.cxx"
#include "test_clipon_polymorphic_io.cxx"
#include "test_vector_io.cxx"
#include "test_list_io.cxx"
#include "test_deque_io.cxx"
#include "test_complex_io.cxx"
#include "test_string_io.cxx"
#include "test_map_io.cxx"
#include "test_set_io.cxx"
#include "test_stack_io.cxx"
#include "test_indent.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main()\
{ testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_vsl_io()
{
  test_arbitrary_length_int_conversion();
  test_binary_io();
  test_polymorphic_io();
  test_clipon_polymorphic_io();
  test_vector_io();
  test_list_io();
  test_deque_io();
  test_complex_io();
  test_string_io();
  test_map_io();
  test_set_io();
  test_stack_io();
  test_indent();
}

TESTLIB_DEFINE_MAIN(run_test_vsl_io);
