// First define testmain
#include <vsl/vsl_test.h>

// Test forward declarations.
#include <vsl/vsl_fwd.h>

#undef TESTMAIN
#define TESTMAIN(x)
#include <vsl/tests/test_polymorphic_io.cxx> // should come first
#include <vsl/tests/test_arbitrary_length_int_conversion.cxx>
#include <vsl/tests/test_binary_io.cxx>
#include <vsl/tests/test_clipon_polymorphic_io.cxx>
#include <vsl/tests/test_vector_io.cxx>
#include <vsl/tests/test_list_io.cxx>
#include <vsl/tests/test_deque_io.cxx>
#include <vsl/tests/test_complex_io.cxx>
#include <vsl/tests/test_string_io.cxx>
#include <vsl/tests/test_map_io.cxx>
#include <vsl/tests/test_set_io.cxx>
#include <vsl/tests/test_stack_io.cxx>
#include <vsl/tests/test_indent.cxx>

#undef TESTMAIN
#define TESTMAIN(x) int main()\
{ vsl_test_start(#x); x(); return vsl_test_summary(); }

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

TESTMAIN(run_test_vsl_io);
