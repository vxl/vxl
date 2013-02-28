#include <testlib/testlib_test.h>
#include <volm/volm_spherical_region_index.h>

#include <volm/volm_io.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_save.h>
#include <bpgl/bpgl_camera_utils.h>

static void test_region_index()
{
  // input files
  vcl_string sph_index_file = "Z:\\projects\\FINDER\\index\\geoindex_zone_17_inc_2_nh_100_pa_2\\geo_index_tile_3_node_-79.968750_32.625000_-79.937500_32.656250_index_label_orientation_hyp_32.6507_-79.949_3.07477.txt";
  vcl_string unit_sph_file = "e:/data/Finder/unit_sphere_2_75_105.vsl";
  volm_spherical_region_index region_index(sph_index_file,unit_sph_file);
  region_index.print(vcl_cout);

}


TESTMAIN(test_region_index);
