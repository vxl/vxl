#include <testlib/testlib_register.h>

DECLARE( test_volm_conf_object );
DECLARE( test_volm_conf_query  );
DECLARE( test_volm_conf_score  );
DECLARE( test_volm_conf_land_map_indexer );
DECLARE( test_volm_conf_buffer );
DECLARE( test_volm_conf_2d_indexer );
void
register_tests()
{
  REGISTER( test_volm_conf_object );
  REGISTER( test_volm_conf_query  );
  REGISTER( test_volm_conf_score  );
  REGISTER( test_volm_conf_land_map_indexer );
  REGISTER( test_volm_conf_buffer );
  REGISTER( test_volm_conf_2d_indexer );
}

DEFINE_MAIN;