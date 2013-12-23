//:
// \file
// \brief  executable to find and create kml file containing road intersections from open street map
//
// \author Yi Dong
// \date December 19 2013
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_osm_objects.h>
#include <volm/volm_category_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_box_2d.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <bkml/bkml_write.h>
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>

void error(vcl_string log_file, vcl_string msg)
{
  vcl_cerr << msg;  volm_io::write_post_processing_log(log_file, msg);
}

//: simple searching whether a point is inside the vector already
bool is_included(vcl_vector<vgl_point_2d<double> > const& pts, vgl_point_2d<double> const& pt)
{
  vgl_vector_2d<double> pt_v(pt.x(), pt.y());
  unsigned num_pts = pts.size();
  for (unsigned i = 0; i < num_pts; i++) {
    vgl_point_2d<double> dp = pts[i]-pt_v;
    if ( dp.x()*dp.x() < 1E-8 && dp.y()*dp.y() < 1E-8)
      return true;
  }
  return false;
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> osm_folder("-osm", "folder where osm binary stores", "");
  vul_arg<vcl_string> out_folder("-out", "output folder","");
  vul_arg<float> min_size ("-min", "minimum size of image size (in wgs84 degree)",0.05);
  vul_arg<unsigned> world_id("-world", "world id for ROI (from 1 to 5)",100);
  vul_arg<unsigned> tile_id("-tile", "tile id for ROI", 100);
  vul_arg_parse(argc, argv);

  // check inputs
  if (osm_folder().compare("") == 0 || out_folder().compare("") == 0 || world_id() == 100 || tile_id() == 100) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;
  
  vul_timer t;
  t.mark();

  log_file << out_folder() << "/log_osm_2d_map_wr" << world_id() << ".xml";

  // create volm_geo_index2 from tile
  vcl_vector<volm_tile> tiles;
  if (world_id() == 1)       tiles = volm_tile::generate_p1b_wr1_tiles();
  else if (world_id() == 2)  tiles = volm_tile::generate_p1b_wr2_tiles();
  else if (world_id() == 3)  tiles = volm_tile::generate_p1b_wr3_tiles();
  else if (world_id() == 4)  tiles = volm_tile::generate_p1b_wr4_tiles();
  else if (world_id() == 5)  tiles = volm_tile::generate_p1b_wr5_tiles();
  else {
    log << "ERROR: unknown world id " << world_id() << " only 1 to 5 are allowed\n";  error(log_file.str(), log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned tile_size = tiles.size();
  vcl_vector<volm_geo_index2_node_sptr> roots;
  for (unsigned i = 0; i < tile_size; i++)
    roots.push_back(volm_geo_index2::construct_tree<volm_osm_object_ids_sptr>(tiles[i], min_size()));

  // load open street map binary
  vcl_vector<volm_osm_objects> osm_objs;
  for (unsigned i = 0; i < tile_size; i++) {
    vcl_stringstream osm_file;  osm_file << osm_folder() << "/p1b_wr" << world_id() << "_tile_" << i << "_osm.bin";
    if (!vul_file::exists(osm_file.str())) {
      log << "ERROR: can not find osm binary file: " << osm_file.str() << '\n';  error(log_file.str(), log.str());
      return false;
    }
    osm_objs.push_back(volm_osm_objects(osm_file.str()));
  }

  vcl_cout << " --------------- START -----------------" << vcl_endl;
  vcl_cout << " there are " << tiles.size() << " tiles, "
           << osm_objs.size() << " open street map dataset " << vcl_endl;
  if (tile_id() > tiles.size()) {
    log << "ERROR: given tile id " << tile_id() << " does not exist\n";  error(log_file.str(), log.str());
    return false;
  }

  for (unsigned t_idx = 0; t_idx < tile_size; t_idx++)
  {
    if (t_idx != tile_id())
      continue;

    vcl_stringstream kml_name;
    double lon_min, lat_min, lon_max, lat_max;
    lon_min = tiles[tile_id()].bbox().min_x();  lat_min = tiles[tile_id()].bbox().min_y();
    lon_max = tiles[tile_id()].bbox().max_x();  lat_max = tiles[tile_id()].bbox().max_y();
    double scale = lon_max-lon_min;
    kml_name << out_folder() << "/osm_4_way-intersections_wr" << world_id() << "_tile_" << tile_id() << ".kml";
    vcl_ofstream ofs(kml_name.str().c_str());
    bkml_write::open_document(ofs);
    vcl_stringstream str_box;
    str_box << vcl_setprecision(6) << lat_min << 'x' << vcl_setprecision(6) << lon_min << "to"
            << vcl_setprecision(6) << lat_max << 'x' << vcl_setprecision(6) << lon_max;
    bkml_write::write_box(ofs, "region", str_box.str(), tiles[tile_id()].bbox_double());
    // retrieve leaves for current tile
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    volm_geo_index2::get_leaves(roots[t_idx], leaves);
    volm_osm_objects osm = osm_objs[t_idx];

    vcl_vector<vgl_point_2d<double> > all_cross_pts;
    vcl_vector<volm_land_layer> all_cross_props;

    // search for junction inside each leaf
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
      volm_geo_index2_node_sptr leaf = leaves[l_idx];
      vgl_box_2d<double> leaf_bbox_geo = leaf->extent_;

      // find the road that is in current leaf
      unsigned num_roads = osm.num_roads();
      vcl_vector<vcl_vector<vgl_point_2d<double> > > roads_in_leaf;
      vcl_vector<volm_land_layer> roads_in_leaf_props;
      for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
        vcl_vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
        vcl_vector<vgl_point_2d<double> > line_geo;
        // check and obtain the road segment that lies inside the leaf
        if (!volm_io_tools::line_inside_the_box(leaf_bbox_geo, road, line_geo))
          continue;
        roads_in_leaf.push_back(line_geo);
        roads_in_leaf_props.push_back(osm.loc_lines()[r_idx]->prop());
      }
      vcl_cout <<  leaves.size() << " leaves in total, handling " << l_idx << " leaf with " << roads_in_leaf.size() << " roads in it ";
      // find and ingest osm junctions for current leaf
      unsigned n_road_in = roads_in_leaf.size();
      for (unsigned i_rdx = 0; i_rdx < n_road_in; i_rdx++) {
        if (i_rdx%1000 == 0)  vcl_cout << i_rdx << ".";  vcl_cout.flush();
        vcl_vector<vgl_point_2d<double> > curr_rd = roads_in_leaf[i_rdx];
        volm_land_layer curr_rd_prop = roads_in_leaf_props[i_rdx];
        vcl_vector<vcl_vector<vgl_point_2d<double> > > net;
        vcl_vector<volm_land_layer> net_props;
        for (unsigned i = 0; i < n_road_in; i++)
          if (i != i_rdx)
          {
            net.push_back(roads_in_leaf[i]);
            net_props.push_back(roads_in_leaf_props[i]);
          }
        // find all possible junction for current road
        vcl_vector<vgl_point_2d<double> > cross_pts;
        vcl_vector<volm_land_layer> cross_props;
        if (!volm_io_tools::search_junctions(curr_rd, curr_rd_prop, net, net_props, cross_pts, cross_props)) {
          log << "ERROR: find road junction for tile " << t_idx << " leaf " << leaf_bbox_geo << " road " << i_rdx << " failed\n";
          error(log_file.str(), log.str());
          return volm_io::EXE_ARGUMENT_ERROR;
        }
        if (cross_pts.size() != cross_props.size()) {
          log << "ERROR: number of junctions (" << cross_pts.size()
              << ") is different from number of junction types (" << cross_props.size()
              << ") for road " << i_rdx << "!\n";
          error(log_file.str(), log.str());
          return volm_io::EXE_ARGUMENT_ERROR;
        }
        for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
          if (cross_props[c_idx].name_ == "T_section")
            continue;
          if (!is_included(all_cross_pts, cross_pts[c_idx]) && cross_props[c_idx].name_ != "invalid") {
            all_cross_pts.push_back(cross_pts[c_idx]);
            all_cross_props.push_back(cross_props[c_idx]);
          }
          
        }
      }
      vcl_cout << vcl_endl;
      
    }  // end of loop over leaves

    // put all junctions into kml file
    for (unsigned c_idx = 0; c_idx < all_cross_pts.size(); c_idx++) {
      vcl_string name = all_cross_props[c_idx].name_;
      vcl_stringstream str_p;  str_p << "id=" << c_idx;
      bkml_write::write_location(ofs, all_cross_pts[c_idx], name, str_p.str(), 0.6);
    }
    bkml_write::close_document(ofs);
    ofs.close();

    vcl_cout << "****************************" << vcl_endl;
    vcl_cout << "\nFINISHED " << vcl_endl;
    vcl_cout << "****************************" << vcl_endl;
  }  // end of loop over tiles

  vcl_cout << "total time: " << t.all()/1000 << " seconds = " << t.all()/(1000*60) << " mins.\n";
  return volm_io::SUCCESS;

}