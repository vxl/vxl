#include <testlib/testlib_register.h>

DECLARE( test_memory_chunk_io );
DECLARE( test_image_view_io );

void
register_tests()
{
  REGISTER( test_memory_chunk_io );
  REGISTER( test_image_view_io );
}

DEFINE_MAIN;
