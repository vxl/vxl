#include <testlib/testlib_register.h>

DECLARE( test_brec_update_changes_process );
DECLARE( test_brec_create_mog_image_process );

void register_tests()
{
  REGISTER( test_brec_update_changes_process );
  REGISTER( test_brec_create_mog_image_process );
}


DEFINE_MAIN;
