#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_tile.h>
#include <bbas/volm/volm_geo_index.h>
#include <bbas/volm/volm_io.h>
#include <bkml/bkml_parser.h>

static void test_io()
{
  vcl_string path =  "C:/Users/mundy/VisionSystems/Finder/VolumetricQuery/vol_indices/geo_index_tile_3_node_-79.968750_32.656250_-79.937500_32.687500_index_label_orientation_hyp_32.6574_-79.9432_4.24473.txt";
  vcl_vector<unsigned char> data;
  bool good = volm_io::read_ray_index_data(path, data);
}

TESTMAIN(test_io);
