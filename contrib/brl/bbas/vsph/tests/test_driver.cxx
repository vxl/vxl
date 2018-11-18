#include <testlib/testlib_register.h>


DECLARE( test_view_sphere );
DECLARE( test_spherical_coord );
DECLARE( test_camera_bounds );
DECLARE( test_sph_geom );
DECLARE( test_unit_sphere );
DECLARE( test_sph_segment );
DECLARE( test_utils );
DECLARE( test_grid_index );
DECLARE( test_sph_cover );

void
register_tests()
{
  REGISTER( test_view_sphere );
  REGISTER( test_spherical_coord );
  REGISTER( test_camera_bounds );
  REGISTER( test_sph_geom );
  REGISTER( test_unit_sphere );
  REGISTER( test_sph_segment );
  REGISTER( test_utils );
  REGISTER( test_grid_index );
  REGISTER( test_sph_cover);
}

DEFINE_MAIN;
