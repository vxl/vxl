#include <set>
#include <iostream>
#include <testlib/testlib_test.h>

#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_query.h>
#include <bbas/volm/volm_query_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_spherical_region_query.h>
#include <volm/volm_spherical_region_index.h>
#include <volm/volm_vrml_io.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p0.h>
#include <volm/volm_loc_hyp.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_tile.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_locations_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <vpl/vpl.h>
#include <vul/vul_timer.h>

static void test_volm_matcher_p0()
{
  // TO DO -- creating desired indices for pass 0 matcher
  TEST("void test, require pass 0 indices creation", true, true);
}

TESTMAIN(test_volm_matcher_p0);
