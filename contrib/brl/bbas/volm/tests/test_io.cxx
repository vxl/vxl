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

#if 0
  vcl_string weight_file = "z:/projects/FINDER/test1/p1a_test1_83/weight_param.txt";

  vcl_vector<volm_weight> weights;

  if (vul_file::exists(weight_file)) {
    vcl_cout << " reading weight from file ---> " << weight_file << vcl_endl;
    volm_weight::read_weight(weights, weight_file);
  }
  else {
    // load the depth_map_scene and create a set of equal weight parameters, given the number of objects defined in the depth_map_scene
    vcl_string dms_bin = "Z://projects//FINDER//test1//p1a_test1_06//p1a_test1_06.vsl";
    vsl_b_ifstream dms_is(dms_bin.c_str());
    depth_map_scene_sptr dm = new depth_map_scene;
    dm->b_read(dms_is);
    dms_is.close();
    volm_weight::equal_weight(weights, dm);
  }

  float sum_obj = 0;
  for (vcl_vector<volm_weight>::iterator vit = weights.begin(); vit != weights.end(); ++vit) {
    vcl_cout << ' ' << vit->w_typ_ << ' ' << vit->w_ori_ << ' ' << vit->w_lnd_ << ' ' << vit->w_dst_ << ' ' << vit->w_ord_ << ' ' << vit->w_obj_ << vcl_endl;
    float sum_att = vit->w_ori_ + vit->w_lnd_ + vit->w_dst_ + vit->w_ord_;
    vcl_string test_str = "summation of attributes for " + vit->w_typ_;
    sum_obj += vit->w_obj_;
    TEST_NEAR(test_str.c_str(), sum_att, 1, 0.001);
  }
  TEST_NEAR("summation of all object weight", sum_obj, 1, 0.001);
#endif
#if 0
  vcl_cout << " LAND_ID\n";

  vcl_vector<vcl_string> land_types;

  vcl_map<unsigned char, vcl_vector<vcl_string> > temp;
  vcl_map<int, volm_attributes >::iterator mit = volm_label_table::land_id.begin();
  unsigned cnt = 0;
  for (; mit != volm_label_table::land_id.end(); ++mit) {
    temp[mit->second.id_].push_back(mit->second.name_);
  }
  vcl_map<unsigned char, vcl_vector<vcl_string> >::iterator it = temp.begin();
  for (; it != temp.end(); ++it) {
    vcl_string land_name;
    for ( vcl_vector<vcl_string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
      land_name += (*vit);
      if (vit != it->second.end()-1) land_name += " OR ";
    }
    land_types.push_back(land_name);
    vcl_cout << "land id = " << (int)it->first << " string = " << land_name << vcl_endl;
  }
#endif
}

TESTMAIN(test_io);
