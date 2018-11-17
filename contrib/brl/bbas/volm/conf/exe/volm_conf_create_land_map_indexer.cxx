// This is contrib/brl/bbas/volm/conf/exe/volm_conf_create_land_map_indexer.cxx
//:
// \file
// \brief  executable to create configurational land map indexer database
//
// \author Yi Dong
// \date August 20, 2014
// \verbatim
//   Modifications
//    <none yet>
// \endverbatim
//

#include <algorithm>
#include <iostream>
#include <utility>
#include <vgl/vgl_intersection.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_tile.h>
#include <volm/conf/volm_conf_land_map_indexer.h>
#include <volm/volm_category_io.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bkml/bkml_parser.h>


static void error(std::string log_file, const std::string& msg)
{
  std::cerr << msg;  volm_io::write_post_processing_log(std::move(log_file), msg);
}

int main(int argc, char** argv)
{
  vul_arg<unsigned>   world_id("-world", "ROI world id", 9999);
  vul_arg<unsigned>   tile_id("-tile", "ROI tile id", 9999);
  vul_arg<double>     min_size("-min", "minimum size of geo index (in wgs84 degree)", 0.0078125);
  vul_arg<double>     density("-density", "location point density (in meter)", 5.0);
  vul_arg<double> road_density("-line-density", "location point density along the line (in meter)", 5.0);
  vul_arg<std::string> poly_roi("-poly", "ROI region polygon kml file", "");
  vul_arg<std::string> map_folder("-map", "land map folder", "");
  vul_arg<std::string> osm_file("-osm", "osm binary file", "");
  vul_arg<std::string> urgent_folder("-build", "URGENT data folder", "");
  vul_arg<std::string> sme_folder("-sme", "SME data folder", "");
  vul_arg<std::string> out_folder("-out", "output folder", "");
  vul_arg<int>        leaf_idx("-leaf", "leaf id inside tile (for parallel execution)", -1);
  vul_arg<bool>       is_land_map("-land", "option to choose add land map data (default is false)", false);
  vul_arg<unsigned>   osm_level("-level", "level of OSM data that will be chosen to add into database", 2);
  vul_arg<bool>       is_kml("-kml", "option to generate kml (default is true)", true);
  vul_arg<unsigned>   is_osm_road("-road", "option to put OSM road into database (default is false)", 1);
  vul_arg<unsigned>   is_osm_pts("-pts",   "option to put OSM location points into database (default is true)", 1);
  vul_arg<unsigned>   is_osm_regions("-region", "option to put OSM regions into database (default is true)", 1);
  vul_arg<unsigned>   is_osm_junction("-junction","option to put OSM road intersection into database (default is true)", 1);
  vul_arg_parse(argc, argv);

  // input check
  if (world_id() == 9999 || tile_id() == 9999 || poly_roi().compare("") == 0 || map_folder().compare("") == 0 ||
      osm_file().compare("") == 0 || urgent_folder().compare("") == 0 || out_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return false;
  }
  std::stringstream log_file_str;
  log_file_str << out_folder() + "/log_create_land_map_indexer_" << tile_id() << ".xml";
  std::string log_file = log_file_str.str();
  std::stringstream log;

  // locate region
  std::vector<volm_tile> tiles;
  if (!volm_tile::generate_tiles(world_id(), tiles)) {
    log << "ERROR: Unknown ROI world id: " << world_id() << '\n';  error(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  if (tile_id() >= tiles.size()) {
    log << "ERROR: Unknown tile id " << tile_id() << " for ROI world " << world_id() << '\n';  error(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  // create 2d geo index
  if (!vul_file::exists(poly_roi())) {
    log << "ERROR: Can not find roi polygon kml file: " << poly_roi() << '\n';  error(log_file, log.str());
    return false;
  }
  vgl_polygon<double> roi_poly = bkml_parser::parse_polygon(poly_roi());
  bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<volm_conf_land_map_indexer_sptr>(tiles[tile_id()].bbox_double(), min_size(), roi_poly);
  std::vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  unsigned tree_depth = bvgl_2d_geo_index::depth(root);
  std::stringstream tree_txt;
  tree_txt << out_folder() << "/2d_geo_index_tile_" << tile_id() << ".txt";
  std::stringstream tree_kml;
  tree_kml << out_folder() << "/2d_geo_index_tile_" << tile_id() << "_depth_" << tree_depth << ".kml";
  bvgl_2d_geo_index::write(root, tree_txt.str(), min_size());
  bvgl_2d_geo_index::write_to_kml(root, tree_depth, tree_kml.str(), "land_map_indexer");

  // load NLCD land map
  std::vector<volm_img_info> map_info;
  volm_io_tools::load_nlcd_imgs(map_folder(), map_info);

  // load OSM data
  volm_osm_objects osm(osm_file());

  // load URGENT building data
  std::string glob = urgent_folder() + "/*.csv";
  std::vector<std::pair<vgl_polygon<double>, vgl_point_2d<double> > > build_polys;
  std::vector<double> build_heights;
  for (vul_file_iterator fit = glob; fit; ++fit) {
    volm_io::read_building_file(fit(), build_polys, build_heights);
  }

  // load SME data
  glob = sme_folder() + "/*.csv";
  std::vector<std::pair<vgl_point_2d<double>, int> > sme_objects;
  for (vul_file_iterator fit = glob; fit; ++fit)
    volm_io::read_sme_file(fit(), sme_objects);

  std::cout << " ------------------------------ START -----------------------------\n";
  std::cout << "ROI polygon (" << roi_poly[0].size() << " vertices) is loaded from " << poly_roi() << '\n';
  std::cout << "2D bvgl geo index is created with min size: " << min_size() << " and " << leaves.size() << " leaves (depth " << tree_depth << ") are inside ROI.\n";
  std::cout << "read " << map_info.size() << " land images!\n";
  std::cout << "read " << sme_objects.size() << " SME objects!\n";
  std::cout << "read " << build_polys.size() << " URGENT buildings!\n";
  std::cout << "read " << osm.num_locs() << " OSM location points, " << osm.num_roads() << " OSM roads and " << osm.num_regions() << " OSM regions" << std::endl;

  // start to create land map indexer for each leaf
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
  {
    if (leaf_idx() >= 0 && leaf_idx() < leaves.size())
      if (l_idx != leaf_idx())
        continue;

    auto* leaf_ptr = dynamic_cast<bvgl_2d_geo_index_node<volm_conf_land_map_indexer_sptr>*>(leaves[l_idx].ptr());
    leaf_ptr->contents_ = new volm_conf_land_map_indexer(leaf_ptr->extent_, density());
    std::cout << "--------------------------------------------------------------------------\n";
    std::cout << "\t adding locations into region: " << leaf_ptr->extent_ << " (leaf id: " << l_idx << ")...\n";

    // add land map data if required
    if (is_land_map())
    {
      std::cout << "\t adding locations from " << map_info.size() << " land maps\n";
      for (auto & m_idx : map_info) {
        auto* image = dynamic_cast<vil_image_view<vxl_byte>*>(m_idx.img_r.ptr());
        if (!image) {
          log << "ERROR: load image view failed for land map: " << m_idx.img_name << '\n';  error(log_file, log.str());
          return volm_io::EXE_ARGUMENT_ERROR;
        }
        if (!leaf_ptr->contents_->add_locations(*image, m_idx.cam)) {
          log << "ERROR: adding locations from land map: " << m_idx.img_name << "failed\n";  error(log_file, log.str());
          return volm_io::EXE_ARGUMENT_ERROR;
        }
      }
      std::cout << "\t " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type()
               << " land types) are added after loading data from land maps" << std::flush << std::endl;
    }

    // add urgent data
    std::cout << "\t adding locations from " << build_polys.size() << " URGENT buildings data...\n";
    for (unsigned i = 0; i < build_polys.size(); i++)
    {
      unsigned char land_id = volm_osm_category_io::volm_land_table_name["building"].id_;
      if (build_heights[i] > 20.0)
        land_id = volm_osm_category_io::volm_land_table_name["tall_building"].id_;
      // add URGENT building by its center points
      vgl_point_3d<double> loc_pt(build_polys[i].second.x(), build_polys[i].second.y(), build_heights[i]);
      if (!leaf_ptr->contents_->add_locations(loc_pt, land_id)) {
        log << "ERROR: adding location from URGENT building[" << i << "]: " << build_polys[i].second << " failed\n";
        error(log_file, log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
#if 0
      // add URGENT building by its boundary points
      vgl_polygon<double> poly;
      poly.new_sheet();
      for (unsigned pi = 0; pi < build_polys[i].first[0].size(); pi++)
        if (std::find(poly[0].begin(), poly[0].end(), build_polys[i].first[0][pi]) == poly[0].end())
          poly.push_back(build_polys[i].first[0][pi]);
      if (!vgl_intersection(leaf_ptr->extent_, poly))
        continue;
      if (!leaf_ptr->contents_->add_locations(poly, land_id, build_heights[i])) {
        log << "ERROR: adding location from URGENT building: " << build_polys[i].second << " failed." << std::endl;  error(log_file, log.str());
        return volm_io::EXE_ARGUMENT_ERROR;
      }
#endif
    }
    std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading URGENT data"
             << std::flush << std::endl;

    // add sme data
    std::cout << "\t adding locations from " << sme_objects.size() << " SME objects...\n";
    for (auto & sme_object : sme_objects) {
      vgl_point_3d<double> sme_pt(sme_object.first.x(), sme_object.first.y(), -1.0);
      leaf_ptr->contents_->add_locations(sme_pt, (unsigned char)sme_object.second);
    }
    std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading SME data"
             << std::flush << std::endl;

    // add OSM data
    if (is_osm_pts()) // add OSM location points
    {
      unsigned n_pts = osm.num_locs();
      std::cout << "\t adding locations from " << n_pts << " OSM location points...\n";
      std::vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
      for (unsigned p_idx = 0; p_idx < n_pts; p_idx++)
      {
        if (loc_pts[p_idx]->prop().level_ < osm_level())
          continue;
        vgl_point_3d<double> pt_3d(loc_pts[p_idx]->loc().x(), loc_pts[p_idx]->loc().y(), -1.0);
        leaf_ptr->contents_->add_locations(pt_3d, loc_pts[p_idx]->prop().id_);
      }
      std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading OSM points"
               << std::flush << std::endl;
    }

    if (is_osm_road())  // add OSM location roads
    {
      unsigned n_lines = osm.num_roads();
      std::cout << "\t adding locations from " << n_lines << " OSM roads...\n";
      for (unsigned r_idx = 0; r_idx < n_lines; r_idx++)
      {
        if (osm.loc_lines()[r_idx]->prop().level_ < osm_level())
          continue;
        // ignore the general road category
        //if (osm.loc_lines()[r_idx]->prop().name_ == "roads")
        //  continue;
        leaf_ptr->contents_->add_locations(osm.loc_lines()[r_idx]->line(), osm.loc_lines()[r_idx]->prop().id_, -1.0, road_density());
      }
      std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading OSM roads"
               << std::flush << std::endl;
    }

    if (is_osm_junction())  // add OSM road intersections
    {
      std::cout << "\t adding locations from OSM road intersections...\n";
      unsigned n_lines = osm.num_roads();
      std::vector<std::vector<vgl_point_2d<double> > > lines;
      std::vector<unsigned char> lines_prop;
      for (unsigned i = 0; i < n_lines; i++) {
        lines.push_back(osm.loc_lines()[i]->line());
        lines_prop.push_back(osm.loc_lines()[i]->prop().id_);
      }
      std::cout << "there are " << lines.size() << " roads with " << lines_prop.size() << " properties" << std::endl;
      leaf_ptr->contents_->add_locations(lines, lines_prop);
      std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading OSM road intersections"
               << std::flush << std::endl;
    }

    if (is_osm_regions())  // add OSM location regions
    {
      unsigned n_regions = osm.num_regions();
      std::cout << "\t adding locations from " << n_regions << " OSM regions...\n";
      for (unsigned r_idx = 0; r_idx < n_regions; r_idx++)
      {
        if (osm.loc_polys()[r_idx]->prop().level_ < osm_level() )
          continue;
        vgl_polygon<double> poly(osm.loc_polys()[r_idx]->poly()[0]);
        leaf_ptr->contents_->add_locations(poly, osm.loc_polys()[r_idx]->prop().id_, -1.0);
      }
      std::cout << "\t   " << leaf_ptr->contents_->nlocs() << " locations (" << leaf_ptr->contents_->nland_type() << " land types) are added after loading OSM regions"
               << std::flush << std::endl;
    }

    // output
    std::stringstream filename;
    filename << out_folder() << leaf_ptr->get_label_name("land_map_index", "all");
    // write out kml file for visualization if necessary
    if (is_kml())
    {
      std::string out_kml_file = vul_file::strip_extension(filename.str()) + ".kml";
      std::cout << "\t write location database into kml: " << out_kml_file << "...\n";
      leaf_ptr->contents_->write_out_kml(out_kml_file, 0.25E-4);
    }

    // write out binary
    std::string out_bin_file = filename.str();
    std::cout << "\t write location database into binary file: " << filename.str() << "..." << std::flush << std::endl;
    leaf_ptr->contents_->write_out_bin(out_bin_file);

  }  // end of loop over leaves

  return volm_io::SUCCESS;
}
