#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <bbas/volm/volm_tile.h>
#include <bbas/volm/volm_geo_index.h>
#include <bbas/volm/volm_io.h>
#include <volm/volm_category_io.h>
#include <bkml/bkml_parser.h>
#if 0 // used in commented-out part
#include <vul/vul_file.h>
#endif

static void test_io()
{
  // test the volm_fallback_category
  vcl_map<unsigned char, vcl_vector<unsigned char> >::iterator mit = volm_fallback_label::fallback_id.begin();
  vcl_map<unsigned char, vcl_vector<float> >::iterator mit_w = volm_fallback_label::fallback_weight.begin();

  for (; mit != volm_fallback_label::fallback_id.end(); ++mit) {
    vcl_cout << (int)mit->first << '(' << volm_osm_category_io::volm_land_table[mit->first].name_ << ") ---> ";
    volm_fallback_label::print_id(mit->first);
    //for (vcl_vector<unsigned char>::iterator vit = mit->second.begin(); vit != mit->second.end(); ++vit)
    //  vcl_cout << volm_label_table::land_string(*vit) << ", ";
    vcl_cout << vcl_setw(10) << vcl_setfill(' ') << " ------ ";
    volm_fallback_label::print_wgt(mit->first);
    for (vcl_vector<unsigned char>::iterator vit = mit->second.begin(); vit != mit->second.end(); ++vit)
      vcl_cout << vcl_setprecision(3) << (int)*vit << ' ';
    vcl_cout << '\n' << vcl_endl;
    ++mit_w;
  }

  TEST("number of fallback_label and weight", volm_fallback_label::fallback_id.size(), volm_fallback_label::fallback_weight.size());

}

TESTMAIN(test_io);
