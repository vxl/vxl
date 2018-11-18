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
DECLARE( test_region_index );
DECLARE( test_spherical_region );
DECLARE( test_geo_index2 );
DECLARE( test_osm_parser );
DECLARE( test_category_io );
DECLARE( test_osm_object );
DECLARE( test_candidate_region_parser );
DECLARE( test_utils );
DECLARE( test_overlapping_resources );
DECLARE( test_intersecting_resources );
DECLARE( test_compute_intersection );

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
  REGISTER( test_region_index );
  REGISTER( test_spherical_region );
  REGISTER( test_geo_index2 );
  REGISTER( test_osm_parser );
  REGISTER( test_category_io );
  REGISTER( test_osm_object );
  REGISTER( test_candidate_region_parser );
  REGISTER( test_utils );
  REGISTER( test_overlapping_resources );
  REGISTER( test_intersecting_resources );
  REGISTER( test_compute_intersection );
}

DEFINE_MAIN;
