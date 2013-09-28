#include <testlib/testlib_test.h>
#include <boxm2/volm/desc/boxm2_volm_desc_ex_indexer.h>
#include <volm/desc/volm_desc_indexer.h>
#include <bvrml/bvrml_write.h>

static void test_ex_indexer()
{
  vcl_string index_folder = "z:/projects/FINDER/index/geoindex_zone_17_inc_2.0_nh_100_pa_5/";
  vcl_string out_index_folder = "d:/work/find/desc_index/radius_100_200_500_1000_inc_2.0_nh_100_pa_5/";
  vcl_string geo_hypo_folder = "z:/projects/FINDER/index/geoindex_zone_17_inc_2.0_nh_100/";
  vcl_vector<double> radius;
  radius.push_back(100);  radius.push_back(500);  radius.push_back(1000);  radius.push_back(200);
  unsigned index_layer_size = 1176;
  float ind_buff = 1.0f;

  // create depth_interval
  float solid_angle = 2;
  float vmin = 2;
  float dmax = 3000;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle, vmin, dmax);
  vcl_map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.begin();
  vcl_vector<double> depth_interval;
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back((double)iter->first);

  volm_desc_indexer_sptr ex_indexer = new volm_desc_ex_indexer(index_folder, out_index_folder, radius, depth_interval, index_layer_size, ind_buff);

  vcl_vector<unsigned> tile_ids;
  tile_ids.push_back(0), tile_ids.push_back(3);
  for (unsigned i = 0; i < tile_ids.size(); i++) {
    ex_indexer->load_tile_hypos(geo_hypo_folder, tile_ids[i]);
    ex_indexer->index(1.0f, -1, 1000);
    vcl_cout << " tile " << tile_ids[i] << " finished" << vcl_endl;
  }

}

TESTMAIN( test_ex_indexer );