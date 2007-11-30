#include <testlib/testlib_register.h>

DECLARE(test_champher);
DECLARE(test_matcher);
DECLARE(test_processor);



void
register_tests()
{
  REGISTER(test_champher);
  REGISTER(test_matcher);
  REGISTER(test_processor);
}

DEFINE_MAIN;
