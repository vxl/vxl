//:
// \file
// \brief  executable to create geo_indexed open stree map object ids
//  Each leaf node contains lists of ids of the volm_osm objects that intersects with this leaf
//
// \author Yi Dong
// \date August 08, 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_geo_index2_sptr.h>
#include <volm/volm_osm_objects.h>
#include <vpgl/vpgl_utm.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vgl/vgl_intersection.h>
#include <bkml/bkml_parser.h>

void error_report(vcl_string error_file, vcl_string error_msg)
{
  vcl_cerr << error_msg;
  volm_io::write_post_processing_log(error_file, error_msg);
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> osm_in("-osm", "osm binary file contains all volm_osm objects", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<double> min_size("-min-size", "minimum leaf size (in degree), note the root corresponds to a tile with 1 degree in size", 0.5);
  vul_arg<unsigned> world_id("-world", "id of the region of interests(1-5)", 100);
  vul_arg<unsigned> tile_id("-tile", "id of the tile", 100);
  vul_arg<vcl_string> out_pre("-out_pre", "output file folder with file separator at the end", "");
  vul_arg<bool> read("-read", "option to read created geo_index2_osm and osm statistics for current tile", false);
  vul_arg<bool> is_kml("-k", "option to create kml file for volm_osm object", false);
  
  vul_arg_parse(argc, argv);

  if (read()) {
    // check input
    if(osm_in().compare("") == 0 || out_pre().compare("") == 0 || tile_id() == 100 || world_id() == 100)
    {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    // read and construct tree
    vcl_stringstream file_name;
    file_name << out_pre() << "geo_index2_wr" << world_id() << "_tile_" << tile_id() << ".txt";
    if (!vul_file::exists(file_name.str())) {
      vcl_cout << "ERROR: can not find geo_index2 txt: " << file_name.str() << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    double min_size;
    volm_geo_index2_node_sptr root = volm_geo_index2::read_and_construct<volm_osm_object_ids_sptr>(file_name.str(), min_size);
    // obtain all leaves
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    volm_geo_index2::get_leaves(root, leaves);
    // load the content for valid leaves
    vcl_stringstream file_name_pre;
    file_name_pre << out_pre() << "/geo_index2_wr" << world_id() << "_tile_" << tile_id();
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
      vcl_string bin_file = leaves[l_idx]->get_label_name(file_name_pre.str(), "osm");
      if (!vul_file::exists(bin_file))
        continue;
      volm_geo_index2_node<volm_osm_object_ids_sptr>* ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
      ptr->contents_ = new volm_osm_object_ids(bin_file);
    }

    // load the osm bin file to get real location date with unit of lon and lat, associated with ids stored in geo_index2
    if (!vul_file::exists(osm_in())) {
      vcl_cout << "ERROR: can not find osm binary: " << osm_in() << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_osm_objects osm_objs(osm_in());
    if (is_kml()) {
        vcl_stringstream kml_pts, kml_roads, kml_regions;
        kml_pts << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_pts.kml";
        kml_roads << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
        kml_regions << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_regions.kml";
        osm_objs.write_pts_to_kml(kml_pts.str());
        osm_objs.write_lines_to_kml(kml_roads.str());
        osm_objs.write_polys_to_kml(kml_regions.str());
    }
    // start
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
    {
      volm_geo_index2_node<volm_osm_object_ids_sptr>* leaf_ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
      if (leaf_ptr->contents_->is_empty())
        continue;
#if 1
      vcl_cout << " leaf " << l_idx << " (" << leaf_ptr->extent_.min_point() << ") has contents : "
               << leaf_ptr->contents_->num_pts() << " points "
               << leaf_ptr->contents_->num_lines() << " lines "
               << leaf_ptr->contents_->num_regions() << " regions "
               << vcl_endl;
      vcl_vector<unsigned> pt_ids = leaf_ptr->contents_->pt_ids();
      vcl_vector<unsigned> line_ids = leaf_ptr->contents_->line_ids();
      vcl_vector<unsigned> region_ids = leaf_ptr->contents_->region_ids();
      vcl_vector<unsigned>::iterator vit;
      for (vit = pt_ids.begin(); vit != pt_ids.end(); ++vit)
        vcl_cout << ' ' << (*vit);
      vcl_cout << vcl_endl;
      for (vit = line_ids.begin(); vit != line_ids.end(); ++vit)
        vcl_cout << ' ' << (*vit);
      vcl_cout << vcl_endl;
      for (vit = region_ids.begin(); vit != region_ids.end(); ++vit)
        vcl_cout << ' ' << (*vit);
      vcl_cout << vcl_endl;
#endif
    }

    return volm_io::SUCCESS;
  }

  // check input
  if (osm_in().compare("") == 0 || out_pre().compare("") == 0 ||
      in_poly().compare("") == 0 || tile_id() == 100 || world_id() == 100)
  {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_stringstream log;
  vcl_stringstream log_file;
  log_file << out_pre() << "/log_wr" << world_id() << "_tile_" << tile_id() << ".xml";
  // create volm_geo_index2
  vcl_cout << " =========== Start to create geo_index2_osm for world " << world_id() << " on tile " << tile_id()
           << " ===============" << vcl_endl;
  vcl_cout << " \t leaf size = " << min_size() << " degree" << vcl_endl;
  if (!vul_file::exists(in_poly())) {
    log << "ERROR: can not find region polygon file: " << in_poly() << '\n';
    error_report(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << " \t outer poly  has: " << poly[0].size() << vcl_endl;

  vcl_vector<volm_tile> tiles;
  switch (world_id())
  {
    case 1: {  tiles = volm_tile::generate_p1b_wr1_tiles();  break;  }
    case 2: {  tiles = volm_tile::generate_p1b_wr2_tiles();  break;  }
    case 3: {  tiles = volm_tile::generate_p1b_wr3_tiles();  break;  }
    case 4: {  tiles = volm_tile::generate_p1b_wr4_tiles();  break;  }
    case 5: {  tiles = volm_tile::generate_p1b_wr5_tiles();  break;  }
    default:
      log << "ERROR: unknown world id: " << world_id() << '\n';
      error_report(log_file.str(), log.str());
      return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (tile_id() > tiles.size()) {
    log << "ERROR: given tile id " << tile_id() << " exceeds total number of tiles in world " << world_id() << '\n';
    error_report(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_tile tile = tiles[tile_id()];

  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_osm_object_ids_sptr>(tile, min_size(), poly);

  // write the geo index2 structure
  vcl_stringstream file_name; file_name << out_pre() << "geo_index2_wr" << world_id() << "_tile_" << tile_id() << ".txt";
  volm_geo_index2::write(root, file_name.str(), min_size());
  unsigned depth = volm_geo_index2::depth(root);
  vcl_stringstream file_name2;
  file_name2 << out_pre() << "geo_index2_wr" << world_id() << "_tile_" << tile_id() << "_depth_0" << ".kml";
  vcl_stringstream file_name3;
  file_name3 << out_pre() << "geo_index2_wr" << world_id() << "_tile_" << tile_id() << "_depth_" << depth << ".kml";
  volm_geo_index2::write_to_kml(root, 0, file_name2.str());
  volm_geo_index2::write_to_kml(root, depth, file_name3.str());

  // load the volm_osm object
  volm_osm_objects osm_objs(osm_in());
  vcl_cout << " =========== Load volumetric open stree map objects... " << " ===============" << vcl_endl;
  vcl_cout << " \t number of location points in osm: " << osm_objs.num_locs() << vcl_endl;
  vcl_cout << " \t number of roads in osm: " << osm_objs.num_roads() << vcl_endl;
  vcl_cout << " \t number of regions in osm: " << osm_objs.num_regions() << vcl_endl;

  // add contents into valid leaves
  vcl_cout << " =========== Add osm contents into valid leaves... "  << " ===============" << vcl_endl;
  // add loc points into leaves
  vcl_vector<volm_osm_object_point_sptr> loc_pts = osm_objs.loc_pts();
  unsigned num_pts = osm_objs.num_locs();
  for (unsigned p_idx = 0; p_idx < num_pts; p_idx++) {
    vgl_point_2d<double> loc_pt = loc_pts[p_idx]->loc();
    volm_geo_index2_node_sptr leaf;
    volm_geo_index2::get_leaf(root, leaf, loc_pt);
    if (!leaf)
      continue;
    volm_geo_index2_node<volm_osm_object_ids_sptr>* leaf_ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaf.ptr());
    if (!leaf_ptr->contents_)
      leaf_ptr->contents_ = new volm_osm_object_ids();
    leaf_ptr->contents_->add_pt(p_idx);
  }
  // add road id into leaves
  vcl_vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  unsigned num_lines = osm_objs.num_roads();
  for (unsigned line_idx = 0; line_idx < num_lines; line_idx++) {
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    volm_geo_index2::get_leaves(root, leaves, loc_lines[line_idx]->line());
    if (leaves.empty())
      continue;
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
      volm_geo_index2_node<volm_osm_object_ids_sptr>* leaf_ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
      if (!leaf_ptr->contents_)
        leaf_ptr->contents_ = new volm_osm_object_ids();
      leaf_ptr->contents_->add_line(line_idx);
    }
  }
  // add region id into leaves
  vcl_vector<volm_osm_object_polygon_sptr> loc_regions = osm_objs.loc_polys();
  unsigned num_regions = osm_objs.num_regions();
  for (unsigned region_idx = 0; region_idx < num_regions; region_idx++) {
    vgl_polygon<double> poly = loc_regions[region_idx]->poly();
    // get rid off polygon with duplicated points
    bool ignore = false;
    for (unsigned i = 0; i < poly[0].size()-1; i++) {
      if (poly[0][i] == poly[0][i+1])
        ignore = true;
    }
    if (ignore)
      continue;
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    volm_geo_index2::get_leaves(root, leaves, loc_regions[region_idx]->poly()[0]);
    if (leaves.empty())
      continue;
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
      volm_geo_index2_node<volm_osm_object_ids_sptr>* leaf_ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
      if (!leaf_ptr->contents_)
        leaf_ptr->contents_ = new volm_osm_object_ids();
      leaf_ptr->contents_->add_region(region_idx);
    }
  }

  vcl_cout << " =========== write binary for leaves with contents... " << vcl_endl;
  // write contents (only leaves with contents)
  vcl_stringstream file_name_pre;
  file_name_pre << out_pre() << "/geo_index2_wr" << world_id() << "_tile_" << tile_id();
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
    volm_geo_index2_node<volm_osm_object_ids_sptr>* leaf_ptr = dynamic_cast<volm_geo_index2_node<volm_osm_object_ids_sptr>* >(leaves[l_idx].ptr());
    if (leaf_ptr->contents_->is_empty())
      continue;
#if 1
    vcl_cout << " leaf " << l_idx << " (" << leaf_ptr->extent_.min_point() << ") has contents : "
             << leaf_ptr->contents_->num_pts() << " points "
             << leaf_ptr->contents_->num_lines() << " lines "
             << leaf_ptr->contents_->num_regions() << " regions "
             << vcl_endl;
    vcl_vector<unsigned> pt_ids = leaf_ptr->contents_->pt_ids();
    vcl_vector<unsigned> line_ids = leaf_ptr->contents_->line_ids();
    vcl_vector<unsigned> region_ids = leaf_ptr->contents_->region_ids();
    vcl_vector<unsigned>::iterator vit;
    for (vit = pt_ids.begin(); vit != pt_ids.end(); ++vit)
      vcl_cout << ' ' << (*vit);
    vcl_cout << vcl_endl;
    for (vit = line_ids.begin(); vit != line_ids.end(); ++vit)
      vcl_cout << ' ' << (*vit);
    vcl_cout << vcl_endl;
    for (vit = region_ids.begin(); vit != region_ids.end(); ++vit)
      vcl_cout << ' ' << (*vit);
    vcl_cout << vcl_endl;
#endif
    vcl_string bin_file = leaf_ptr->get_label_name(file_name_pre.str(), "osm");
    leaf_ptr->contents_->write_osm_ids(bin_file);
  }
  vcl_cout << " ========================================== Finish ===============================================" << vcl_endl;
  vcl_cout << " \t results are stored in : " << out_pre() << vcl_endl;
  vcl_cout << " ========================================== Finish ===============================================" << vcl_endl;
  return volm_io::SUCCESS;
}
