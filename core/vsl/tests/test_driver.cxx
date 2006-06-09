#include <testlib/testlib_register.h>

DECLARE(test_indent);
DECLARE(test_binary_io);
DECLARE(test_arbitrary_length_int_conversion);
DECLARE(test_clipon_polymorphic_io);
DECLARE(test_complex_io);
DECLARE(test_deque_io);
DECLARE(test_list_io);
DECLARE(test_map_io);
DECLARE(test_polymorphic_io);
DECLARE(test_set_io);
DECLARE(test_stack_io);
DECLARE(test_string_io);
DECLARE(test_vector_io);
DECLARE(test_vlarge_block_io);

void
register_tests()
{
  REGISTER(test_indent);
  REGISTER(test_binary_io);
  REGISTER(test_arbitrary_length_int_conversion);
  REGISTER(test_clipon_polymorphic_io);
  REGISTER(test_complex_io);
  REGISTER(test_deque_io);
  REGISTER(test_list_io);
  REGISTER(test_map_io);
  REGISTER(test_polymorphic_io);
  REGISTER(test_set_io);
  REGISTER(test_stack_io);
  REGISTER(test_string_io);
  REGISTER(test_vector_io);
  REGISTER(test_vlarge_block_io);
}

DEFINE_MAIN;
