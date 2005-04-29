#include <testlib/testlib_register.h>

DECLARE( test_bugl);
DECLARE( test_random_transform_set_2d);

void
register_tests()
{
  REGISTER ( test_bugl );
  REGISTER ( test_random_transform_set_2d );
}

DEFINE_MAIN;
