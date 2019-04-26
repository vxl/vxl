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
#include <vgl/vgl_area.h>
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

#include <utility>

static void error(std::string log_file, const std::string& msg)
{
  std::cerr << msg;  volm_io::write_post_processing_log(std::move(log_file), msg);
}

#if 1
  // For Phase 1A, generate 2D land map using NLCD, LIDAR(elev), SME(fort), URGENT(building), OSM data
  // Output GeoTiff image size is controlled by geo index and has 1 meter resolution
  // Pipeline: 1.  --> generate label using NLCD + LIDAR data (LIDAR elevation is used to refined beach/water boundary)
  //           2.  --> put OSM data onto the basic label map (road network needs to be put onto the map first)
  //           3.  --> put URGENT building, SME data onto OSM label image
int main(int argc, char** argv)
{
  vul_arg<std::string>  nlcd_folder("-nlcd", "folder where all NLCD image are", "");
  vul_arg<std::string> lidar_folder("-lidar", "folder where LIDAR image are stored", "");
  vul_arg<std::string>   osm_folder("-osm", "folder where OSM data are stored", "");
  vul_arg<std::string> build_folder("-build", "folder where URGENT data are stored", "");
  vul_arg<std::string>   sme_folder("-sme", "folder where SME fort data are stored", "");
  vul_arg<std::string>   out_folder("-out", "output folder", "");
  vul_arg<std::string>    world_str("-world", "world name of ROI", "coast");
  vul_arg<unsigned>        tile_id("-tile", "tile id of ROI", 100);
  vul_arg<float>          min_size("-min", "minimum size of the geo index (in wgs84)", 0.0625);
  vul_arg<int>            leaf_idx("-leaf", "leaf id inside tile (for parallel execution)", -1);
  vul_arg<std::string>      roi_kml("-roi", "kml file storing the ROI region polygon", "");
  vul_arg_parse(argc, argv);

  // input check
  if (nlcd_folder().compare("") == 0 || lidar_folder().compare("") == 0 || osm_folder().compare("") == 0 || roi_kml().compare("") == 0 ||
      build_folder().compare("") == 0 || sme_folder().compare("") == 0 || out_folder().compare("") == 0 ||
      tile_id() == 100 )
  {  vul_arg_display_usage_and_exit();  return -1;  }

  if (world_str().compare("coast") != 0) {
    std::cout << "ERROR: input ROI world: " << world_str() << " is not supported (only coast allowed)" << std::endl;
    return -1;
  }

  std::string log_file = out_folder() + "/log_osm_2d_map_wr_" + world_str() + ".xml";
  std::stringstream log;
  unsigned t_id = tile_id();

  std::string phase_str;
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
  std::vector<volm_tile> tiles;
  if (world_str().compare("coast") == 0)  tiles = volm_tile::generate_p1_wr2_tiles();
  unsigned tile_size = tiles.size();
  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_osm_object_ids_sptr>(tiles[t_id], min_size(), roi_poly);
  std::cout << "Create geo index with min_size: " << min_size() << std::endl;

  unsigned tree_depth = volm_geo_index2::depth(root);
  std::stringstream kml;  kml << out_folder() << "/p1a_" << world_str() << "_tile_" << t_id << "_depth_" << tree_depth << ".kml";
  volm_geo_index2::write_to_kml(root, tree_depth, kml.str());
  std::stringstream root_txt;  root_txt << out_folder() << "/p1a_" << world_str() << "_tile_" << t_id << ".txt";

  // load NLCD images and sort them based on tile order
  std::vector<volm_img_info> info_tmp;
  volm_io_tools::load_nlcd_imgs(nlcd_folder(), info_tmp);
  if (info_tmp.size() != tile_size) {
    log << "ERROR: mismatch in create tiles " << tiles.size() << " and loaded " << info_tmp.size() << " nlcd images, check input nlcd folder\n";  error(log_file, log.str());
    return -1;
  }
  std::vector<volm_img_info> nlcd_infos;
  for (unsigned i = 0; i < tile_size; i++) {
    for (unsigned j = 0; j < tile_size; j++) {
      double diff_lon = tiles[i].bbox_double().min_x() - info_tmp[j].bbox.min_x();
      double diff_lat = tiles[i].bbox_double().min_y() - info_tmp[j].bbox.min_y();
      double diff = std::sqrt(diff_lon*diff_lon + diff_lat*diff_lat);
      if (diff < 0.25) {
        nlcd_infos.push_back(info_tmp[j]);
        break;
      }
    }
  }


  // load Lidar images
  std::vector<volm_img_info> lidar_infos;
  volm_io_tools::load_lidar_imgs(lidar_folder(), lidar_infos);
  std::cout << "read " << lidar_infos.size() << " LIDAR images!" << std::endl;

  // load sme data
  std::string glob = sme_folder() + "/*.csv";
  std::vector<std::pair<vgl_point_2d<double>, int> > sme_objects;
  for (vul_file_iterator fit = glob; fit; ++fit)
    volm_io::read_sme_file(fit(), sme_objects);

  // load urgent building data
  glob = build_folder() + "/*.csv";
  std::vector<std::pair<vgl_polygon<double>, vgl_point_2d<double> > > build_polys;
  std::vector<double> build_heights;
  for (vul_file_iterator fit = glob; fit; ++fit) {
    volm_io::read_building_file(fit(), build_polys, build_heights);
  }

  // load open street map binary
  std::stringstream osm_file;  osm_file << osm_folder() << "/p1" << phase_str << "_wr" << world_id << "_tile_" << t_id << "_osm.bin";
  if (!vul_file::exists(osm_file.str())) {
    log << "ERROR: can not find osm binary file: " << osm_file.str() << '\n';  error(log_file, log.str());
    return -1;
  }
  volm_osm_objects osm(osm_file.str());

  std::cout << " --------------- START -----------------" << std::endl;
  std::cout << "generate 2d land category map for tile " << t_id << std::endl;
  std::cout << "read " << nlcd_infos.size() << " NLCD images!" << std::endl;
  std::cout << "read " << lidar_infos.size() << " LIDAR images!" << std::endl;
  std::cout << "read " << sme_objects.size() << " SME objects!" << std::endl;
  std::cout << "read " << build_polys.size() << " URGENT buildings!" << std::endl;
  std::cout << "read " << osm.num_locs() << " OSM loc points, " << osm.num_roads() << " OSM roads and " << osm.num_regions() << " OSM regions" << std::endl;


  // start to create 2d map
  volm_img_info nlcd_info = nlcd_infos[t_id];
  auto* nlcd_img = dynamic_cast<vil_image_view<vxl_byte>*>(nlcd_info.img_r.ptr());
  if (!nlcd_img) {
    log << "ERROR: unsupported NLCD image pixel format: " << nlcd_info.img_r->pixel_format() << '\n';  error(log_file, log.str());
    return -1;
  }
  std::cout << "NLCD image covers the given tile: " << nlcd_info.img_name << std::endl;
  // retrieve leaves for current tile
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves);
  std::cout << leaves.size() << " leaves require map creation" << std::endl;
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
    auto ni = (unsigned)std::ceil(box_lx);
    auto nj = (unsigned)std::ceil(box_ly);
    vgl_box_2d<double> leaf_bbox_geo = leaf->extent_;
    vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);
    // create geo camera for output image
    vnl_matrix<double> trans_matrix(4,4,0.0);
    trans_matrix[0][0] = scale_x/ni;    trans_matrix[1][1] = -scale_y/nj;
    trans_matrix[0][3] = lon_min;       trans_matrix[1][3] = lat_max;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    cam->set_scale_format(true);
    std::stringstream cam_name;
    cam_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << std::setprecision(12) << lat_min
                                              << tiles[t_id].direction()  << std::setprecision(12) << lon_min
                                              << "_S" << scale_x << 'x' << scale_y << ".tfw";
    cam->save_as_tfw(cam_name.str());
    // create 2d image for current leaf
    vil_image_view<vxl_byte>   out_img(ni, nj, 1);
    vil_image_view<vxl_byte> level_img(ni, nj, 1);

    std::stringstream img_name;
    img_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << std::setprecision(12) << lat_min
                             << tiles[t_id].direction() << std::setprecision(12) << lon_min
                             << "_S" << scale_x << 'x' << scale_y << ".tif";
    out_img.fill(0);  level_img.fill(0);
    std::cout << "leaf " << l_idx << " has geo boundary " << leaf_bbox_geo << " --> image size " << out_img.ni() << 'x' << out_img.nj() << std::endl;

    // obtain LIDAR images that intersect with current leaf
    std::vector<std::pair<vil_image_view<float>, vpgl_geo_camera*> > lidar_imgs;
    for (auto & lidar_info : lidar_infos) {
      if (vgl_area(vgl_intersection(lidar_info.bbox, leaf_bbox_geo)) <= 0.0)
        continue;
      if (lidar_info.img_r->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
        log << "ERROR: unsupported LIDAR image pixel format: " << lidar_info.img_r->pixel_format() << '\n';  error(log_file, log.str());
        return -1;
      }
      vil_image_view<float> img(lidar_info.img_r);
      lidar_imgs.emplace_back(img, lidar_info.cam);
    }
    std::cout << lidar_imgs.size() << " LIDAR image intersect with the leaf " << l_idx << std::endl;
    // ingest NLCD image and refine the beach/water boundary by LIDAR elevation
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // obtain global lon/lat
        double lon, lat, gz;
        auto local_x = (float)(i+0.5);  auto local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        // find NLCD pixel
        unsigned char label = volm_osm_category_io::volm_land_table_name["invalid"].id_;
        double u, v;
        nlcd_info.cam->global_to_img(lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u+0.5);  auto vv = (unsigned)std::floor(v+0.5);
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
          if (x >= 0 && y >= 0 && x < static_cast<int>(out_img.ni()) && y < static_cast<int>(out_img.nj())) {
            if (curr_level > level_img(x, y))
            {  out_img(x,y) = curr_id;  level_img(x, y) = curr_level;  }
          }
        }
      }
    }
    std::cout << cnt << " OSM regions are ingested into Land map" << std::endl;

    // refine the water-sand boundary using lidar elev value
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        // obtain global lon/lat
        double lon, lat, gz;
        auto local_x = (float)(i+0.5);  auto local_y = (float)(box_ly-j+0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        float elev = 100.0f;
        bool found = false;
        for (auto & lidar_img : lidar_imgs) {
          double u, v;
          lidar_img.second->global_to_img(lon, lat, gz, u, v);
          auto uu = (unsigned)std::floor(u+0.5);  auto vv = (unsigned)std::floor(v+0.5);
          if (uu > 0 && vv > 0 && uu < lidar_img.first.ni() && vv < lidar_img.first.nj()) {
            elev = lidar_img.first(uu, vv);  found = true;
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
    std::vector<std::vector<vgl_point_2d<double> > > roads_in_leaf;
    std::vector<volm_land_layer> roads_in_leaf_props;
    for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
      std::vector<vgl_point_2d<double> > road = osm.loc_lines()[r_idx]->line();
      std::vector<vgl_point_2d<double> > line_geo;
      // check and obtain the road segment that lies inside the leaf
      if (!volm_io_tools::line_inside_the_box(leaf_bbox_geo, road, line_geo))
        continue;
      // go from geo coords to leaf local
      std::vector<vgl_point_2d<double> > line_img;
      unsigned char curr_level = osm.loc_lines()[r_idx]->prop().level_;
      unsigned char curr_id = osm.loc_lines()[r_idx]->prop().id_;
      double width = osm.loc_lines()[r_idx]->prop().width_;
      for (auto & pt_idx : line_geo) {
        double lx, ly, lz;
        lvcs->global_to_local(pt_idx.x(), pt_idx.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
        if (i>=0 && j>=0 && i<out_img.ni() && j<out_img.nj())
          line_img.emplace_back(i,j);
      }
      if (line_img.size() < 2)  continue;
      // record current line for later junction calculation
      roads_in_leaf.push_back(line_img);
      roads_in_leaf_props.push_back(osm.loc_lines()[r_idx]->prop());
      // expend the line to polygon given certain width
      if (width < 1.0) width = 1.1;
      vgl_polygon<double> img_poly;
      if (!volm_io_tools::expend_line(line_img, width, img_poly)) {
        log << " expending osm line in tile " << t_id << ", leaf " << leaf->extent_ << " failed given width " << width << std::endl;
        error(log_file, log.str());
        return -1;
      }
      cnt++;
      vgl_polygon_scan_iterator<double> it(img_poly, true);
      for (it.reset(); it.next();  ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x) {
          if ( x >= 0 && y >= 0 && x < static_cast<int>(out_img.ni()) && y < static_cast<int>(out_img.nj())) {
            if (curr_level > level_img(x, y)) {
              level_img(x,y) = curr_level;   out_img(x,y) = curr_id;
            }
          }
        }
      }
    }
    std::cout << cnt << " roads are ingested into land map" << std::endl;
    // find and ingest osm junctions
    cnt = 0;
    unsigned n_road_in = roads_in_leaf.size();
    for (unsigned r_idx = 0; r_idx < n_road_in; r_idx++)
    {
      std::vector<vgl_point_2d<double> > curr_rd = roads_in_leaf[r_idx];
      volm_land_layer curr_rd_prop = roads_in_leaf_props[r_idx];
      std::vector<std::vector<vgl_point_2d<double> > > net;
      std::vector<volm_land_layer> net_props;
      for (unsigned i = 0; i < n_road_in; i++)
        if (i != r_idx) {
          net.push_back(roads_in_leaf[i]);  net_props.push_back(roads_in_leaf_props[i]);
        }
      // find all possible junction for current road
      std::vector<vgl_point_2d<double> > cross_pts;
      std::vector<volm_land_layer> cross_props;
      std::vector<volm_land_layer> cross_geo_props;
      if (!volm_io_tools::search_junctions(curr_rd, curr_rd_prop, net, net_props, cross_pts, cross_props, cross_geo_props)) {
        log << "ERROR: find road junction for tile " << t_id << " leaf " << leaf_bbox_geo << " road " << r_idx << " failed\n";
        error(log_file, log.str());
        return -1;
      }
      // ingest junction for current roads (note we will overwrite anything but buildings
      for (unsigned c_idx = 0; c_idx < cross_pts.size(); c_idx++) {
        cnt++;
        unsigned char curr_level = cross_props[c_idx].level_;
        unsigned char curr_id = cross_props[c_idx].id_;
        double radius = cross_props[c_idx].width_;
        int cx = (int)std::floor(cross_pts[c_idx].x() + 0.5);
        int cy = (int)std::floor(cross_pts[c_idx].y() + 0.5);
        for (int ii = cx-radius; ii < cx+radius; ii++)
          for (int jj = cy-radius; jj <cy+radius; jj++)
            if (ii >=0 && jj>=0 && ii < static_cast<int>(out_img.ni()) && jj < static_cast<int>(out_img.nj()))
              if (curr_level >= level_img(ii,jj) && out_img(ii,jj) != volm_osm_category_io::volm_land_table_name["building"].id_) {
                out_img(ii,jj)   = curr_id;
                level_img(ii,jj) = curr_level;
              }
      }
    }
    std::cout << cnt << " road crosses are ingested into land map" << std::endl;

    // ingest osm points
    cnt = 0;
    unsigned n_pts = osm.num_locs();
    std::vector<volm_osm_object_point_sptr> loc_pts = osm.loc_pts();
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
      if (x >= 0 && y >= 0 && x < static_cast<int>(out_img.ni()) && y < static_cast<int>(out_img.nj()))
        if (curr_level >= level_img(x,y))
        {  out_img(x, y) = curr_id;  level_img(x,y) = curr_level;  cnt++;}
    }
    std::cout << cnt << " location points are ingested into land map" << std::endl;

    // ingest SME data -- forts only
    cnt = 0;
    double fort_rad = 200.0;
    for (auto & sme_object : sme_objects)
    {
      if ( (unsigned char)sme_object.second != volm_osm_category_io::volm_land_table_name["forts"].id_)
        continue;
      double lx, ly, lz;
      lvcs->global_to_local(sme_object.first.x(), sme_object.first.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = lx - leaf_bbox.min_x();  double j = leaf_bbox.max_y() - ly;
      int x = (int)i;  int y = (int)j;
      cnt++;
      for (int ii = x-fort_rad; ii < x+fort_rad; ii++)
        for (int jj = y-fort_rad; jj < y+fort_rad; jj++)
          if (ii >= 0 && jj >= 0 && ii < static_cast<int>(out_img.ni()) && jj < static_cast<int>(out_img.nj()) && level_img(ii,jj) == 0) {
            out_img(ii,jj) = volm_osm_category_io::volm_land_table_name["forts"].id_;
            cnt++;
          }
    }
    std::cout << cnt << " SME forts are ingested into land map" << std::endl;

#if 1
    // ingest URGENT data
    cnt = 0;
    for (unsigned ii = 0; ii < build_polys.size(); ii++)
    {
      unsigned char curr_id = volm_osm_category_io::volm_land_table_name["building"].id_;
      if (build_heights[ii] > 20)
        curr_id = volm_osm_category_io::volm_land_table_name["tall_building"].id_;
      // check if this is one of the sme objects
      for (auto & sme_object : sme_objects) {
        if (build_polys[ii].first.contains(sme_object.first.x(), sme_object.first.y())) {
          curr_id = sme_object.second;
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
        if (x>=0 && y>=0 && x< static_cast<int>(out_img.ni()) && y< static_cast<int>(out_img.nj()))
          img_poly[0].push_back(vgl_point_2d<double>(i,j));
      }

      // mark the building pixel but do not overwrite restaurant and pier (note we enlarge each pixel by 5 pixel)
      vgl_polygon_scan_iterator<double> psi(img_poly, true);
      for (psi.reset(); psi.next();  ) {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) {
          for (int yy = y-5; yy < y+5; yy++) {
            for (int xx = x-5; xx < x+5; xx++) {
              if (xx>=0 && yy>=0 && xx< static_cast<int>(out_img.ni()) && yy< static_cast<int>(out_img.nj())
                  && out_img(xx,yy) != volm_osm_category_io::volm_land_table_name["restaurant"].id_
                  && out_img(xx,yy) != volm_osm_category_io::volm_land_table_name["piers"].id_)
              {  out_img(xx,yy) = curr_id;  cnt++;  }
            }
          }
        }
      }
    }
    std::cout << cnt << " URGENT buildings are ingested" << std::endl;
#endif

    // enlarge pier pixels by 5 pixel
    std::vector<std::pair<unsigned, unsigned> > pier_pixels;
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        if (out_img(i,j) == volm_osm_category_io::volm_land_table_name["piers"].id_) {
          pier_pixels.emplace_back(i,j);
        }
    std::cout << pier_pixels.size() << " piers exist in leaf " << l_idx << std::endl;
    for (auto & pier_pixel : pier_pixels) {
      unsigned i = pier_pixel.first;  unsigned j = pier_pixel.second;
      for (size_t x=i-8; x<i+8; x++)
        for (size_t y=j-8; y<j+8; y++)
          if (x>=0 && y>=0 && x<ni && y<nj) {
            out_img(x,y) = volm_osm_category_io::volm_land_table_name["piers"].id_;
          }
    }
    std::cout << "enlarge pier pixels succeed" << std::endl;

#endif

    // save the image
    vil_save(out_img, img_name.str().c_str());
    vil_image_view<vil_rgb<vxl_byte> > out_rgb_img(ni, nj, 1);
    out_rgb_img.fill(volm_osm_category_io::volm_land_table_name["invalid"].color_);
    std::stringstream color_name;
    color_name << out_folder() << "/Land2dMap_" << tiles[t_id].hemisphere() << std::setprecision(12) << lat_min
                               << tiles[t_id].direction() << std::setprecision(12) << lon_min
                               << "_S" << scale_x << 'x' << scale_y << ".png";
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        out_rgb_img(i,j) = volm_osm_category_io::volm_land_table[out_img(i,j)].color_;
    vil_save(out_rgb_img, color_name.str().c_str());

    std::cout << l_idx << " finished successfully!" << std::endl;
  } // end of leaf loop

  return -1;
}
#endif
