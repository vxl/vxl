// This is contrib/brl/bbas/volm/desc/tests/test_volm_descriptor.cxx
//:
// \file
// \brief Tests for volumetric descriptor
// \author Yi Dong
// \date   May 28, 2013
//
#include <testlib/testlib_test.h>
#include <volm/desc/volm_desc_ex.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <vnl/vnl_random.h>

static void test_volm_desc_ex()
{
  // create a existance histogram from depth_map_scene
  vcl_string dm_file = "z:/projects/FINDER/test1/p1a_test1_20/p1a_test1_20.vsl";
  depth_map_scene_sptr dm = new depth_map_scene;
  vsl_b_ifstream dis(dm_file.c_str());
  dm->b_read(dis);
  dis.close();

  // define the radius
  vcl_vector<double> radius;
  radius.push_back(100);  radius.push_back(50);  radius.push_back(200);
  volm_desc_sptr desc = new volm_desc_ex(dm, radius);
  desc->print();

  // create a existance histogram from index
  float solid_angle = 2.0f, vmin = 2.0, dmax = 3000;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle, vmin, dmax);
  // construct depth_interval table for pass 1 matcher
  vcl_map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  vcl_vector<double> depth_interval;
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.begin();
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back(iter->first);

  vcl_string index_dst_file = "D:/work/Dropbox/FINDER/index/geoindex_beachgrass_combined_pa_5/geo_index_tile_3_node_-79.949219_32.650391_-79.948242_32.651367_index.bin";
  vcl_string index_combine_file = "D:/work/Dropbox/FINDER/index/geoindex_beachgrass_combined_pa_5/geo_index_tile_3_node_-79.949219_32.650391_-79.948242_32.651367_index_label_combined.bin";

  
  //vnl_random rand(9667566);
  //for (unsigned i = 0; i < 1024; i += 10) {
  //  index_dst[i] = (unsigned char)rand.lrand32(depth_interval.size());
  //  index_combined[i] = (unsigned char)rand.lrand32(254);
  //}

  //volm_desc_sptr desc_index = new volm_desc_ex(index_dst, index_combined, depth_interval, radius);
  //desc_index->print();
  //desc_index->visualize("./test_desc_ex_index.svg", 2);

  // binary IO test
  desc->visualize("./test_desc_ex_dms.svg", 2);
  vsl_b_ofstream os("./volm_desc_ex_dms.bin");
  desc->b_write(os);
  os.close();

  vsl_b_ifstream is("./volm_desc_ex_dms.bin");
  volm_desc_sptr desc_in = new volm_desc_ex();
  desc_in->b_read(is);
  is.close();
  desc_in->print();
  desc_in->visualize("./test_desc_ex_dms_in.svg", 2);

  bool is_same = desc_in->name() == desc->name();
  is_same = is_same && desc_in->nbins() == desc->nbins();

  TEST ("sky bin should exist", desc->count(385), 1);
  TEST ("water bin should exist", desc->count(1), 1);
  TEST ("building 2 should exist", desc->count(279), 1);
  TEST ("binary io is correct", is_same, true);


}

static void test_volm_descriptor()
{
  vcl_cout << "======================== test the existance histogram ====================== " << vcl_endl;
  test_volm_desc_ex();
  vcl_cout << "============================================================================ " << vcl_endl;

}

TESTMAIN( test_volm_descriptor );
