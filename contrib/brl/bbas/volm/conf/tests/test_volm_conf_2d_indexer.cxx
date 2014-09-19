#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_2d_indexer.h>
#include <volm/conf/volm_conf_indexer.h>
#include <vul/vul_timer.h>

static void test_volm_conf_2d_indexer()
{
  vul_timer t;
  // location index folder
  vcl_string loc_index_folder = "v:/p1a_related/p1a_index/gt_only/geoindex_p1a_wr2/";

  // land map index folder
  vcl_string land_map_folder = "v:/p1a_related/p1a_index/conf_index/land_map_index/wr6_h/";

  double radius = 200.0;  // in meter
  unsigned tile_id = 3;
  float buffer_capacity = 2.0; // GB
  vcl_string out_folder = "v:/p1a_related/p1a_index/gt_only/geoindex_conf_2d_h_radius_200/";
  // create index
  volm_conf_indexer_sptr indexer = new volm_conf_2d_indexer(radius, out_folder, land_map_folder, tile_id);

  TEST("load location database", indexer->load_loc_hypos(loc_index_folder, tile_id), true);

  vcl_cout << indexer->loc_leaves().size() << " leaves loaded from geo_folder: " << loc_index_folder << vcl_endl;
  unsigned n_locs = 0;
  for (unsigned i = 0; i < indexer->loc_leaves().size(); i++) {
    vcl_cout << " extent: " << indexer->loc_leaves()[i]->extent_ << ", number of locations: " << indexer->loc_leaves()[i]->hyps_->locs_.size() << vcl_endl;
    n_locs += indexer->loc_leaves()[i]->hyps_->locs_.size();
  }
  // start to index
  indexer->index(buffer_capacity, 0, 2);

  vcl_cout << "indexing " << n_locs << " consuming: " << t.all()/(1000.0*60.0) << " seconds" << vcl_endl;

  // load created index
  vcl_stringstream file_name_pre;
  file_name_pre << loc_index_folder << "/geo_index_tile_" << tile_id;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str()+".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  vcl_vector<volm_geo_index_node_sptr> loc_leaves;
  loc_leaves.clear();
  volm_geo_index::get_leaves_with_hyps(root, loc_leaves);
  vcl_stringstream index_file_name_pre;
  index_file_name_pre << out_folder << "conf_index_tile_" << tile_id;
  for (unsigned i = 0; i < loc_leaves.size(); i++)
  {
    volm_conf_buffer<volm_conf_object> ind(buffer_capacity);
    vcl_string bin_file_name = index_file_name_pre.str() + "_" + loc_leaves[i]->get_string() + "_" + indexer->get_index_name() + ".bin";
    TEST("initialize read", ind.initialize_read(bin_file_name), true);
    vgl_point_3d<double> h_pt;
    while ( loc_leaves[i]->hyps_->get_next(0, 1, h_pt))
    {
      vcl_stringstream kml_file;
      kml_file << vul_file::strip_extension(bin_file_name) << '_' << vcl_setprecision(6) << h_pt.x() << '_' << vcl_setprecision(6) << h_pt.y() << ".kml";
      vcl_cout << "kml_file: " << kml_file.str() << vcl_endl;
      vcl_vector<volm_conf_object> values;
      ind.get_next(values);
      volm_conf_object::write_to_kml(h_pt.x(), h_pt.y(), values, kml_file.str());
    }
  }
  return;
}

TESTMAIN( test_volm_conf_2d_indexer );