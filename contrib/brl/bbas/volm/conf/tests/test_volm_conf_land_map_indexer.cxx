#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_land_map_indexer.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_io.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <bsol/bsol_intrinsic_curve_2d.h>
#include <vul/vul_file.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_algorithm.h>

#include <vgl/vgl_polygon.h>
#include <bkml/bkml_parser.h>

static void test_volm_conf_land_map_indexer()
{
  // location index folder
  vcl_string loc_index_folder = "v:/p1a_related/p1a_index/pier_only/geoindex_p1a_wr2/";

  // land mpa index folder
  vcl_string land_map_folder = "v:/p1a_related/p1a_index/conf_index/land_map_index/wr6/";

  double radius = 3000.0;  // in meter
  unsigned tile_id = 3;
  double min_size = 0.0078125;
  
  vcl_string poly_roi = "v:/p1a_related/Coast-WR.kml";
  vgl_polygon<double> roi_poly = bkml_parser::parse_polygon(poly_roi);
  vcl_vector<volm_tile> tiles;
  volm_tile::generate_tiles(6, tiles);
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<volm_conf_land_map_indexer_sptr>(tiles[tile_id].bbox_double(), min_size, roi_poly);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);

  // write the tree
  vcl_string out_folder = "./";
  unsigned tree_depth = bvgl_2d_geo_index::depth(root);
  vcl_stringstream tree_txt;
  tree_txt << out_folder << "/2d_geo_index_tile_" << tile_id << ".txt";
  vcl_stringstream tree_kml;
  tree_kml << out_folder << "/2d_geo_index_tile_" << tile_id << "_depth_" << tree_depth << ".kml";
  bvgl_2d_geo_index::write(root, tree_txt.str(), min_size);
  bvgl_2d_geo_index::write_to_kml(root, tree_depth, tree_kml.str(), "land_map_indexer");


  // check loaded tree filename
  vcl_string tree_txt_in = "V:/p1a_related/p1a_index/conf_index/land_map_index/wr6/2d_geo_index_tile_3.txt";
  double min_size_in;
  bvgl_2d_geo_index_node_sptr root_in = bvgl_2d_geo_index::read_and_construct<volm_conf_land_map_indexer_sptr>(tree_txt_in, min_size_in);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves_in;
  bvgl_2d_geo_index::get_leaves(root, leaves_in);

  vcl_cout << "input tree min_size:  " << min_size << vcl_endl;
  vcl_cout << "loaded tree min_size: " << min_size_in << vcl_endl;
  for (unsigned i = 0; i < leaves.size(); i++)
  {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[i].ptr());
    vcl_stringstream filename;
    filename << out_folder << leaf_ptr->get_label_name("land_map_index", "all");
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr_in = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[i].ptr());
    vcl_stringstream filename_in;
    filename_in << out_folder << leaf_ptr_in->get_label_name("land_map_index", "all");

    vcl_cout << " ---------------------------------leaf " << i << "----------------------------------- " << vcl_endl;
    vcl_cout << "leaf: " << leaf_ptr->extent_ << " --> filename: " << filename.str() << vcl_endl; 
    vcl_cout << "leaf: " << leaf_ptr_in->extent_ << " --> filename: " << filename_in.str() << vcl_endl;

  }
#if 0
  // NLCD folder
  vcl_string nlcd_folder = "v:/p1a_related/NLCD/";
  vcl_vector<volm_img_info> nlcd_infos;
  volm_io_tools::load_nlcd_imgs(nlcd_folder, nlcd_infos);

  // load OSM data
  vcl_string osm_bin_file = "v:/p1a_related/OSM/p1a_wr2_tile_3_osm.bin";
  volm_osm_objects osm(osm_bin_file);
  vcl_cout << "read " << osm.num_locs() << " OSM loc points, " << osm.num_roads() << " OSM roads and " << osm.num_regions() << " OSM regions" << vcl_endl;

  // load URGENT building folder
  vcl_string urgent_file = "v:/p1a_related/URGENT/Urgent_N32W080.csv";
  vcl_vector<vcl_pair<vgl_polygon<double>, vgl_point_2d<double> > > build_polys;
  vcl_vector<double> build_heights;
  volm_io::read_building_file(urgent_file, build_polys, build_heights);

  // load SME data (points)
  vcl_string sme_file = "v:/p1a_related/SME/SME_N32W080.csv";
  vcl_vector<vcl_pair<vgl_point_2d<double>, int> > sme_objects;
  volm_io::read_sme_file(sme_file, sme_objects);

  // create a 2d geo index tree
  vgl_box_2d<double> bbox(-80, -79, 32, 33);
  double min_size = 0.25;
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<volm_conf_land_map_indexer_sptr>(bbox, min_size);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  unsigned tree_depth = bvgl_2d_geo_index::depth(root);
  // write out the tree structure
  vcl_string out_folder = "./";
  vcl_string tree_txt = out_folder + "bvgl_2d_geo_index.txt";
  vcl_stringstream tree_kml;
  tree_kml << out_folder + "bvgl_2d_geo_index_depth_" << tree_depth << ".kml";
  bvgl_2d_geo_index::write(root, tree_txt, min_size);
  bvgl_2d_geo_index::write_to_kml(root, tree_depth, tree_kml.str());
  
#if 1
  // create volm_conf_land_map_indexer for each leaf
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    leaf_ptr->contents_ = new volm_conf_land_map_indexer(leaf_ptr->extent_, 5.0);

    if (l_idx != 5)
      continue;
#endif

#if 0
    // add NLCD
    for (unsigned i = 0; i < nlcd_infos.size(); i++) {
      vil_image_view<vxl_byte>* image = dynamic_cast<vil_image_view<vxl_byte>*>(nlcd_infos[i].img_r.ptr());
      leaf_ptr->contents_->add_locations(*image, nlcd_infos[i].cam);
    }
    volm_conf_loc_map land_locs = leaf_ptr->contents_->land_locs();
    vcl_cout << leaf_ptr->contents_->nlocs() << " locations are added into land map indexer from NLCD, including: " << leaf_ptr->contents_->nland_type() << " land types:\n";
    for (volm_conf_loc_map::iterator mit = land_locs.begin(); mit != land_locs.end(); ++mit)
      vcl_cout << mit->second.size() << " " << volm_osm_category_io::volm_land_table[mit->first].name_ << ' ';
    vcl_cout << vcl_endl;
#endif

#if 0
    // add OSM points
    unsigned n_pts = osm.num_locs();
    vcl_vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
    for (unsigned p_idx = 0; p_idx < n_pts; p_idx++) {
      if (loc_pts[p_idx]->prop().level_ < 2)
        continue;
      leaf_ptr->contents_->add_locations(loc_pts[p_idx]->loc(), loc_pts[p_idx]->prop().id_);
    }
    land_locs = leaf_ptr->contents_->land_locs();
    vcl_cout << leaf_ptr->contents_->nlocs() << " locations are added into leaf " << l_idx << " from OSM points, including: " << leaf_ptr->contents_->nland_type() << " land types:\n";
    for (volm_conf_loc_map::iterator mit = land_locs.begin(); mit != land_locs.end(); ++mit)
      vcl_cout << mit->second.size() << " " << volm_osm_category_io::volm_land_table[mit->first].name_ << ' ';
    vcl_cout << vcl_endl;
#endif

#if 0
    // add OSM lines
    unsigned n_lines = osm.num_roads();
    for (unsigned r_idx = 0; r_idx < n_lines; r_idx++)
    {
      if (osm.loc_lines()[r_idx]->prop().level_ < 2)
        continue;
      vcl_vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
      leaf_ptr->contents_->add_locations(road, osm.loc_lines()[r_idx]->prop().id_);
    }
#endif

#if 0
    // add OSM junctions
    n_lines = osm.num_roads();
    vcl_vector<vcl_vector<vgl_point_2d<double> > > lines;
    vcl_vector<unsigned char> lines_prop;
    for (unsigned i = 0; i < n_lines; i++) {
      lines.push_back(osm.loc_lines()[i]->line());
      lines_prop.push_back(osm.loc_lines()[i]->prop().id_);
    }
    leaf_ptr->contents_->add_locations(lines, lines_prop);

#endif

#if 0
    // add OSM polygons
    unsigned n_regions = osm.num_regions();
    for (unsigned r_idx = 0; r_idx < n_regions; r_idx++)
    {
      if (osm.loc_polys()[r_idx]->prop().level_ < 2)
        continue;
      //if (osm.loc_polys()[r_idx]->prop().id_ != 133)
      //  continue;
      vgl_polygon<double> poly(osm.loc_polys()[r_idx]->poly()[0]);
      leaf_ptr->contents_->add_locations(poly, osm.loc_polys()[r_idx]->prop().id_);
    }
#endif

#if 1
    // add URGENT data
    for (unsigned i = 0; i < build_polys.size(); i++)
    {
      unsigned char land_id = volm_osm_category_io::volm_land_table_name["building"].id_;
      if (build_heights[i] > 10.0)
        land_id = volm_osm_category_io::volm_land_table_name["tall_building"].id_;
      // form the polygon
      vgl_polygon<double> poly;
      poly.new_sheet();
      for (unsigned pi = 0; pi < build_polys[i].first[0].size(); pi++)
        if (vcl_find(poly[0].begin(), poly[0].end(), build_polys[i].first[0][pi]) == poly[0].end())
          poly.push_back(build_polys[i].first[0][pi]);
      leaf_ptr->contents_->add_locations(poly, land_id);
    }
#endif

#if 1
    // add SME data (fort only)
    for (unsigned i = 0; i < sme_objects.size(); i++)
    {
      unsigned char land_id = (unsigned char)sme_objects[i].second;
      leaf_ptr->contents_->add_locations(sme_objects[i].first, land_id);
    }
#endif

  }

#if 0
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    vcl_vector<unsigned char> land_types = leaf_ptr->contents_->land_types();
    vcl_cout << "Leaf " << l_idx << " covers " << leaf_ptr->contents_->bbox()
             << " and has " << land_types.size() << " different land types given density " << leaf_ptr->contents_->density() << vcl_endl;
    vcl_stringstream filename;
    filename << out_folder << leaf_ptr->get_label_name("land_map_indexer", "all");
    vcl_string out_kml_file = vul_file::strip_extension(filename.str()) + ".kml";
    leaf_ptr->contents_->write_out_kml(out_kml_file, 1E-5, true);
  }
#endif

#if 1
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    vcl_vector<unsigned char> land_types = leaf_ptr->contents_->land_types();
    vcl_cout << "Leaf " << l_idx << " covers " << leaf_ptr->contents_->bbox()
             << " and has " << land_types.size() << " different land types given density " << leaf_ptr->contents_->density() << vcl_endl;
    for (vcl_vector<unsigned char>::iterator vit = land_types.begin(); vit != land_types.end(); ++vit) {
      vcl_stringstream filename;
      vcl_string identifier = volm_osm_category_io::volm_land_table[*vit].name_;
      vcl_replace(identifier.begin(), identifier.end(), '/', '_');
      filename << out_folder << leaf_ptr->get_label_name("land_map_indexer", identifier);
      vcl_string out_kml_file = vul_file::strip_extension(filename.str()) + ".kml";
      leaf_ptr->contents_->write_out_kml(out_kml_file, *vit);
    }
  }
#endif

  // test binary IO
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
  {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    vcl_stringstream out_bin_file;
    out_bin_file << out_folder << leaf_ptr->get_label_name("land_map_indexer", "");
    leaf_ptr->contents_->write_out_bin(out_bin_file.str());
  }

  double min_size_in;
  bvgl_2d_geo_index_node_sptr root_in = bvgl_2d_geo_index::read_and_construct<volm_conf_land_map_indexer_sptr>(tree_txt, min_size_in);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves_in;
  bvgl_2d_geo_index::get_leaves(root_in, leaves_in);

  TEST_NEAR("geo index tree structure IO: min_size", min_size, min_size_in, 1E-5);
  TEST("geo index tree structure IO: leave structure", leaves.size(), leaves_in.size());

  for (unsigned l_idx = 0; l_idx < leaves_in.size(); l_idx++)
  {
    vcl_stringstream in_bin_file;
    in_bin_file << out_folder << leaves_in[l_idx]->get_label_name("land_map_indexer","");
    if (!vul_file::exists(in_bin_file.str()))
      continue;
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves_in[l_idx].ptr());
    leaf_ptr->contents_ = new volm_conf_land_map_indexer(in_bin_file.str());
  }
  bool binary_io = true;
  for (unsigned l_idx = 0; l_idx < leaves_in.size(); l_idx++)
  {
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>* leaf_in_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves_in[l_idx].ptr());
    if (!leaf_in_ptr->contents_)
      continue;
    vcl_cout << " --------------- leaf " << l_idx << " --------------------" << vcl_endl;
    vcl_cout << " ori leaf: " << leaf_ptr->contents_->box_string() << " --> " 
             << leaf_ptr->contents_->nland_type() << " land types, " << leaf_ptr->contents_->nlocs() << " locations" << vcl_endl;
    vcl_cout << "read leaf: " << leaf_in_ptr->contents_->box_string() << " --> "
             << leaf_in_ptr->contents_->nland_type() << " land types, " << leaf_in_ptr->contents_->nlocs() << " locations" << vcl_endl;
    binary_io = (leaf_ptr->contents_->nland_type() == leaf_in_ptr->contents_->nland_type()) &&
                (leaf_ptr->contents_->nlocs() == leaf_in_ptr->contents_->nlocs());
  }
  TEST("vpgl_conf_land_map_indexer binary IO", binary_io, true);
#endif
}

TESTMAIN( test_volm_conf_land_map_indexer );