#include <testlib/testlib_register.h>

DECLARE( test_vil );
DECLARE( test_load_gen );
DECLARE( test_assign );
DECLARE( test_colour_space );
DECLARE( test_copy );
DECLARE( test_file_format_read );
DECLARE( test_flipud );
DECLARE( test_interpolate );
DECLARE( test_load );
DECLARE( test_memory_image );
DECLARE( test_memory_image_of );
DECLARE( test_resample );
DECLARE( test_save_load_image );
DECLARE( test_write_endian );

void
register_tests()
{
  REGISTER( test_vil );
  REGISTER( test_load_gen );
  REGISTER( test_assign );
  REGISTER( test_colour_space );
  REGISTER( test_copy );
  REGISTER( test_file_format_read );
  REGISTER( test_flipud );
  REGISTER( test_interpolate );
  REGISTER( test_load );
  REGISTER( test_memory_image );
  REGISTER( test_memory_image_of );
  REGISTER( test_resample );
  REGISTER( test_save_load_image );
  REGISTER( test_write_endian );
}

DEFINE_MAIN;
