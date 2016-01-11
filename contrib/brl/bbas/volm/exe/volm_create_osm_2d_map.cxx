//:
// \file
// \brief  executable to create land label 2d image (1 meter x 1 meter) resolution based on geo_cover and open street map
//
// \author Yi Dong
// \date September 22 2013
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
#include <volm/volm_osm_parser.h>
#include <vcl_where_root_dir.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_file_iterator.h>

static void error(vcl_string log_file, vcl_string msg)
{
  vcl_cerr << msg;  volm_io::write_post_processing_log(log_file, msg);
}

#if 0
int main(int argc, char** argv)
{
  vul_arg<vcl_string> osm_file("-osm", "folder where osm binary stores", "");
  vul_arg<vcl_string> out_folder("-out", "output folder", "");
  vul_arg<unsigned> world_id("-world", "world id for ROI (from 1 to 5", 6);
  vul_arg<unsigned> tile_id("-tile", "tile id for ROI", 0);
  vul_arg_parse(argc, argv);
  // load open street map binary
  if (!vul_file::exists(osm_file())) {
    vcl_cout << "error: can not find osm file: " << osm_file() << "!\n";
    return false;
  }
  // check the osm_to_volm file
  vcl_string osm_to_volm_txt = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  if (!vul_file::exists(osm_to_volm_txt)) {
    vcl_cout << "error: can not find osm_to_volm_txt: " << osm_to_volm_txt << "!\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  volm_osm_objects osm_obj(osm_file(),osm_to_volm_txt);

  // create 2d map
  vcl_cout << " --------------- START -----------------" << vcl_endl;
  vcl_cout << " world id = " << world_id() << ", tile_id = " << tile_id() << vcl_endl;
  // obtain the bounding box of current osm
  vgl_box_2d<double> osm_bbox = volm_osm_parser::parse_bbox(osm_file());
  double lon_min, lat_min, lon_max, lat_max;
  lon_min = osm_bbox.min_x();  lat_min = osm_bbox.min_y();
  lon_max = osm_bbox.max_x();  lat_max = osm_bbox.max_y();
  double scale_x = lon_max - lon_min;
  double scale_y = lat_max - lat_min;
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat_min, lon_min, 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double box_lx, box_ly, box_lz;
  lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);
  unsigned ni = (unsigned)vcl_ceil(box_lx);
  unsigned nj = (unsigned)vcl_ceil(box_ly);
  // form bounding box for current leaf in local coordinate
  vgl_box_2d<double> leaf_bbox_geo(lon_min, lon_max, lat_min, lat_max);
  vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);
  //vgl_box_2d<double> leaf_bbox(0.0, 0.0, box_lx, box_ly);

  // create 2d image for current leaf
  vil_image_view<vxl_byte> out_img(ni, nj, 1);
  vil_image_view<vxl_byte> level_img(ni, nj, 1);

  vcl_stringstream img_name;
  img_name << out_folder() << "/osm_" << "N" << vcl_setprecision(6) << lat_min
                            << "W" << vcl_setprecision(6) << lon_min
                            << "_S" << scale_x << 'x' << scale_y << ".tif";
  out_img.fill(0);
  level_img.fill(0);
  vcl_cout << " tile " << tile_id() << " has geo boundary " << leaf_bbox_geo
               << " corresponding to image size " << out_img.ni() << 'x' << out_img.nj() << vcl_endl;

  // ingest osm regions
  volm_osm_objects osm = osm_obj;
  unsigned cnt = 0;
  unsigned num_regions = osm.num_regions();
  for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
    vgl_polygon<double> poly(osm.loc_polys()[r_idx]->poly()[0]);
    // get rid off polygon with duplicated points
    bool ignore = false;
    for (unsigned i = 0; i < poly[0].size()-1; i++) {
      if (poly[0][i] == poly[0][i+1])
        ignore = true;
    }
    if (ignore)
      continue;
    // check whether the region intersect with current leaf
    if (!vgl_intersection(leaf_bbox_geo, poly))
      continue;
    unsigned char curr_level = osm.loc_polys()[r_idx]->prop().level_;
    // geo cover is already level 0 and therefore anything in osm with level 0 is ignored
    if (curr_level == 0)
      continue;
    // go from geo coord wgs84 to local
    vgl_polygon<double> img_poly(1);
    unsigned char curr_id = osm.loc_polys()[r_idx]->prop().id_;
    for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
      double lx, ly, lz;
      lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = lx;  double j = box_ly - ly;
      img_poly[0].push_back(vgl_point_2d<double>(i,j));
    }
    // using polygon iterator to loop over all points inside the polygon and intersect with leaf
    cnt++;
    vgl_polygon_scan_iterator<double> it(img_poly, true);
    for (it.reset(); it.next();  )
    {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if (x >=0 && y >= 0 && x < out_img.ni() && y < out_img.nj()) {
          if (curr_level >= level_img(x,y)) {
            out_img(x,y) = curr_id;  level_img(x,y) = curr_level;
          }
        }
      }
    }
  }

  // ingest osm roads (also record the road that intersects with current leaf for junction generation)
  cnt = 0;
  unsigned num_roads = osm.num_roads();
  vcl_vector<vcl_vector<vgl_point_2d<double> > > roads_in_leaf;
  vcl_vector<volm_land_layer> roads_in_leaf_props;
  for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
    vcl_vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
    vcl_vector<vgl_point_2d<double> > line_geo;
    // check and obtain the road segment that lies inside the leaf
    if (!volm_io_tools::line_inside_the_box(leaf_bbox_geo, road, line_geo))
      continue;
    // go from geo coords to leaf local
    vcl_vector<vgl_point_2d<double> > line_img;
    unsigned char curr_level = osm.loc_lines()[r_idx]->prop().level_;
    unsigned char curr_id = osm.loc_lines()[r_idx]->prop().id_;
    double width = osm.loc_lines()[r_idx]->prop().width_;
    for (unsigned pt_idx = 0; pt_idx < line_geo.size(); pt_idx++) {
      double lx, ly, lz;
      lvcs->global_to_local(line_geo[pt_idx].x(), line_geo[pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = lx - leaf_bbox.min_x();
      double j = leaf_bbox.max_y() - ly;
      if (i>=0 && j>=0 && i<out_img.ni() && j<out_img.nj())
        line_img.push_back(vgl_point_2d<double>(i,j));
    }
    if (line_img.size() < 2)
      continue;
    // record current line for later junction calculation
    roads_in_leaf.push_back(line_img);
    roads_in_leaf_props.push_back(osm.loc_lines()[r_idx]->prop());
    // expend the line to polygon given certain width
    if (width < 1.0) width = 1.1;
    vgl_polygon<double> img_poly;
    if (!volm_io_tools::expend_line(line_img, width, img_poly)) {
      vcl_cout << " expending osm line in tile " << tile_id() << ", leaf " << leaf_bbox_geo << " failed given width " << width << vcl_endl;
      return false;
    }
    // update the label
    cnt++;
    vgl_polygon_scan_iterator<double> it(img_poly, true);
    for (it.reset(); it.next();  ) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if ( x >= 0 && y >= 0 && x < out_img.ni() && y < out_img.nj()) {
          if (curr_level > level_img(x, y)) {
            level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
          }
        }
      }
    }
  }

  // ingest osm points
  cnt = 0;
  unsigned n_pts = osm.num_locs();
  vcl_vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
  for (unsigned  p_idx = 0; p_idx < n_pts; p_idx++) {
    vgl_point_2d<double> pt = loc_pts[p_idx]->loc();
    if (!leaf_bbox_geo.contains(pt))
      continue;
    // transfer from geo coord to image pixel
    unsigned char curr_level = loc_pts[p_idx]->prop().level_;
    unsigned char curr_id = loc_pts[p_idx]->prop().id_;
    double lx, ly, lz;
    lvcs->global_to_local(pt.x(), pt.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
    double i = lx - leaf_bbox.min_x();
    double j = leaf_bbox.max_y() - ly;
    int x = (int)i;  int y = (int)j;
    if (x>0 && y>0 && x<out_img.ni() && y<out_img.nj())
      if (curr_level > level_img(x,y)) {
        level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
      }
  }

  //// find and ingest osm junctions
  //unsigned n_road_in = roads_in_leaf.size();
  //for (unsigned r_idx = 0; r_idx < n_road_in; r_idx++) {
  //  vcl_vector<vgl_point_2d<double> > curr_rd = roads_in_leaf[r_idx];
  //  volm_land_layer curr_rd_prop = roads_in_leaf_props[r_idx];
  //  vcl_vector<vcl_vector<vgl_point_2d<double> > > net;
  //  vcl_vector<volm_land_layer> net_props;
  //  for (unsigned i = 0; i < n_road_in; i++)
  //    if (i != r_idx)
  //    {
  //      net.push_back(roads_in_leaf[i]);
  //      net_props.push_back(roads_in_leaf_props[i]);
  //    }
  //  // find all possible junction for current road
  //  vcl_vector<vgl_point_2d<double> > cross_pts;
  //  vcl_vector<volm_land_layer> cross_props;
  //  vcl_vector<volm_land_layer> cross_geo_props;
  //  if (!volm_io_tools::search_junctions(curr_rd, curr_rd_prop, net, net_props, cross_pts, cross_props, cross_geo_props)) {
  //    vcl_cout << "ERROR: find road junction for tile " << tile_id() << " leaf " << leaf_bbox_geo << " road " << r_idx << " failed\n";
  //    return false;
  //  }
  //  // ingest junction for current roads
  //  for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
  //    unsigned char curr_level = cross_props[c_idx].level_;
  //    unsigned char curr_id = cross_props[c_idx].id_;
  //    double radius = cross_props[c_idx].width_;
  //    int cx = (int)vcl_floor(cross_pts[c_idx].x() + 0.5);
  //    int cy = (int)vcl_floor(cross_pts[c_idx].y() + 0.5);
  //    for (int ii = cx-radius; ii < cx+radius; ii++)
  //      for (int jj = cy-radius; jj <cy+radius; jj++)
  //        if (ii >=0 && jj>=0 && ii <out_img.ni() && jj < out_img.nj())
  //          if (curr_level >= level_img(ii,jj)) {
  //            out_img(ii,jj)   = curr_id;
  //            level_img(ii,jj) = curr_level;
  //          }
  //  }
  //}

  // save the images
  vil_save(out_img, img_name.str().c_str());

  // save a color image for debug purpose
  vil_image_view<vil_rgb<vxl_byte> > out_class_img(ni, nj, 1);
  out_class_img.fill(volm_osm_category_io::volm_land_table[0].color_);
  vcl_stringstream color_name;
  color_name << out_folder() << "/osm_" << "N" << vcl_setprecision(6) << lat_min
                              << "W" << vcl_setprecision(6) << lon_min
                              << "_S" << scale_x << 'x' << scale_y << ".png";
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      out_class_img(i,j) = volm_osm_category_io::volm_land_table[out_img(i,j)].color_;
  vil_save(out_class_img, color_name.str().c_str());
  return true;
}
#endif

#if 0
// combine geo_cover image with urban_region images
int main(int argc, char** argv)
{
  vul_arg<vcl_string> geo_folder("-geo", "folder where geo_cover tif images stores", "");
  vul_arg<vcl_string> urban_folder("-urban", "folder where urban tif image stores", "");
  vul_arg<vcl_string> out_folder("-out", "output folder","");
  vul_arg<unsigned> world_id("-world", "world id for ROI (from 1 to 5",100);
  vul_arg_parse(argc, argv);

  if (geo_folder().compare("") == 0 || out_folder().compare("") == 0 || world_id() == 0 || urban_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return false;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;

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
    roots.push_back(volm_geo_index2::construct_tree<volm_osm_object_ids_sptr>(tiles[i], 1.0));

  for (unsigned i = 0; i < tile_size; i++) {
    vcl_cout << " tile " << i << " has tree root " << roots[i]->extent_ << vcl_endl;
    unsigned tree_depth = volm_geo_index2::depth(roots[i]);
    vcl_stringstream kml;  kml << out_folder() << "/p1b_wr" << world_id() << "_tile_" << i << "_depth_" << tree_depth << ".kml";
    volm_geo_index2::write_to_kml(roots[i], tree_depth, kml.str());
  }


  // load geo_cover images and sort them based on tile order
  vcl_vector<volm_img_info> info_tmp;
  volm_io_tools::load_geocover_imgs(geo_folder(), info_tmp);
  if (info_tmp.size() != tile_size) {
    log << "ERROR: mismatch in created tile and loaded geo cover image, check input world id and geo_folder\n";  error(log_file.str(), log.str());
    return false;
  }
  vcl_vector<volm_img_info> geo_infos;
  for(unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - info_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - info_tmp[j].bbox.min_y();
      double diff = vcl_sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        geo_infos.push_back(info_tmp[j]);
        break;
      }
    }
  }

  // load urban tif image and sort them based on tile order
  vcl_vector<volm_img_info> urban_tmp;
  volm_io_tools::load_urban_imgs(urban_folder(), urban_tmp);
  if (urban_tmp.size() != tile_size) {
    log << "ERROR: mismatch in created tile and loaded geo cover image, check input world id and urban_folder\n";  error(log_file.str(), log.str());
    return false;
  }
  vcl_vector<volm_img_info> urban_infos;
  for (unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - urban_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - urban_tmp[j].bbox.min_y();
      double diff = vcl_sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        urban_infos.push_back(urban_tmp[j]);
        break;
      }
    }
  }
  for (unsigned t_idx = 0; t_idx < tile_size; t_idx++)
  {
    // get geo cover image and urban image
    volm_img_info geo_cover = geo_infos[t_idx];
    volm_img_info urban_info = urban_infos[t_idx];
    vil_image_view<vxl_byte>* geo_img = dynamic_cast<vil_image_view<vxl_byte> * >(geo_cover.img_r.ptr());
    vil_image_view<vxl_byte>* urban_img = dynamic_cast<vil_image_view<vxl_byte> * >(urban_info.img_r.ptr());

    // create an image having same resolution with geo_cover image
    unsigned ni = (unsigned)geo_img->ni();
    unsigned nj = (unsigned)geo_img->nj();
    vil_image_view<vxl_byte> out_img(ni, nj, 1);
    out_img.fill(0);

    // copy geo_cover images
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        out_img(i,j) = volm_osm_category_io::geo_land_table[(*geo_img)(i,j)].id_;
      }
    }

    // ingest the urban image
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // transfer to lon and lat
        double lon, lat;
        geo_cover.cam->img_to_global(i, j, lon, lat);
        // obtain the pixel in urban image
        double u, v;
        urban_info.cam->global_to_img(lon, lat, 0.0, u, v);
        unsigned uu = (unsigned)vcl_floor(u+0.5);
        unsigned vv = (unsigned)vcl_floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < urban_info.ni && vv < urban_info.nj) {
          if ( (*urban_img)(uu,vv) == 255) {
            out_img(i,j) = volm_osm_category_io::geo_land_table[volm_osm_category_io::GEO_URBAN].id_;
          }
        }
      }
    }

    vcl_string img_name = out_folder() + "/" + geo_cover.name + "_urban.tif";
    vcl_cout << " image_name = " << vcl_endl;
    // save the images
    vil_save(out_img, img_name.c_str());
  }
  return true;

}
#endif

#if 0
// FOR phase 1B --> generate classification map using 1. GeoCover; 2. OSM map;  3. Satellite classification map
//  Note the 2D map size is defined by geo_index and the output image have 1 meter resolution
// generate classification map used to refine satellite height map
int main(int argc, char** argv)
{
  vul_arg<vcl_string> geo_folder("-geo", "folder where geo_cover tif images stores", "");
  vul_arg<vcl_string> osm_folder("-osm", "folder where osm binary stores", "");
  vul_arg<vcl_string> urban_folder("-urban", "folder where urban tif image stores", "");
  vul_arg<vcl_string> out_folder("-out", "output folder","");
  vul_arg<float> min_size ("-min", "minimum size of image size (in wgs84 degree)",0.0625);
  vul_arg<unsigned> world_id("-world", "world id for ROI (from 1 to 5",100);
  vul_arg<unsigned> tile_id("-tile", "tile id for ROI", 100);
  vul_arg<int> leaf_idx("-leaf", "leaf id of geo index for current tile (negative value will loop over all leaves", -1);
  vul_arg<vcl_string> class_img_folder("-class-img", "classification image from satellite modeling","");
  vul_arg<bool> is_satellite_building("-is-sat-building", "option to ingest satellite classification buildings into 2d land map", false);
  vul_arg<bool> is_osm_region("-is-osm-region", "option to ingest all osm regions", false);
  vul_arg<bool> is_osm_building("-is-osm-building", "option to ingest osm buildings into classification map",false);
  vul_arg<bool> is_osm_road("-is-osm-road", "option to ingest all osm road and junctions into classification map", false);
  vul_arg<bool> is_osm_pt("-is-osm-pt", "option to ingest all osm points into classification map", false);
  vul_arg_parse(argc, argv);

  // check the input
  if (geo_folder().compare("") == 0 || osm_folder().compare("") == 0 || out_folder().compare("") == 0 ||
      world_id() == 0 || urban_folder().compare("") == 0 || class_img_folder().compare("") == 0)
  {
    vul_arg_display_usage_and_exit();
    return false;
  }
  vcl_stringstream log_file;
  vcl_stringstream log;

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

  for (unsigned i = 0; i < tile_size; i++) {
    vcl_cout << " tile " << i << " has tree root " << roots[i]->extent_ << vcl_endl;
    unsigned tree_depth = volm_geo_index2::depth(roots[i]);
    vcl_stringstream kml;  kml << out_folder() << "/p1b_wr" << world_id() << "_tile_" << i << "_depth_" << tree_depth << ".kml";
    volm_geo_index2::write_to_kml(roots[i], tree_depth, kml.str());
  }

  // write the tree structure for sat 2d map
  for (unsigned i = 0; i < tile_size; i++) {
    vcl_stringstream out_txt;  out_txt << out_folder() << "/p1b_wr" << world_id() << "_tile_" << i << ".txt";
    volm_geo_index2::write(roots[i], out_txt.str(), min_size());
  }

  // load geo_cover images and sort them based on tile order
  vcl_vector<volm_img_info> info_tmp;
  volm_io_tools::load_geocover_imgs(geo_folder(), info_tmp);
  if (info_tmp.size() != tile_size) {
    log << "ERROR: mismatch in created tile and loaded geo cover image, check input world id and geo_folder\n";  error(log_file.str(), log.str());
    return false;
  }
  vcl_vector<volm_img_info> geo_infos;
  for(unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - info_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - info_tmp[j].bbox.min_y();
      double diff = vcl_sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        geo_infos.push_back(info_tmp[j]);
        break;
      }
    }
  }

  // load urban tif image and sort them based on tile order
  vcl_vector<volm_img_info> urban_tmp;
  volm_io_tools::load_urban_imgs(urban_folder(), urban_tmp);
  if (urban_tmp.size() != tile_size) {
    log << "ERROR: mismatch in created tile and loaded geo cover image, check input world id and urban_folder\n";  error(log_file.str(), log.str());
    return false;
  }
  vcl_vector<volm_img_info> urban_infos;
  for (unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - urban_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - urban_tmp[j].bbox.min_y();
      double diff = vcl_sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        urban_infos.push_back(urban_tmp[j]);
        break;
      }
    }
  }

  // load the satellite classification images
  vcl_vector<volm_img_info> class_img_infos;
  volm_io_tools::load_imgs(class_img_folder(), class_img_infos, true, true, true);

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
           << geo_infos.size() << " geo_cover images, "
           << class_img_infos.size() << " satellite class images and "
           << osm_objs.size() << " open street map dataset " << vcl_flush << vcl_endl;
  if (tile_id() > tiles.size()) {
    log << "ERROR: given tile id " << tile_id() << " does not exist\n";  error(log_file.str(), log.str());
    return false;
  }
  // create 2d map
  for (unsigned t_idx = 0; t_idx < tile_size; t_idx++)
  {
    if (t_idx != tile_id())
      continue;
    // retrieve leaves for current tile
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    volm_geo_index2::get_leaves(roots[t_idx], leaves);
    volm_osm_objects osm = osm_objs[t_idx];
    volm_img_info geo_cover = geo_infos[t_idx];
    volm_img_info urban_info = urban_infos[t_idx];
    vil_image_view<vxl_byte>* geo_img = dynamic_cast<vil_image_view<vxl_byte> * >(geo_cover.img_r.ptr());
    vil_image_view<vxl_byte>* urban_img = dynamic_cast<vil_image_view<vxl_byte> * >(urban_info.img_r.ptr());

    vcl_cout << " geo_cover img_name: " << geo_cover.img_name << " geo_cover name: " << geo_cover.name << vcl_endl;
    vcl_cout << " geo_cover bounding box " << geo_cover.bbox << vcl_endl;
    vcl_cout << " urban img_name: " << urban_info.img_name << " urban name: " << urban_info.name << vcl_endl;
    vcl_cout << " urban bounding box: " << urban_info.bbox << vcl_endl;
    vcl_cout << " urban image size: " << urban_info.ni << "x" << urban_info.nj << vcl_endl;
    vcl_cout << " chosen leaf: " << leaf_idx() << vcl_endl;

    for (unsigned l_idx = 0; l_idx < (unsigned)leaves.size(); l_idx++) {
      vcl_cout << " leaf_id = " << l_idx << " --> bounding box = " << leaves[l_idx]->extent_ << vcl_endl;
    }


    // create a 2d image for each leaf at desired size
    for (unsigned l_idx = 0; l_idx < (unsigned)leaves.size(); l_idx++) {
      if (leaf_idx() > 0 && leaf_idx() < (int)leaves.size())
        if ((int)l_idx != leaf_idx())
          continue;
      // calculate desired resolution
      volm_geo_index2_node_sptr leaf = leaves[l_idx];
      double lon_min, lat_min, lon_max, lat_max;
      lon_min = leaf->extent_.min_x();  lat_min = leaf->extent_.min_y();
      lon_max = leaf->extent_.max_x();  lat_max = leaf->extent_.max_y();
      double scale_x = lon_max-lon_min;
      double scale_y = lat_max-lat_min;
      vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat_min, lon_min, 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      double box_lx, box_ly, box_lz;
      lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);
      unsigned ni = (unsigned)vcl_ceil(box_lx);
      unsigned nj = (unsigned)vcl_ceil(box_ly);
      // form bounding box for current leaf in local coordinate
      vgl_box_2d<double> leaf_bbox_geo = leaf->extent_;
      vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);

      // create an associate camera tfw for current image
      vnl_matrix<double> trans_matrix(4,4,0.0);
      trans_matrix[0][0] = scale_x/ni;
      trans_matrix[1][1] = -scale_y/nj;
      trans_matrix[0][3] = lon_min;
      trans_matrix[1][3] = lat_max;
      vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
      cam->set_scale_format(true);
      vcl_stringstream cam_name;
      cam_name << out_folder() << "/Sat2dMap_" << tiles[t_idx].hemisphere() << vcl_setprecision(12) << lat_min
                               << tiles[t_idx].direction() << vcl_setprecision(12) << lon_min
                               << "_S" << scale_x << 'x' << scale_y << ".tfw";
      cam->save_as_tfw(cam_name.str());

      // create 2d image for current leaf
      vil_image_view<vxl_byte> out_img(ni, nj, 1);
      vil_image_view<vxl_byte> level_img(ni, nj, 1);

      vcl_stringstream img_name;
      img_name << out_folder() << "/Sat2dMap_" << tiles[t_idx].hemisphere() << vcl_setprecision(12) << lat_min
                               << tiles[t_idx].direction() << vcl_setprecision(12) << lon_min
                               << "_S" << scale_x << 'x' << scale_y << ".tif";

      out_img.fill(0);
      level_img.fill(0);

      vcl_cout << " tile " << t_idx << " leaf " << l_idx << " has geo boundary " << leaf_bbox_geo
               << " corresponding to image size " << out_img.ni() << 'x' << out_img.nj() << vcl_endl;

      // ingest geo_cover for the image
      for (unsigned i = 0; i < ni; i++) {
        for (unsigned j = 0; j < nj; j++) {
          // transfer coords to get pixel in geo cover image
          double lon, lat, gz;
          float local_x = (float)(i+0+0.5);
          float local_y = (float)(box_ly-j+0.5);
          lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
          double u, v;
          geo_cover.cam->global_to_img(lon, lat, gz, u, v);
          unsigned uu = (unsigned)vcl_floor(u+0.5);
          unsigned vv = (unsigned)vcl_floor(v+0.5);
          if (uu < geo_cover.ni && vv < geo_cover.nj)
            out_img(i,j) = volm_osm_category_io::geo_land_table[(*geo_img)(uu,vv)].id_;
        }
      }

      // ingest urban image to add extra urban region
      // Note that in urban image, three pixel values exist, 0, 1 and 255.  255 refers to urban region
      for (unsigned i = 0; i < ni; i++) {
        for (unsigned j = 0; j < nj; j++) {
          // transfer coords to get pixel in urban image
          double lon, lat, gz;
          float local_x = (float)(i+0+0.5);
          float local_y = (float)(box_ly-j+0.5);
          lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
          double u, v;
          urban_info.cam->global_to_img(lon, lat, gz, u, v);
          unsigned uu = (unsigned)vcl_floor(u+0.5);
          unsigned vv = (unsigned)vcl_floor(v+0.5);
          if (uu > 0 && vv > 0 && uu < urban_info.ni && vv < urban_info.nj) {
            if ( (*urban_img)(uu,vv) == 255) {
              out_img(i,j) = volm_osm_category_io::geo_land_table[volm_osm_category_io::GEO_URBAN].id_;
            }
          }
        }
      }

      // ingest satellite classification buildings if necessary
      if (is_satellite_building())
      {
        // find overlapped satellite class images
        vcl_vector<volm_img_info> sat_img_infos;
        for (unsigned s_idx = 0; s_idx < class_img_infos.size(); s_idx++) {
          if (!vgl_intersection(leaf_bbox_geo, class_img_infos[s_idx].bbox).is_empty())
            sat_img_infos.push_back(class_img_infos[s_idx]);
        }
        if (sat_img_infos.size() != 0)
        {
          vcl_cout << "there are " << sat_img_infos.size() << " satellite class images intersect with current 2d map leaf: " << leaf_bbox_geo << vcl_endl;
          vcl_cout << "\t they are: \n";
          for (unsigned c_idx = 0; c_idx < sat_img_infos.size(); c_idx++) {
            vcl_cout << "\t\t" << sat_img_infos[c_idx].name << " ---> " << sat_img_infos[c_idx].bbox << vcl_endl;
          }
          for (unsigned i = 0; i < ni; i++) {
            for (unsigned j = 0; j < nj; j++) {
              double lon, lat, gz;
              float local_x = (float)(i+0+0.5);
              float local_y = (float)(box_ly-j+0.5);
              lvcs->local_to_global(local_x, local_y, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
              bool is_pixel_found = false;
              for (unsigned c_idx = 0; (c_idx < sat_img_infos.size() && !is_pixel_found); c_idx++)
              {
                double u, v;
                sat_img_infos[c_idx].cam->global_to_img(lon, lat, gz, u, v);
                unsigned uu = (unsigned)vcl_floor(u+0.5);
                unsigned vv = (unsigned)vcl_floor(v+0.5);
                if (uu < sat_img_infos[c_idx].ni && vv < sat_img_infos[c_idx].nj) {
                  is_pixel_found = true;  // to avoid overlap region twice
                  vil_image_view<vxl_byte> imgc(sat_img_infos[c_idx].img_r);
                  if (imgc(uu,vv) == volm_osm_category_io::volm_land_table_name["building"].id_ ||
                      imgc(uu,vv) == volm_osm_category_io::volm_land_table_name["tall_building"].id_ ) {
                    unsigned char curr_id = imgc(uu,vv);
                    unsigned char curr_level = volm_osm_category_io::volm_land_table[curr_id].level_;
                    if (curr_level >= level_img(i,j)) {
                      out_img(i,j) = curr_id;  level_img(i,j) = curr_level;
                      break;
                    }
                  }
                }
              }
            }
          }
        }
#if 0
        // find the overlapped leaves
        vcl_vector<volm_geo_index2_node_sptr> class_leaves;
        volm_geo_index2::get_leaves(class_root, class_leaves, leaf_bbox_geo);
        if (class_leaves.empty()) {
          vcl_cout << "no satellite class map intersect with current 2d map" << vcl_endl;
        }
        else
        {
          // load the satellite class map, camera and store them accordingly
          vcl_vector<vcl_string> leaf_strings;
          vcl_vector<vgl_box_2d<double> > leaf_boxes;
          for (vcl_vector<volm_geo_index2_node_sptr>::iterator c_vit = class_leaves.begin();
               c_vit != class_leaves.end(); ++c_vit)
          {
            vcl_string leaf_string = (*c_vit)->get_string();
            vcl_string c_img_name = class_img_folder() + "/scene_" + leaf_string + "_ortho_class.tif";
            if (!vul_file::exists(c_img_name)) {
              continue;
            }
            leaf_strings.push_back(leaf_string);
            leaf_boxes.push_back((*c_vit)->extent_);
            // current class image/cam has been loaded before, ignore
            if (class_img_map.find(leaf_string) != class_img_map.end())
              continue;
            // load the image
            vil_image_view<vxl_byte> c_img = vil_load(c_img_name.c_str());
            class_img_map.insert(vcl_pair<vcl_string, vil_image_view<vxl_byte> >(leaf_string, c_img));
            // load the camera
            vcl_string c_cam_tfw = class_cam_folder() + "/scene_" + leaf_string + "_ortho_cam_geo.tfw";
            if (!vul_file::exists(c_cam_tfw)) {
              log << "ERROR: can not find satellite ortho camera: " << c_img_name << '\n';  error(log_file.str(), log.str());
              return volm_io::EXE_ARGUMENT_ERROR;
            }
            vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs;
            vpgl_geo_camera* c_cam;
            // pass a dummy lvcs and 0 utm_zone to create camera
            vpgl_geo_camera::init_geo_camera(c_cam_tfw, dummy_lvcs, 0, 0, c_cam);
            class_cam_map.insert(vcl_pair<vcl_string, vpgl_geo_camera*>(leaf_string, c_cam));
          }
          vcl_cout << "there are " << leaf_strings.size() << " class maps intersect with current 2d map" << vcl_endl;
          vcl_cout << "\t they are: \n";
          for (unsigned c_idx = 0; c_idx < leaf_strings.size(); c_idx++) {
            vcl_cout << " \t\t " << leaf_strings[c_idx] << " ---> " << leaf_boxes[c_idx] << vcl_endl;
          }
          if (leaf_strings.size() != 0) {
            for (unsigned i = 0; i < ni; i++) {
              //vcl_cout << "i = " << i << "," << vcl_flush;
              for (unsigned j = 0; j < nj; j++) {

                double lon, lat, gz;
                float local_x = (float)(i+0+0.5);
                float local_y = (float)(box_ly-j+0.5);
                lvcs->local_to_global(local_x, local_y, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
                bool is_pixel_found = false;
                for (unsigned c_idx = 0; (c_idx < leaf_strings.size() && !is_pixel_found); c_idx++)
                {
                  //if (!leaf_boxes[c_idx].contains(lon, lat))
                  //  continue;
                  double u, v;
                  class_cam_map[leaf_strings[c_idx]]->global_to_img(lon, lat, gz, u, v);
                  unsigned uu = (unsigned)vcl_floor(u+0.5);
                  unsigned vv = (unsigned)vcl_floor(v+0.5);
                  if (uu > 0 && vv > 0 && uu < class_img_map[leaf_strings[c_idx]].ni() && vv < class_img_map[leaf_strings[c_idx]].nj()) {
                    is_pixel_found = true;  // to avoid handling overlap region twice
                    // only ingest buildings satellite classification map
                    if (class_img_map[leaf_strings[c_idx]](uu,vv) == volm_osm_category_io::volm_land_table_name["building"].id_)
                    {
                      unsigned char curr_id = class_img_map[leaf_strings[c_idx]](uu,vv);
                      unsigned char curr_level = volm_osm_category_io::volm_land_table[curr_id].level_;
                      if (curr_level >= level_img(i,j)) {
                        out_img(i,j) = curr_id; level_img(i,j) = curr_level;
                        break;
                      }
                    }
                  }
                }
              }
            }
          }
        }
#endif
      }

      unsigned cnt = 0;

      // ingest osm regions if necessary
      if (is_osm_region())
      {
        unsigned num_regions = osm.num_regions();
        for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
          vgl_polygon<double> poly(osm.loc_polys()[r_idx]->poly()[0]);
          // get rid off polygon with duplicated points
          bool ignore = false;
          for (unsigned i = 0; i < poly[0].size()-1; i++) {
            if (poly[0][i] == poly[0][i+1])
              ignore = true;
          }
          if (ignore)
            continue;
          // check whether the region intersect with current leaf
          if (!vgl_intersection(leaf->extent_, poly))
            continue;
          unsigned char curr_level = osm.loc_polys()[r_idx]->prop().level_;
          // geo cover is already level 0 and therefore anything in osm with level 0 is ignored
          if (curr_level == 0)
            continue;
          // go from geo coords wgs84 to local
          vgl_polygon<double> img_poly(1);
          unsigned char curr_id = osm.loc_polys()[r_idx]->prop().id_;
          if (curr_id != volm_osm_category_io::volm_land_table_name["building"].id_ && is_osm_building())  // only ingest buildings if necessary
            continue;
          for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
            double lx, ly, lz;
            lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
            double i = lx;  double j = box_ly - ly;
            img_poly[0].push_back(vgl_point_2d<double>(i,j));
          }
          // using polygon iterator to loop over all points inside the polygon and intersect with leaf
          cnt++;
          vgl_polygon_scan_iterator<double> it(img_poly, true);
          for (it.reset(); it.next();  )
          {
            int y = it.scany();
            for (int x = it.startx(); x <= it.endx(); ++x) {
              if (x >=0 && y >= 0 && x < (int)out_img.ni() && y < (int)out_img.nj()) {
                if (curr_level >= level_img(x,y)) {
                  out_img(x,y) = curr_id;  level_img(x,y) = curr_level;
                }
              }
            }
          }
        }
      }

      // ingest osm roads and junctions if necessary
      if (is_osm_road())
      {
        cnt = 0;
        unsigned num_roads = osm.num_roads();
        vcl_vector<vcl_vector<vgl_point_2d<double> > > roads_in_leaf;
        vcl_vector<volm_land_layer> roads_in_leaf_props;
        for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
          vcl_vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
          vcl_vector<vgl_point_2d<double> > line_geo;
          // check and obtain the road segment that lies inside the leaf
          if (!volm_io_tools::line_inside_the_box(leaf_bbox_geo, road, line_geo))
            continue;

          // go from geo coords to leaf local
          vcl_vector<vgl_point_2d<double> > line_img;
          unsigned char curr_level = osm.loc_lines()[r_idx]->prop().level_;
          unsigned char curr_id = osm.loc_lines()[r_idx]->prop().id_;
          double width = osm.loc_lines()[r_idx]->prop().width_;
          for (unsigned pt_idx = 0; pt_idx < line_geo.size(); pt_idx++) {
            double lx, ly, lz;
            lvcs->global_to_local(line_geo[pt_idx].x(), line_geo[pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
            double i = lx - leaf_bbox.min_x();
            double j = leaf_bbox.max_y() - ly;
            if (i>=0 && j>=0 && i<out_img.ni() && j<out_img.nj())
              line_img.push_back(vgl_point_2d<double>(i,j));
          }
          if (line_img.size() < 2)
            continue;

          // record current line for later junction calculation
          roads_in_leaf.push_back(line_img);
          roads_in_leaf_props.push_back(osm.loc_lines()[r_idx]->prop());
          // expend the line to polygon given certain width
          if (width < 1.0) width = 1.1;
          vgl_polygon<double> img_poly;
          if (!volm_io_tools::expend_line(line_img, width, img_poly)) {
            log << " expending osm line in tile " << t_idx << ", leaf " << leaf->extent_ << " failed given width " << width << vcl_endl;
            error(log_file.str(), log.str());
            return false;
          }
          // update the label
          cnt++;
          vgl_polygon_scan_iterator<double> it(img_poly, true);
          for (it.reset(); it.next();  ) {
            int y = it.scany();
            for (int x = it.startx(); x <= it.endx(); ++x) {
              if ( x >= 0 && y >= 0 && x < (int)out_img.ni() && y < (int)out_img.nj()) {  // osm road will overwrite everything
                level_img(x,y) = curr_level;  out_img(x,y) = curr_id;
                //if (curr_level > level_img(x, y)) {
                //  level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
                //}
              }
            }
          }
        }
        // find and ingest osm junctions
        unsigned n_road_in = roads_in_leaf.size();
        for (unsigned r_idx = 0; r_idx < n_road_in; r_idx++) {
          vcl_vector<vgl_point_2d<double> > curr_rd = roads_in_leaf[r_idx];
          volm_land_layer curr_rd_prop = roads_in_leaf_props[r_idx];
          vcl_vector<vcl_vector<vgl_point_2d<double> > > net;
          vcl_vector<volm_land_layer> net_props;
          for (unsigned i = 0; i < n_road_in; i++)
            if (i != r_idx)
            {
              net.push_back(roads_in_leaf[i]);
              net_props.push_back(roads_in_leaf_props[i]);
            }
          // find all possible junction for current road
          vcl_vector<vgl_point_2d<double> > cross_pts;
          vcl_vector<volm_land_layer> cross_props;
          vcl_vector<volm_land_layer> cross_geo_props;
          if (!volm_io_tools::search_junctions(curr_rd, curr_rd_prop, net, net_props, cross_pts, cross_props, cross_geo_props)) {
            log << "ERROR: find road junction for tile " << t_idx << " leaf " << leaf_bbox_geo << " road " << r_idx << " failed\n";
            error(log_file.str(), log.str());
            return false;
          }
          // ingest junction for current roads
          for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
            unsigned char curr_level = cross_props[c_idx].level_;
            unsigned char curr_id = cross_props[c_idx].id_;
            double radius = cross_props[c_idx].width_;
            int cx = (int)vcl_floor(cross_pts[c_idx].x() + 0.5);
            int cy = (int)vcl_floor(cross_pts[c_idx].y() + 0.5);
            for (int ii = cx-radius; ii < cx+radius; ii++)
              for (int jj = cy-radius; jj <cy+radius; jj++)
                if (ii >=0 && jj>=0 && ii < (int)out_img.ni() && jj < (int)out_img.nj())
                  if (curr_level >= level_img(ii,jj)) {
                    out_img(ii,jj)   = curr_id;
                    level_img(ii,jj) = curr_level;
                  }
          }
        }
      }

      // ingest osm points if necessary
      if (is_osm_pt())
      {
        cnt = 0;
        unsigned n_pts = osm.num_locs();
        vcl_vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
        for (unsigned  p_idx = 0; p_idx < n_pts; p_idx++) {
          vgl_point_2d<double> pt = loc_pts[p_idx]->loc();
          if (!leaf_bbox_geo.contains(pt))
            continue;
          // transfer from geo coord to image pixel
          unsigned char curr_level = loc_pts[p_idx]->prop().level_;
          unsigned char curr_id = loc_pts[p_idx]->prop().id_;
          double lx, ly, lz;
          lvcs->global_to_local(pt.x(), pt.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
          double i = lx - leaf_bbox.min_x();
          double j = leaf_bbox.max_y() - ly;
          int x = (int)i;  int y = (int)j;
          if (x>0 && y>0 && x<(int)out_img.ni() && y<(int)out_img.nj())
            if (curr_level >= level_img(x,y)) {
              level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
            }
        }
      }

      // save the images
      vil_save(out_img, img_name.str().c_str());
#if 1
      // save a color image for debug purpose
      vil_image_view<vil_rgb<vxl_byte> > out_class_img(ni, nj, 1);
      out_class_img.fill(volm_osm_category_io::volm_land_table[0].color_);
      vcl_stringstream color_name;
      color_name << out_folder() << "/Sat2dMap_" << tiles[t_idx].hemisphere() << vcl_setprecision(12) << lat_min
                                 << tiles[t_idx].direction() << vcl_setprecision(12) << lon_min
                                 << "_S" << scale_x << 'x' << scale_y << ".png";
      for (unsigned i = 0; i < ni; i++)
        for (unsigned j = 0; j < nj; j++)
          out_class_img(i,j) = volm_osm_category_io::volm_land_table[out_img(i,j)].color_;
      vil_save(out_class_img, color_name.str().c_str());
#endif
    } // end of each leaf
  } // end of each tile

  return true;
}
#endif

#if 1
  // For Phase 1A, generate 2D land map using NLCD, LIDAR(elev), SME(fort), URGENT(building), OSM data
  // Output GeoTiff image size is controlled by geo index and has 1 meter resolution
  // Pipeline: 1.  --> generate label using NLCD + LIDAR data (LIDAR elevation is used to refined beach/water boundary)
  //           2.  --> put OSM data onto the basic label map (road network needs to be put onto the map first)
  //           3.  --> put URGENT building, SME data onto OSM label image
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  nlcd_folder("-nlcd", "folder where all NLCD image are", "");
  vul_arg<vcl_string> lidar_folder("-lidar", "folder where LIDAR image are stored", "");
  vul_arg<vcl_string>   osm_folder("-osm", "folder where OSM data are stored", "");
  vul_arg<vcl_string> build_folder("-build", "folder where URGENT data are stored", "");
  vul_arg<vcl_string>   sme_folder("-sme", "folder where SME fort data are stored", "");
  vul_arg<vcl_string>   out_folder("-out", "output folder", "");
  vul_arg<vcl_string>    world_str("-world", "world name of ROI", "coast");
  vul_arg<unsigned>        tile_id("-tile", "tile id of ROI", 100);
  vul_arg<float>          min_size("-min", "minimum size of the geo index (in wgs84)", 0.0625);
  vul_arg<int>            leaf_idx("-leaf", "leaf id inside tile (for parallel execution)", -1);
  vul_arg<vcl_string>      roi_kml("-roi", "kml file storing the ROI region polygon", "");
  vul_arg_parse(argc, argv);

  // input check
  if (nlcd_folder().compare("") == 0 || lidar_folder().compare("") == 0 || osm_folder().compare("") == 0 || roi_kml().compare("") == 0 ||
      build_folder().compare("") == 0 || sme_folder().compare("") == 0 || out_folder().compare("") == 0 ||
      tile_id() == 100 )
  {  vul_arg_display_usage_and_exit();  return false;  }

  if (world_str().compare("coast") != 0) {
    vcl_cout << "ERROR: input ROI world: " << world_str() << " is not supported (only coast allowed)" << vcl_endl;
    return false;
  }

  vcl_string log_file = out_folder() + "/log_osm_2d_map_wr_" + world_str() + ".xml";
  vcl_stringstream log;
  unsigned t_id = tile_id();

  vcl_string phase_str;
  if (world_str() == "desert" || world_str() == "coast" || world_str() == "Desert" || world_str() == "Coast")
    phase_str = "a";
  else if (world_str() == "chile" || world_str() == "india" || world_str() == "jordan" || world_str() == "philippines" || world_str() == "taiwan" ||
           world_str() == "Chile" || world_str() == "India" || world_str() == "Jordan" || world_str() == "Philippines" || world_str() == "Taiwan")
    phase_str = "b";
  else {
    log << "ERROR: unknown ROI world: " << world_str() << '\n';  error(log_file, log.str());
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned world_id;
  if (world_str() == "Desert" || world_str() == "desert")                 world_id = 1;
  else if (world_str() == "Coast" || world_str() == "coast")              world_id = 2;
  else if (world_str() == "Chile" || world_str() == "chile")              world_id = 1;
  else if (world_str() == "India" || world_str() == "india")              world_id = 2;
  else if (world_str() == "Jordan" || world_str() == "jordan")            world_id = 3;
  else if (world_str() == "Philippines" || world_str() == "philippines")  world_id = 4;
  else if (world_str() == "Taiwan" || world_str() == "taiwan")            world_id = 5;

  // create volm_geo_index2 for given ROI world
  vgl_polygon<double> roi_poly = bkml_parser::parse_polygon(roi_kml());
  vcl_vector<volm_tile> tiles;
  if (world_str().compare("coast") == 0)  tiles = volm_tile::generate_p1_wr2_tiles();
  unsigned tile_size = tiles.size();
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_osm_object_ids_sptr>(tiles[t_id], min_size(), roi_poly);
  vcl_cout << "Create geo index with min_size: " << min_size() << vcl_endl;

  unsigned tree_depth = volm_geo_index2::depth(root);
  vcl_stringstream kml;  kml << out_folder() << "/p1a_" << world_str() << "_tile_" << t_id << "_depth_" << tree_depth << ".kml";
  volm_geo_index2::write_to_kml(root, tree_depth, kml.str());
  vcl_stringstream root_txt;  root_txt << out_folder() << "/p1a_" << world_str() << "_tile_" << t_id << ".txt";

  // load NLCD images and sort them based on tile order
  vcl_vector<volm_img_info> info_tmp;
  volm_io_tools::load_nlcd_imgs(nlcd_folder(), info_tmp);
  if (info_tmp.size() != tile_size) {
    log << "ERROR: mismatch in create tiles " << tiles.size() << " and loaded " << info_tmp.size() << " nlcd images, check input nlcd folder\n";  error(log_file, log.str());
    return false;
  }
  vcl_vector<volm_img_info> nlcd_infos;
  for (unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - info_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - info_tmp[j].bbox.min_y();
      double diff = vcl_sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        nlcd_infos.push_back(info_tmp[j]);
        break;
      }
    }
  }


  // load Lidar images
  vcl_vector<volm_img_info> lidar_infos;
  volm_io_tools::load_lidar_imgs(lidar_folder(), lidar_infos);
  vcl_cout << "read " << lidar_infos.size() << " LIDAR images!" << vcl_endl;

  // load sme data
  vcl_string glob = sme_folder() + "/*.csv";
  vcl_vector<vcl_pair<vgl_point_2d<double>, int> > sme_objects;
  for (vul_file_iterator fit = glob; fit; ++fit)
    volm_io::read_sme_file(fit(), sme_objects);

  // load urgent building data
  glob = build_folder() + "/*.csv";
  vcl_vector<vcl_pair<vgl_polygon<double>, vgl_point_2d<double> > > build_polys;
  vcl_vector<double> build_heights;
  for (vul_file_iterator fit = glob; fit; ++fit) {
    volm_io::read_building_file(fit(), build_polys, build_heights);
  }

  // load open street map binary
  vcl_stringstream osm_file;  osm_file << osm_folder() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << t_id << "_osm.bin";
  if (!vul_file::exists(osm_file.str())) {
    log << "ERROR: can not find osm binary file: " << osm_file.str() << '\n';  error(log_file, log.str());
    return false;
  }
  volm_osm_objects osm(osm_file.str());

  vcl_cout << " --------------- START -----------------" << vcl_endl;
  vcl_cout << "generate 2d land category map for tile " << t_id << vcl_endl;
  vcl_cout << "read " << nlcd_infos.size() << " NLCD images!" << vcl_endl;
  vcl_cout << "read " << lidar_infos.size() << " LIDAR images!" << vcl_endl;
  vcl_cout << "read " << sme_objects.size() << " SME objects!" << vcl_endl;
  vcl_cout << "read " << build_polys.size() << " URGENT buildings!" << vcl_endl;
  vcl_cout << "read " << osm.num_locs() << " OSM loc points, " << osm.num_roads() << " OSM roads and " << osm.num_regions() << " OSM regions" << vcl_endl;


  // start to create 2d map
  volm_img_info nlcd_info = nlcd_infos[t_id];
  vil_image_view<vxl_byte>* nlcd_img = dynamic_cast<vil_image_view<vxl_byte>*>(nlcd_info.img_r.ptr());
  if (!nlcd_img) {
    log << "ERROR: unsupported NLCD image pixel format: " << nlcd_info.img_r->pixel_format() << '\n';  error(log_file, log.str());
    return false;
  }
  vcl_cout << "NLCD image covers the given tile: " << nlcd_info.img_name << vcl_endl;
  // retrieve leaves for current tile
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  vcl_cout << leaves.size() << " leaves require map creation" << vcl_endl;
  // create 2d map for each leaf at desired size
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
  {
    if (leaf_idx() >= 0 && leaf_idx() < leaves.size())
      if (l_idx != leaf_idx())
        continue;
    // calculate desired resolution
    volm_geo_index2_node_sptr leaf = leaves[l_idx];
    double lon_min, lat_min, lon_max, lat_max;
    lon_min = leaf->extent_.min_x();  lat_min = leaf->extent_.min_y();
    lon_max = leaf->extent_.max_x();  lat_max = leaf->extent_.max_y();
    double scale_x = lon_max - lon_min;
    double scale_y = lat_max - lat_min;
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat_min, lon_min, 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    double box_lx, box_ly, box_lz;
    lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);
    unsigned ni = (unsigned)vcl_ceil(box_lx);
    unsigned nj = (unsigned)vcl_ceil(box_ly);
    vgl_box_2d<double> leaf_bbox_geo = leaf->extent_;
    vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);
    // create geo camera for output image
    vnl_matrix<double> trans_matrix(4,4,0.0);
    trans_matrix[0][0] = scale_x/ni;    trans_matrix[1][1] = -scale_y/nj;
    trans_matrix[0][3] = lon_min;       trans_matrix[1][3] = lat_max;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    cam->set_scale_format(true);
    vcl_stringstream cam_name;
    cam_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << vcl_setprecision(12) << lat_min
                                              << tiles[t_id].direction()  << vcl_setprecision(12) << lon_min
                                              << "_S" << scale_x << 'x' << scale_y << ".tfw";
    cam->save_as_tfw(cam_name.str());
    // create 2d image for current leaf
    vil_image_view<vxl_byte>   out_img(ni, nj, 1);
    vil_image_view<vxl_byte> level_img(ni, nj, 1);

    vcl_stringstream img_name;
    img_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << vcl_setprecision(12) << lat_min
                             << tiles[t_id].direction() << vcl_setprecision(12) << lon_min
                             << "_S" << scale_x << 'x' << scale_y << ".tif";
    out_img.fill(0);  level_img.fill(0);
    vcl_cout << "leaf " << l_idx << " has geo boundary " << leaf_bbox_geo << " --> image size " << out_img.ni() << 'x' << out_img.nj() << vcl_endl;

    // obtain LIDAR images that intersect with current leaf
    vcl_vector<vcl_pair<vil_image_view<float>, vpgl_geo_camera*> > lidar_imgs;
    for (vcl_vector<volm_img_info>::iterator vit = lidar_infos.begin(); vit != lidar_infos.end(); ++vit) {
      if (vgl_intersection(vit->bbox, leaf_bbox_geo).area() <= 0.0)
        continue;
      if (vit->img_r->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
        log << "ERROR: unsupported LIDAR image pixel format: " << vit->img_r->pixel_format() << '\n';  error(log_file, log.str());
        return false;
      }
      vil_image_view<float> img(vit->img_r);
      lidar_imgs.push_back(vcl_pair<vil_image_view<float>, vpgl_geo_camera*>(img, vit->cam));
    }
    vcl_cout << lidar_imgs.size() << " LIDAR image intersect with the leaf " << l_idx << vcl_endl;
    // ingest NLCD image and refine the beach/water boundary by LIDAR elevation
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // obtain global lon/lat
        double lon, lat, gz;
        float local_x = (float)(i+0.5);  float local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        // find NLCD pixel
        unsigned char label = volm_osm_category_io::volm_land_table_name["invalid"].id_;
        double u, v;
        nlcd_info.cam->global_to_img(lon, lat, gz, u, v);
        unsigned uu = (unsigned)vcl_floor(u+0.5);  unsigned vv = (unsigned)vcl_floor(v+0.5);
        if (uu > 0 && vv > 0 && uu < nlcd_img->ni() && vv < nlcd_img->nj()) {
          label = (*nlcd_img)(uu, vv);
          unsigned char land_label = volm_osm_category_io::nlcd_land_table[label].id_;
          out_img(i,j) = land_label;
        }
      }
    }

#if 1
    unsigned cnt = 0;
    // ingest OSM regions
    unsigned num_regions = osm.num_regions();
    for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
      vgl_polygon<double> poly(osm.loc_polys()[r_idx]->poly()[0]);
      // get rid off polygon with duplicated points
      bool ignore = false;
      for (unsigned i = 0; i < poly[0].size()-1; i++)
        if (poly[0][i] == poly[0][i+1])  ignore = true;
      if (ignore)  continue;
      // check whether the region intersects with current leaf
      if (!vgl_intersection(leaf->extent_, poly))
        continue;
      unsigned char curr_level = osm.loc_polys()[r_idx]->prop().level_;
      // NLCD is already level 0 and therefore anything in osm with level 0 is ignored
      if (curr_level == 0)  continue;
      // transfer to image pixel
      vgl_polygon<double> img_poly(1);
      unsigned char curr_id = osm.loc_polys()[r_idx]->prop().id_;
      //// remove buildings from osm
      //if (curr_id == volm_osm_category_io::volm_land_table_name["building"].id_)
      //  continue;
      for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
        double lx, ly, lz;
        lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double i = lx;  double j = box_ly - ly;
        img_poly[0].push_back(vgl_point_2d<double>(i,j));
      }
      // ingest using polygon iterator
      cnt++;
      vgl_polygon_scan_iterator<double> it(img_poly, true);
      for (it.reset(); it.next();  ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x) {
          if (x >= 0 && y >= 0 && x < out_img.ni() && y < out_img.nj()) {
            if (curr_level > level_img(x, y))
            {  out_img(x,y) = curr_id;  level_img(x, y) = curr_level;  }
          }
        }
      }
    }
    vcl_cout << cnt << " OSM regions are ingested into Land map" << vcl_endl;

    // refine the water-sand boundary using lidar elev value
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // obtain global lon/lat
        double lon, lat, gz;
        float local_x = (float)(i+0.5);  float local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        float elev = 100.0f;
        bool found = false;
        for (unsigned k = 0; k < lidar_imgs.size(); k++) {
          double u, v;
          lidar_imgs[k].second->global_to_img(lon, lat, gz, u, v);
          unsigned uu = (unsigned)vcl_floor(u+0.5);  unsigned vv = (unsigned)vcl_floor(v+0.5);
          if (uu > 0 && vv > 0 && uu < lidar_imgs[k].first.ni() && vv < lidar_imgs[k].first.nj()) {
            elev = lidar_imgs[k].first(uu, vv);  found = true;
            break;
          }
        }
        if (out_img(i,j) == volm_osm_category_io::volm_land_table_name["Barren_Land/Beach"].id_ && found) {
          if (elev < 0.5f && found)
            out_img(i,j) = volm_osm_category_io::volm_land_table_name["Open_Water"].id_;
        }
        else if (out_img(i,j) == volm_osm_category_io::volm_land_table_name["Woody_Wetlands/Marina"].id_ && found) {
          if (elev < 0.5f && found)
            out_img(i,j) = volm_osm_category_io::volm_land_table_name["Open_Water"].id_;
        }
        else {
          if (elev < 1.0f && found)
            out_img(i,j) = volm_osm_category_io::volm_land_table_name["Open_Water"].id_;
        }
      }
    }

    // ingest OSM roads and junctions
    cnt = 0;
    unsigned num_roads = osm.num_roads();
    vcl_vector<vcl_vector<vgl_point_2d<double> > > roads_in_leaf;
    vcl_vector<volm_land_layer> roads_in_leaf_props;
    for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
      vcl_vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
      vcl_vector<vgl_point_2d<double> > line_geo;
      // check and obtain the road segment that lies inside the leaf
      if (!volm_io_tools::line_inside_the_box(leaf_bbox_geo, road, line_geo))
        continue;
      // go from geo coords to leaf local
      vcl_vector<vgl_point_2d<double> > line_img;
      unsigned char curr_level = osm.loc_lines()[r_idx]->prop().level_;
      unsigned char curr_id = osm.loc_lines()[r_idx]->prop().id_;
      double width = osm.loc_lines()[r_idx]->prop().width_;
      for (unsigned pt_idx = 0; pt_idx < line_geo.size(); pt_idx++) {
        double lx, ly, lz;
        lvcs->global_to_local(line_geo[pt_idx].x(), line_geo[pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
        if (i>=0 && j>=0 && i<out_img.ni() && j<out_img.nj())
          line_img.push_back(vgl_point_2d<double>(i,j));
      }
      if (line_img.size() < 2)  continue;
      // record current line for later junction calculation
      roads_in_leaf.push_back(line_img);
      roads_in_leaf_props.push_back(osm.loc_lines()[r_idx]->prop());
      // expend the line to polygon given certain width
      if (width < 1.0) width = 1.1;
      vgl_polygon<double> img_poly;
      if (!volm_io_tools::expend_line(line_img, width, img_poly)) {
        log << " expending osm line in tile " << t_id << ", leaf " << leaf->extent_ << " failed given width " << width << vcl_endl;
        error(log_file, log.str());
        return false;
      }
      cnt++;
      vgl_polygon_scan_iterator<double> it(img_poly, true);
      for (it.reset(); it.next();  ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x) {
          if ( x >= 0 && y >= 0 && x < out_img.ni() && y < out_img.nj()) {
            if (curr_level > level_img(x, y)) {
              level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
            }
          }
        }
      }
    }
    vcl_cout << cnt << " roads are ingested into land map" << vcl_endl;
    // find and ingest osm junctions
    cnt = 0;
    unsigned n_road_in = roads_in_leaf.size();
    for (unsigned r_idx = 0; r_idx < n_road_in; r_idx++)
    {
      vcl_vector<vgl_point_2d<double> > curr_rd = roads_in_leaf[r_idx];
      volm_land_layer curr_rd_prop = roads_in_leaf_props[r_idx];
      vcl_vector<vcl_vector<vgl_point_2d<double> > > net;
      vcl_vector<volm_land_layer> net_props;
      for (unsigned i = 0; i < n_road_in; i++)
        if (i != r_idx) {
          net.push_back(roads_in_leaf[i]);  net_props.push_back(roads_in_leaf_props[i]);
        }
      // find all possible junction for current road
      vcl_vector<vgl_point_2d<double> > cross_pts;
      vcl_vector<volm_land_layer> cross_props;
      vcl_vector<volm_land_layer> cross_geo_props;
      if (!volm_io_tools::search_junctions(curr_rd, curr_rd_prop, net, net_props, cross_pts, cross_props, cross_geo_props)) {
        log << "ERROR: find road junction for tile " << t_id << " leaf " << leaf_bbox_geo << " road " << r_idx << " failed\n";
        error(log_file, log.str());
        return false;
      }
      // ingest junction for current roads (note we will overwrite anything but buildings
      for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
        cnt++;
        unsigned char curr_level = cross_props[c_idx].level_;
        unsigned char curr_id = cross_props[c_idx].id_;
        double radius = cross_props[c_idx].width_;
        int cx = (int)vcl_floor(cross_pts[c_idx].x() + 0.5);
        int cy = (int)vcl_floor(cross_pts[c_idx].y() + 0.5);
        for (int ii = cx-radius; ii < cx+radius; ii++)
          for (int jj = cy-radius; jj <cy+radius; jj++)
            if (ii >=0 && jj>=0 && ii <out_img.ni() && jj < out_img.nj())
              if (curr_level >= level_img(ii,jj) && out_img(ii,jj) != volm_osm_category_io::volm_land_table_name["building"].id_) {
                out_img(ii,jj)   = curr_id;
                level_img(ii,jj) = curr_level;
              }
      }
    }
    vcl_cout << cnt << " road crosses are ingested into land map" << vcl_endl;

    // ingest osm points
    cnt = 0;
    unsigned n_pts = osm.num_locs();
    vcl_vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
    for (unsigned p_idx = 0; p_idx < n_pts; p_idx++) {
      vgl_point_2d<double> pt = loc_pts[p_idx]->loc();
      if (!leaf_bbox_geo.contains(pt))  continue;
      // transfer from geo coord to image pixel
      unsigned char curr_level = loc_pts[p_idx]->prop().level_;
      unsigned char curr_id = loc_pts[p_idx]->prop().id_;
      // remove buildings from osm
      if (curr_id == volm_osm_category_io::volm_land_table_name["building"].id_)
        continue;
      double lx, ly, lz;
      lvcs->global_to_local(pt.x(), pt.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
      int x = (int)i;  int y = (int)j;
      if (x >= 0 && y >= 0 && x < out_img.ni() && y < out_img.nj())
        if (curr_level >= level_img(x,y))
        {  out_img(x, y) = curr_id;  level_img(x,y) = curr_level;  cnt++;}
    }
    vcl_cout << cnt << " location points are ingested into land map" << vcl_endl;

    // ingest SME data -- forts only
    cnt = 0;
    double fort_rad = 200.0;
    for (unsigned kk = 0; kk < sme_objects.size(); kk++)
    {
      if ( (unsigned char)sme_objects[kk].second != volm_osm_category_io::volm_land_table_name["forts"].id_)
        continue;
      double lx, ly, lz;
      lvcs->global_to_local(sme_objects[kk].first.x(), sme_objects[kk].first.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
      int x = (int)i;  int y = (int)j;
      cnt++;
      for (int ii = x-fort_rad; ii < x+fort_rad; ii++)
        for (int jj = y-fort_rad; jj < y+fort_rad; jj++)
          if (ii >= 0 && jj >= 0 && ii < out_img.ni() && jj < out_img.nj() && level_img(ii,jj) == 0) {
            out_img(ii,jj) = volm_osm_category_io::volm_land_table_name["forts"].id_;
            cnt++;
          }
    }
    vcl_cout << cnt << " SME forts are ingested into land map" << vcl_endl;

#if 1
    // ingest URGENT data
    cnt = 0;
    for (unsigned ii = 0; ii < build_polys.size(); ii++)
    {
      unsigned char curr_id = volm_osm_category_io::volm_land_table_name["building"].id_;
      if (build_heights[ii] > 20)
        curr_id = volm_osm_category_io::volm_land_table_name["tall_building"].id_;
      // check if this is one of the sme objects
      for (unsigned kk = 0; kk < sme_objects.size(); kk++) {
        if (build_polys[ii].first.contains(sme_objects[kk].first.x(), sme_objects[kk].first.y())) {
          curr_id = sme_objects[kk].second;
          break;
        }
      }

      // find local poly
      vgl_polygon<double> img_poly(1);
      for (unsigned jj = 0; jj < build_polys[ii].first[0].size(); jj++) {
        double lx,ly,lz;
        lvcs->global_to_local(build_polys[ii].first[0][jj].x(), build_polys[ii].first[0][jj].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
        int x = (int)i;  int y = (int)j;
        if (x>=0 && y>=0 && x<out_img.ni() && y<out_img.nj())
          img_poly[0].push_back(vgl_point_2d<double>(i,j));
      }

      // mark the building pixel but do not overwrite restaurant and pier (note we enlarge each pixel by 5 pixel)
      vgl_polygon_scan_iterator<double> psi(img_poly, true);
      for (psi.reset(); psi.next();  ) {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) {
          for (int yy = y-5; yy < y+5; yy++) {
            for (int xx = x-5; xx < x+5; xx++) {
              if (xx>=0 && yy>=0 && xx<out_img.ni() && yy<out_img.nj()
                  && out_img(xx,yy) != volm_osm_category_io::volm_land_table_name["restaurant"].id_
                  && out_img(xx,yy) != volm_osm_category_io::volm_land_table_name["piers"].id_)
              {  out_img(xx,yy) = curr_id;  cnt++;  }
            }
          }
        }
      }
    }
    vcl_cout << cnt << " URGENT buildings are ingested" << vcl_endl;
#endif

    // enlarge pier pixels by 5 pixel
    vcl_vector<vcl_pair<unsigned, unsigned> > pier_pixels;
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        if (out_img(i,j) == volm_osm_category_io::volm_land_table_name["piers"].id_) {
          pier_pixels.push_back(vcl_pair<unsigned, unsigned>(i,j));
        }
    vcl_cout << pier_pixels.size() << " piers exist in leaf " << l_idx << vcl_endl;
    for (vcl_vector<vcl_pair<unsigned, unsigned> >::iterator vit = pier_pixels.begin();  vit != pier_pixels.end(); ++vit) {
      unsigned i = vit->first;  unsigned j = vit->second;
      for (int x=i-8; x<i+8; x++)
        for (int y=j-8; y<j+8; y++)
          if (x>=0 && y>=0 && x<ni && y<nj) {
            out_img(x,y) = volm_osm_category_io::volm_land_table_name["piers"].id_;
          }
    }
    vcl_cout << "enlarge pier pixels succeed" << vcl_endl;

#endif

    // save the image
    vil_save(out_img, img_name.str().c_str());
    vil_image_view<vil_rgb<vxl_byte> > out_rgb_img(ni, nj, 1);
    out_rgb_img.fill(volm_osm_category_io::volm_land_table_name["invalid"].color_);
    vcl_stringstream color_name;
    color_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << vcl_setprecision(12) << lat_min
                               << tiles[t_id].direction() << vcl_setprecision(12) << lon_min
                               << "_S" << scale_x << 'x' << scale_y << ".png";
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        out_rgb_img(i,j) = volm_osm_category_io::volm_land_table[out_img(i,j)].color_;
    vil_save(out_rgb_img, color_name.str().c_str());

    vcl_cout << l_idx << " finished successfully!" << vcl_endl;
  } // end of leaf loop

  return true;
}
#endif
