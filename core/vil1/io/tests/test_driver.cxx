#include <testlib/testlib_register.h>

DECLARE( test_memory_image_format_io );
DECLARE( test_rgb_io );
DECLARE( test_rgba_io );
DECLARE( test_memory_image_impl_io );
DECLARE( test_memory_image_of_io );
DECLARE( golden_test_vil_io );
DECLARE( test_include );

void
register_tests()
{
  REGISTER( test_memory_image_format_io );
  REGISTER( test_rgb_io );
  REGISTER( test_rgba_io );
  REGISTER( test_memory_image_impl_io );
  REGISTER( test_memory_image_of_io );
  REGISTER( golden_test_vil_io );
  REGISTER( test_include );
}
