#include <testlib/testlib_register.h>


DECLARE( test_spherical_container );
DECLARE( test_tile );
DECLARE( test_spherical_shell_container );
DECLARE( test_loc_hyp );
DECLARE( test_query );
DECLARE( test_index );
DECLARE( test_camera_space );
DECLARE( test_region_query );
DECLARE( test_io );


void
register_tests()
{
  REGISTER( test_spherical_container );
  REGISTER( test_tile );
  REGISTER( test_spherical_shell_container );
  REGISTER( test_loc_hyp );
  REGISTER( test_query );
  REGISTER( test_index );
  REGISTER( test_camera_space );
  REGISTER( test_region_query );
  REGISTER( test_io );

}

DEFINE_MAIN;

