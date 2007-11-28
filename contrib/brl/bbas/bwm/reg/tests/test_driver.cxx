#include <testlib/testlib_register.h>

DECLARE(test_champher);
DECLARE(test_matcher);



void
register_tests()
{
  REGISTER(test_champher);
  REGISTER(test_matcher);

}

DEFINE_MAIN;
