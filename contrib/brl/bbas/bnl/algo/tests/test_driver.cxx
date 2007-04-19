#include <testlib/testlib_register.h>


DECLARE(test_legendre_polynomial );
DECLARE( test_sparse_lm );

void
register_tests()
{

   REGISTER( test_legendre_polynomial );
   REGISTER( test_sparse_lm );
}

DEFINE_MAIN;
