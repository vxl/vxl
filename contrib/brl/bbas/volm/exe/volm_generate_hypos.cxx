// :
// \file
// \brief executable to generate hypothesis locations using DEMs or LIDAR
// \author Ozge C. Ozcanli
// \date Nov 15, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_timer.h>
#include <volm/volm_io_tools.h>
#include <volm/volm_category_io.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>

inline float next_mult_2(float val)
{
  return (float)(2.0f * vcl_ceil(val * 0.5f) );
}

// find the elevation of the point and add it to the geo index if found
bool add_hypo(volm_geo_index_node_sptr hyp_root, vcl_vector<volm_img_info> const& infos,
              vcl_vector<volm_img_info> const& class_map_infos,
              vgl_point_2d<double> const& pt, double inc_in_sec_radius, bool search)
{
  // find the elevation of the hypotheses
  float z = 0;
  bool  found_it = false;

  for( unsigned mm = 0; mm < infos.size(); mm++ )
    {
    if( infos[mm].contains(pt) )
      {
      double u, v;
      infos[mm].cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)vcl_floor(u + 0.5);
      int vv = (int)vcl_floor(v + 0.5);
      if( infos[mm].valid_pixel(uu, vv) )
        {
        // check whether this location is building
        bool not_building = true;
        for( unsigned mc = 0; mc < class_map_infos.size(); mc++ )
          {
          if( class_map_infos[mc].contains(pt) )
            {
            double uc, vc;
            class_map_infos[mc].cam->global_to_img(pt.x(), pt.y(), 0, uc, vc);
            int uuc = (int)vcl_floor(uc + 0.5);
            int vvc = (int)vcl_floor(vc + 0.5);
            if( class_map_infos[mc].valid_pixel(uuc, vvc) )
              {
              vil_image_view<vxl_byte> imgc(class_map_infos[mc].img_r);
              // vcl_cout << " loc " << vcl_setprecision(12) << pt << " is " << volm_osm_category_io::volm_land_table[imgc(uuc,vvc)].name_ << " at pixel "
              //         << " (" << uuc << "x" << vvc << ") in image " << class_map_infos[mc].name << vcl_endl;
              if( imgc(uuc,
                       vvc) == volm_osm_category_io::volm_land_table_name["building"].id_ ||
                  imgc(uuc, vvc) == volm_osm_category_io::volm_land_table_name["tall_building"].id_ )
                {
                not_building = false;
                break;
                }
              }
            }
          }
        if( not_building )
          {
          // vil_image_view<vxl_int_16> img(infos[mm].img_r);
          vil_image_view<float> img(infos[mm].img_r);
          z = img(uu, vv);
          found_it = true;
          break;
          }
        }
      }
    }
  if( found_it )  // add the hypo
    {
    if( search )
      {
      if( !volm_geo_index::exists(hyp_root, pt.y(), pt.x(), inc_in_sec_radius) )
        {
        if( z > 0 )
          {
          volm_geo_index::add_hypothesis(hyp_root, pt.x(), pt.y(), z);
          }
        return true;
        }
      }
    else
      {
      if( z > 0 )
        {
        volm_geo_index::add_hypothesis(hyp_root, pt.x(), pt.y(), z);
        }
      return true;
      }
    }
  return false;
}

int find_land_type(vcl_vector<volm_img_info>& geo_infos, vgl_point_2d<double>& pt)
{
  int type = -1;

  for( unsigned mm = 0; mm < geo_infos.size(); mm++ )
    {
    if( geo_infos[mm].contains(pt) )
      {
      double u, v;
      geo_infos[mm].cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)vcl_floor(u + 0.5);
      int vv = (int)vcl_floor(v + 0.5);
      if( geo_infos[mm].valid_pixel(uu, vv) )
        {
        vil_image_view<vxl_byte> img(geo_infos[mm].img_r);
        type = img(uu, vv);
        break;
        }
      }
    }
  return type;
}

bool loc_inside_region(vgl_polygon<double> const& poly, double const& ptx, double const& pty)
{
  // when sheets overlap, the poly.contain method will return false for pts located inside the overlapped region
  // this function will returns true if the given point is inside any single sheet of the polygon
  unsigned num_sheet = poly.num_sheets();

  for( unsigned i = 0; i < num_sheet; i++ )
    {
    vgl_polygon<double> single_sheet_poly(poly[i]);
    if( single_sheet_poly.contains(ptx, pty) )
      {
      return true;
      }
    }
  return false;
}

// read the tiles of the region, create a geo index and write the hyps
int main(int argc,  char* * argv)
{
  vul_timer t;

  t.mark();

  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<float>      inc("-inc", "increments in arcseconds, e.g. 0.1 for ~3m increments", 0.1f);
  vul_arg<unsigned>   nh("-nh",
                         "number of hyps in each direction in one leaf tile, e.g. 100 so that each tile has 100x100 hyps",
                         100);
  vul_arg<vcl_string> out_pre("-out_pre", "output file folder with file separator at the end", "");
  vul_arg<vcl_string> add_gt("-addgt", "add known gt locations? pass the name of txt file containing gt locations", ""); // if no -addgt argument then the value is false, if there is one then the value is true
  vul_arg<bool>       only_gt("-onlygt", "add only known gt locations", false);
  vul_arg<unsigned>   tile_id("-tile", "id of the tile", 0);
  vul_arg<unsigned>   utm_zone("-zone", "utm zone to fill", 17);
  vul_arg<bool>       read("-read", "if passed only read the index in the out_pre() folder and report some statistics",
                           false);
  vul_arg<bool> use_osm_roads("-osm_roads",
                              "if passed, use the osm binary file to read the roads and generate location hypotheses along them",
                              false);
  vul_arg<bool>       non_building("-non-build", "if passed, will generate location on all non-building region", false);
  vul_arg<vcl_string> osm_bin_file("-osm_bin", "the binary file that has all the objects", "");
  vul_arg<vcl_string> osm_tree_folder("-osm_tree",
                                      "the geoindex tree folder that has the tree structure with the ids of osm objects at its leaves",
                                      "");
  vul_arg<vcl_string> region_name("-region_name",
                                  "if passed, will generate constant post on the given region, e.g, sane region, pier region",
                                  "");
  vul_arg<int>        world_id("-world", "the id of the world", -1);
  vul_arg<bool>       p1b("-p1b", "use the p1b tiles", false);
  vul_arg<bool>       p1a("-p1a", "use the p1a tiles", false);
  vul_arg<bool>       use_satellite_img("-sat", "use satellite image resource", false);
  vul_arg<vcl_string> land_class_map_folder("-map", "folder for land classification map with building in the image",
                                            "");
  vul_arg<vcl_string> land("-land", "the input folder of the land type .tif files (geo cover images)", "");
  vul_arg<vcl_string> urgent_folder("-urgent", "RUGENT data folder", "");

  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  double arcsec_to_sec = 1.0f / 3600.0f;
  double inc_in_sec = inc() * arcsec_to_sec;
  double inc_in_sec_radius_ratio = 3.0 / 4.0;
  double inc_in_sec_rad = inc_in_sec * inc_in_sec_radius_ratio; // radius to search for existence of before adding a new one

  // generate locations along the OSM road for p1b world regions
  if( p1b() )
    {

    if( out_pre().compare("") == 0 || in_poly().compare("") == 0 )
      {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
      }
    vcl_cout << "will use increments " << inc() << " arcseconds along north and east directions!\n";

    vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly() );
    vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;
    poly.print(vcl_cout);
    vcl_vector<volm_tile> tiles;
    if( !volm_tile::generate_tiles(world_id(), tiles) )
      {
      vcl_cerr << "Unknown world id: " << world_id() << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
      }

    vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
    unsigned i = tile_id();
    if( i >= tiles.size() )
      {
      vcl_cerr << "Unknown tile id: " << i << vcl_endl;
      return volm_io::EXE_ARGUMENT_ERROR;
      }

    double meter_to_sec = volm_io_tools::meter_to_seconds(tiles[i].lat_, tiles[i].lon_);

    float                    size = 0.05f; // in seconds, set a fixed size for the leaves
    volm_geo_index_node_sptr hyp_root = volm_geo_index::construct_tree(tiles[i], (float)size, poly);
    // write the geo index and the hyps
    if( region_name().compare("") == 0 && !non_building() )
      {
      vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << ".txt";
      volm_geo_index::write(hyp_root, file_name.str(), (float)size);
      unsigned         depth = volm_geo_index::depth(hyp_root);
      vcl_stringstream file_name2; file_name2 << out_pre() << "geo_index_tile_" << i << ".kml";
      volm_geo_index::write_to_kml(hyp_root, 0, file_name2.str() );
      vcl_stringstream file_name3; file_name3 << out_pre() << "geo_index_tile_" << i << "_depth_" << depth << ".kml";
      volm_geo_index::write_to_kml(hyp_root, depth, file_name3.str() );
      }

    if( only_gt() && add_gt().compare("") != 0 )    // only add the ground truth points in the file as they are given (i.e. with the elevs in the file)

      {// load height map resource to get the height for each ground truth location
      vcl_vector<volm_img_info> infos;
      if( use_satellite_img() )
        {
        // load the satellite resources
        volm_io_tools::load_satellite_height_imgs(in_folder(), infos, false, "");
        }
      else
        {
        // load the DEM tiles
        volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
        }
      if( out_pre().compare("") == 0 || world_id() < 0 )
        {
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
        }

      // load satellite classification map to avoid locations on top of the buildings (can be empty if no class map)
      vcl_vector<volm_img_info> class_map_infos;
      volm_io_tools::load_imgs(land_class_map_folder(), class_map_infos, true, true, true);

      vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
      int                                                                                           cnt =
        volm_io::read_gt_file(add_gt(), samples);
      vcl_cout << " adding " << cnt << " gt locs!\n";
      vcl_cout << " height map resources: " << infos.size() << " images are loaded!\n";
      vcl_cout << " land classification resources: " << class_map_infos.size() << " images are loaded!\n";
      for( int j = 0; j < cnt; ++j )
        {
        vcl_cout << samples[j].second.first.first << " adding.. " << samples[j].first.y() << ", "
                 << samples[j].first.x() << ' ';
        // obtain the height of the height map resources
        // check whether the gt_loc is inside roi poly
        if( poly.contains(samples[j].first.x(), samples[j].first.y() ) )
          {
          bool added =
            add_hypo(hyp_root, infos, class_map_infos, vgl_point_2d<double>(samples[j].first.x(),
                                                                            samples[j].first.y() ), inc_in_sec_rad,
                     true);
          // bool added = volm_geo_index::add_hypothesis(hyp_root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
          if( added ) {vcl_cout << " success!\n"; }
          else {vcl_cout << " not found in tree of tile: " << tile_id() << "!\n"; }
          }
        else
          {
          vcl_cout << " not inside the ROI polygons!\n";
          }
        }

      unsigned r_cnt = volm_geo_index::hypo_size(hyp_root);
      vcl_cout << " after addition of gt locs, hyp_root " << i << " has total " << r_cnt
               << " hypotheses in its leaves!\n";

      // write the hypos
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
      vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str() );

      // write the hypos in kml file
      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
      for( unsigned jj = 0; jj < leaves.size(); jj++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[jj]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[jj]->hyps_->write_to_kml(out_file, inc_in_sec_rad, true);
        }

      return volm_io::SUCCESS;
      }

    if( use_osm_roads() )    // if an osm binary file is specified
      {
      if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || osm_bin_file().compare("") == 0 ||
          osm_tree_folder().compare("") == 0 || world_id() < 0 )
        {
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
        }

      volm_osm_objects osm_objs;
      double           min_size;
      vcl_stringstream file_name_pre;
      file_name_pre << osm_tree_folder() << "geo_index2_wr" << world_id() << "_tile_" << tile_id();
      volm_geo_index2_node_sptr root = volm_io_tools::read_osm_data_and_tree(file_name_pre.str(),
                                                                             osm_bin_file(), osm_objs, min_size);
      if( !root )
        {
        vcl_cerr << "Errors in reading " << file_name_pre.str() << " and/or " << osm_bin_file() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }
      vcl_stringstream kml_roads;
      kml_roads << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
      osm_objs.write_lines_to_kml(kml_roads.str() );

      vcl_vector<volm_img_info> infos;
      if( use_satellite_img() )
        {
        // load the satellite resources
        volm_io_tools::load_satellite_height_imgs(in_folder(), infos, false, "height_refined_sat_geo");
        }
      else
        {
        // load the DEM tiles
        volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
        }

      // load satellite classification map to avoid locations on top of the buildings (can be empty if now class map)
      vcl_vector<volm_img_info> class_map_infos;
      volm_io_tools::load_imgs(land_class_map_folder(), class_map_infos, true, true, true);
      vcl_cout << " height map resources: " << infos.size() << " images are loaded!\n";
      vcl_cout << " land classification resources: " << class_map_infos.size() << " images are loaded!\n";

      if( land().compare("") == 0 )    // if not using land types

        {// now go over each road in the osm file and insert into the tree of loc hyps
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        for( unsigned k = 0; k < roads.size(); k++ )
          {
          volm_osm_object_line_sptr         r = roads[k];
          vcl_string                        name = r->prop().name_;
          vcl_vector<vgl_point_2d<double> > points = r->line();
          for( unsigned kk = 1; kk < points.size(); kk++ )
            {
            if( loc_inside_region(poly, points[kk - 1].x(), points[kk - 1].y() ) )
              {
              add_hypo(hyp_root, infos, class_map_infos, points[kk - 1], inc_in_sec_rad, true);
              }
            // now interpolate along a straight line, assume locally planar
            double dif_dy = points[kk].y() - points[kk - 1].y();
            double dif_dx = points[kk].x() - points[kk - 1].x();
            double ds = vcl_sqrt(dif_dy * dif_dy + dif_dx * dif_dx);
            double inc_dy = inc_in_sec * (dif_dy / ds);
            double inc_dx = inc_in_sec * (dif_dx / ds);
            int    cnt = 0;
            while( ds > inc_in_sec )
              {
              ds = ds - inc_in_sec;
              cnt++;
              double x = points[kk - 1].x() + inc_dx * cnt;
              double y = points[kk - 1].y() + inc_dy * cnt;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, infos, class_map_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, true);
                }
              }

            }
          }
        }
      else
        {
        vcl_cout << "Generating hyps along the OSM roads and with increments depending on the Geocover land type!\n";
        vcl_vector<volm_img_info> geo_infos;
        volm_io_tools::load_geocover_imgs(land(), geo_infos);

        // now go over each road in the osm file and insert into the tree of loc hyps
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        vcl_cout << "will process " << roads.size() << " roads...\n"; vcl_cout.flush();
#if 0
        double y = 12.587963; double x = 76.822019;
        vcl_cout << "testing a location's type: x: " << x << " y: " << y << " (first roads first x: "
                 << roads[0]->line()[0].x() << ")\n";
        int type_prev = find_land_type(geo_infos, vgl_point_2d<double>(x, y) );
        vcl_cout << "\t\t type is: " << type_prev << "\n";

        y = 12.575318; x = 76.835210;
        vcl_cout << "testing a location's type: x: " << x << " y: " << y << " (first roads first x: "
                 << roads[0]->line()[0].x() << ")\n";
        type_prev = find_land_type(geo_infos, vgl_point_2d<double>(x, y) );
        vcl_cout << "\t\t type is: " << type_prev << "\n";
#endif
        for( unsigned k = 0; k < roads.size(); k++ )
          {
          if( k % 1000 == 0 ) {vcl_cout << k << "."; } vcl_cout.flush();
          volm_osm_object_line_sptr         r = roads[k];
          vcl_string                        name = r->prop().name_;
          vcl_vector<vgl_point_2d<double> > points = r->line();
          if( !points.size() )
            {
            continue;
            }
          int type_prev = find_land_type(geo_infos, points[0]);
          if( type_prev < 0 )
            {
            inc_in_sec = 4 * meter_to_sec;  // if unknown type, use the smallest possible interval to be safe
            }
          else
            {
            inc_in_sec = volm_osm_category_io::geo_land_hyp_increments[type_prev] * meter_to_sec;
            }
          inc_in_sec_rad = inc_in_sec * inc_in_sec_radius_ratio;
          double remainder = 0.0;
          if( loc_inside_region(poly, points[0].x(), points[0].y() ) )
            {
            add_hypo(hyp_root, infos, class_map_infos, points[0], inc_in_sec_rad, true);
            }
          for( unsigned kk = 1; kk < points.size(); kk++ )
            {
            double prev_x = points[kk - 1].x();
            double prev_y = points[kk - 1].y();
            int    type = find_land_type(geo_infos, points[kk]);
            if( type > 0 && type != type_prev )
              {
              double inc_now = volm_osm_category_io::geo_land_hyp_increments[type] * meter_to_sec;
              inc_in_sec = inc_in_sec < inc_now ? inc_in_sec : inc_now;  // pick the smaller of the increments if different types
              inc_in_sec_rad = inc_in_sec * inc_in_sec_radius_ratio;
              }

            // interpolate along a straight line, assume locally planar
            double dif_dy = points[kk].y() - points[kk - 1].y();
            double dif_dx = points[kk].x() - points[kk - 1].x();
            double ds = vcl_sqrt(dif_dy * dif_dy + dif_dx * dif_dx);

            if( inc_in_sec > ds )
              {
              remainder += ds;
              type_prev = type;
              continue;
              }

            double cos = dif_dx / ds;
            double sin = dif_dy / ds;

            double inc_dy = inc_in_sec * sin;
            double inc_dx = inc_in_sec * cos;

            // get rid of remainder first
            if( remainder < inc_in_sec )
              {
              double rem = inc_in_sec - remainder;
              double inc_dy_rem = rem * sin;
              double inc_dx_rem = rem * cos;
              double x = prev_x + inc_dx_rem;
              double y = prev_y + inc_dy_rem;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, infos, class_map_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, true);
                }
              prev_x = x;
              prev_y = y;
              ds -= rem;
              }

            while( ds > inc_in_sec )
              {
              ds -= inc_in_sec;
              double x = prev_x + inc_dx;
              double y = prev_y + inc_dy;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, infos, class_map_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, false);
                }
              prev_x = x;
              prev_y = y;
              }

            type_prev = type;
            remainder = ds;
            }
          }

        }

      unsigned r_cnt = volm_geo_index::hypo_size(hyp_root);
      vcl_cout << "\n root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";

      // write the hypos
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
      vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str() );

      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
      for( unsigned jj = 0; jj < leaves.size(); jj++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[jj]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[jj]->hyps_->write_to_kml(out_file, inc_in_sec_rad);
        }

      return volm_io::SUCCESS;
      } // end of p1b generating locations along the road

    // generate locations on all non-building region with constant post
    if( non_building() )
      {
      // check input
      if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || world_id() < 0 )
        {
        vul_arg_display_usage_and_exit();
        vcl_cout << "ERROR: missing input for generating phase 1b location database on all non-building region"
                 << vcl_endl;
        }
      // load satellite/dem height images
      vcl_vector<volm_img_info> infos;
      if( use_satellite_img() )
        {
        volm_io_tools::load_satellite_height_imgs(in_folder(), infos, false, "");
        }
      else
        {
        volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
        }

      // load satellite classification 2d map to avoid locations on top of the buildings
      vcl_vector<volm_img_info> class_map_infos;
      volm_io_tools::load_imgs(land_class_map_folder(), class_map_infos, true, true, true);
      vcl_cout << "height map resources: " << infos.size() << " geotiff images are loaded!\n";
      vcl_cout << "land classification resources: " << class_map_infos.size() << " images are loaded\n";

      // load URGENT building footprint
      vcl_string                                                        glob = urgent_folder() + "/*.csv";
      vcl_vector<vcl_pair<vgl_polygon<double>, vgl_point_2d<double> > > build_polys;
      vcl_vector<double>                                                build_heights;
      for( vul_file_iterator fit = glob; fit; ++fit )
        {
        volm_io::read_building_file(fit(), build_polys, build_heights);
        }
      vcl_cout << "URGENT building resources: " << build_polys.size() << " buildings are loaded from "
               << urgent_folder() << vcl_endl;
      // obtain the building polygons that intersect with current tile
      vcl_vector<vgl_polygon<double> > build_in_tile;
      for( unsigned i = 0; i < build_polys.size(); i++ )
        {
        if( vgl_intersection(tiles[tile_id()].bbox_double(), build_polys[i].first) )
          {
          build_in_tile.push_back(build_polys[i].first);
          }
        }
      vcl_cout << build_in_tile.size() << " URGENT buildings are inside given tile" << tiles[tile_id()].bbox()
               << vcl_endl;

      double   inc_in_meter = inc_in_sec * 21 / 0.000202;
      double   size = nh() * inc_in_sec;
      unsigned t_id = tile_id();
      vcl_cout << "generation locations with interval " << inc_in_meter << " meter. (" << inc_in_sec << " seconds)"
               << vcl_endl;
      vcl_cout << "generate geo_index based on location density, each leaf has size: " << size
               << " seconds in geographic cooridnates..\n";
      volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[t_id], (float)size, poly);
      // write the geo index structure
      vcl_stringstream file_name;  file_name << out_pre() << "geo_index_tile_" << t_id << ".txt";
      volm_geo_index::write(root, file_name.str(), (float)size);
      unsigned         depth = volm_geo_index::depth(root);
      vcl_stringstream file_name3;
      file_name3 << out_pre() << "geo_index_tile_" << t_id << "_depth_" << depth << ".kml";
      volm_geo_index::write_to_kml(root, depth, file_name3.str() );
      // loop over each leaf to add locations
      for( unsigned i = 0; i < infos.size(); i++ )
        {
        volm_img_info                        sat_info = infos[i];
        vcl_vector<volm_geo_index_node_sptr> leaves;
        volm_geo_index::get_leaves(root, leaves, sat_info.bbox);
        if( leaves.empty() )
          {
          continue;
          }
        // obtain the building polygons that currently intersect with given leaf
        float leaf_size = (float)leaves[0]->extent_.width();
        vcl_cout << leaves.size() << " leaves (" << leaf_size << " deg) intersects with the height map: "
                 << sat_info.name << vcl_flush << vcl_endl;
        for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
          {
          // obtain buildings that are inside leaf
          vcl_vector<vgl_polygon<double> > build_in_leaf;
          build_in_leaf.clear();
          for( unsigned b_idx = 0; b_idx < build_in_tile.size(); b_idx++ )
            {
            if( vgl_intersection(leaves[l_idx]->extent_, build_in_tile[b_idx]) )
              {
              build_in_leaf.push_back(build_in_tile[b_idx]);
              }
            }
          vcl_cout << build_in_leaf.size() << " are in leaf " << leaves[l_idx]->extent_ << vcl_flush << vcl_endl;
          if( !leaves[l_idx]->hyps_ )
            {
            leaves[l_idx]->hyps_ = new volm_loc_hyp();
            }
          float    lower_left_lon = (float)leaves[l_idx]->extent_.min_point().x();
          float    lower_left_lat = (float)leaves[l_idx]->extent_.min_point().y();
          unsigned nhi = (unsigned)vcl_ceil(leaf_size / inc_in_sec);
          for( unsigned hi = 0; hi < nhi; hi++ )
            {
            double lon = lower_left_lon + hi * inc_in_sec;
            for( unsigned hj = 0; hj < nhi; hj++ )
              {
              double               lat = lower_left_lat + hj * inc_in_sec;
              vgl_point_2d<double> pt(lon, lat);
              // check building from URGENT data
              bool is_building = false;
              for( unsigned i = 0; (i < build_in_leaf.size() && !is_building);  i++ )
                {
                is_building = loc_inside_region(build_in_leaf[i], lon, lat);
                }
              if( is_building )
                {
                continue;
                }
              // check building from classification map
              int type = find_land_type(class_map_infos, pt);
              if( (class_map_infos.size() && type < 0) ||
                  type == (int)volm_osm_category_io::volm_land_table_name["building"].id_ ||
                  type == (int)volm_osm_category_io::volm_land_table_name["tall_building"].id_ )
                {
                continue;
                }
              double u, v;
              sat_info.cam->global_to_img(lon, lat, 0.0, u, v);
              int ii = (int)vcl_floor(u + 0.5);
              int jj = (int)vcl_floor(v + 0.5);
              if( sat_info.valid_pixel(ii, jj) )
                {
                vil_image_view<float> img(sat_info.img_r);
                float                 z = img(ii, jj);
                unsigned              id;
                if( z > 0 && !(leaves[l_idx]->hyps_->exist(lat, lon, inc_in_sec_rad, id) ) )
                  {
                  leaves[l_idx]->hyps_->add(lat, lon, z);
                  }
                }
              }
            }
          }
        vcl_cout << "FINISH!!\n";
        }
      // write the hypo database
      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);
      vcl_cout << leaves.size() << " leaves have location hypotheses" << vcl_endl;
      for( unsigned i = 0; i < leaves.size(); i++ )
        {
        vcl_cout << "\t leaf: " << i << " has " << leaves[i]->hyps_->locs_.size() << " locations" << vcl_endl;
        }
      vcl_stringstream file_name4;  file_name4 << out_pre() << "geo_index_tile_" << t_id;
      vcl_cout << "\nwriting hypos to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(root, file_name4.str() );
      for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[l_idx]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[l_idx]->hyps_->write_to_kml(out_file, inc_in_sec_rad, false);
        }
      vcl_cout << volm_geo_index::hypo_size(root) << " locations are generated in tile " << t_id << vcl_endl;

      return volm_io::SUCCESS;
      }

    // generate locations with constant interval
    if( region_name().compare("all") == 0 )
      {
      // check whether the region is defined
      if( region_name() != "all" &&
          volm_osm_category_io::volm_land_table_name.find(region_name() ) ==
          volm_osm_category_io::volm_land_table_name.end() )
        {
        vcl_cout << "ERROR: unknown region name: " << region_name() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }
      // check input
      if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || world_id() < 0 )
        {
        vul_arg_display_usage_and_exit();
        vcl_cout << "ERROR: missing input for generating phase 1b location database on all non-building region"
                 << vcl_endl;
        }
      // load satellite/dem height images
      vcl_vector<volm_img_info> infos;
      if( use_satellite_img() )
        {
        volm_io_tools::load_satellite_height_imgs(in_folder(), infos, false, "");
        }
      else
        {
        volm_io_tools::load_aster_dem_imgs(in_folder(), infos);
        }

      // load satellite classification 2d map to avoid locations on top of the buildings
      vcl_cout << "height map resources: " << infos.size() << " geotiff images are loaded!\n";
      double   inc_in_meter = inc_in_sec * 21 / 0.000202;
      double   size = nh() * inc_in_sec;
      unsigned t_id = tile_id();
      vcl_cout << "generate location on land category: " << region_name() << vcl_endl;
      vcl_cout << "generation locations with interval " << inc_in_meter << " meter. (" << inc_in_sec << " seconds)"
               << vcl_endl;
      vcl_cout << "generate geo_index based on location density, each leaf has size: " << size
               << " seconds in geographic cooridnates..\n";
      volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[t_id], (float)size, poly);
      // write the geo index structure
      vcl_stringstream file_name;  file_name << out_pre() << "geo_index_tile_" << t_id << ".txt";
      volm_geo_index::write(root, file_name.str(), (float)size);
      unsigned         depth = volm_geo_index::depth(root);
      vcl_stringstream file_name3;
      file_name3 << out_pre() << "geo_index_tile_" << t_id << "_depth_" << depth << ".kml";
      volm_geo_index::write_to_kml(root, depth, file_name3.str() );
      // loop over each leaf to add locations
      for( unsigned i = 0; i < infos.size(); i++ )
        {
        volm_img_info                        sat_info = infos[i];
        vcl_vector<volm_geo_index_node_sptr> leaves;
        volm_geo_index::get_leaves(root, leaves, sat_info.bbox);
        if( leaves.empty() )
          {
          continue;
          }
        float leaf_size = (float)leaves[0]->extent_.width();
        vcl_cout << leaves.size() << " leaves (" << leaf_size << " deg) intersects with the height map: "
                 << sat_info.name << vcl_endl;
        for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
          {
          if( !leaves[l_idx]->hyps_ )
            {
            leaves[l_idx]->hyps_ = new volm_loc_hyp();
            }
          float    lower_left_lon = (float)leaves[l_idx]->extent_.min_point().x();
          float    lower_left_lat = (float)leaves[l_idx]->extent_.min_point().y();
          unsigned nhi = (unsigned)vcl_ceil(leaf_size / inc_in_sec);
          for( unsigned hi = 0; hi < nhi; hi++ )
            {
            double lon = lower_left_lon + hi * inc_in_sec;
            for( unsigned hj = 0; hj < nhi; hj++ )
              {
              double               lat = lower_left_lat + hj * inc_in_sec;
              vgl_point_2d<double> pt(lon, lat);
              double               u, v;
              sat_info.cam->global_to_img(lon, lat, 0.0, u, v);
              int ii = (int)vcl_floor(u + 0.5);
              int jj = (int)vcl_floor(v + 0.5);
              if( sat_info.valid_pixel(ii, jj) )
                {
                vil_image_view<float> img(sat_info.img_r);
                float                 z = img(ii, jj);
                unsigned              id;
                if( z > 0 && !(leaves[l_idx]->hyps_->exist(lat, lon, inc_in_sec_rad, id) ) )
                  {
                  leaves[l_idx]->hyps_->add(lat, lon, z);
                  }
                }
              }
            }
          }
        }
      // write the hypo database
      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);
      vcl_stringstream file_name4;  file_name4 << out_pre() << "geo_index_tile_" << t_id;
      vcl_cout << "\nwriting hypos to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(root, file_name4.str() );
      for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[l_idx]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[l_idx]->hyps_->write_to_kml(out_file, inc_in_sec_rad, true);
        }
      vcl_cout << volm_geo_index::hypo_size(root) << " locations are generated in tile " << t_id << vcl_endl;

      return volm_io::SUCCESS;

      }

    } // end of p1b location generation

  // generate locations along the OSM road network and along the coastline sand region
  if( p1a() )
    {
    // input check
    if( out_pre().compare("") == 0 || in_poly().compare("") == 0 )
      {
      vul_arg_display_usage_and_exit();  return volm_io::EXE_ARGUMENT_ERROR;
      }
    vcl_cout << "will use increments: " << inc() << " arcseconds along north and east directions!\n";
    // parse roi polygon
    vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly() );
    vcl_cout << "ROI poly has: " << poly[0].size() << " points" << vcl_endl;
    // create volm tile
    vcl_vector<volm_tile> tiles;
    if( world_id() == 1 ) {tiles = volm_tile::generate_p1_wr1_tiles(); }
    else if( world_id() == 2 )
      {
      tiles = volm_tile::generate_p1_wr2_tiles();
      }
    else {  vcl_cout << "ERROR: unknown world id in phase 1a" << vcl_endl;  return false;  }
    vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
    unsigned t_id = tile_id();
    if( t_id >= tiles.size() )
      {
      vcl_cout << "ERROR: unknown tile id: " << t_id << vcl_endl;  return volm_io::EXE_ARGUMENT_ERROR;
      }
    double meter_to_sec = volm_io_tools::meter_to_seconds(tiles[t_id].lat_, tiles[t_id].lon_);
    // create and write geo index for locations
    float                    size = 0.1f; // in seconds, set a fixed size for the leaves
    volm_geo_index_node_sptr hyp_root = volm_geo_index::construct_tree(tiles[t_id], (float)size, poly);
    if( region_name().compare("") == 0 )
      {
      vcl_stringstream file_name;  file_name << out_pre() << "geo_index_tile_" << t_id << ".txt";
      volm_geo_index::write(hyp_root, file_name.str(), (float)size);
      unsigned depth = volm_geo_index::depth(hyp_root);
      // vcl_stringstream file_name2;  file_name2 << out_pre() << "geo_index_tile" << t_id << ".kml";
      // volm_geo_index::write_to_kml(hyp_root, 0, file_name2.str());
      vcl_stringstream file_name3;
      file_name3 << out_pre() << "geo_index_tile_" << t_id << "_depth_" << depth << ".kml";
      volm_geo_index::write_to_kml(hyp_root, depth, file_name3.str() );
      }

    // generate ground truth only locations database
    if( only_gt() && add_gt().compare("") != 0 )
      {
      // only add the ground truth points in the files as they are given (using the elev in the ground truth file)
      // load ground truth file
      vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
      int                                                                                           cnt =
        volm_io::read_gt_file(add_gt(), samples);
      vcl_cout << "adding " << cnt << " ground truth locations!\n";
      // add locations into hypo database
      for( int i = 0; i < cnt; i++ )
        {
        if( poly.contains(samples[i].first.x(), samples[i].first.y() ) )
          {
          vcl_cout << samples[i].second.first.first << " adding.. " << samples[i].first.y() << ", "
                   << samples[i].first.x() << ' ';
          // obtain the gt elev from ground truth file
          bool added = volm_geo_index::add_hypothesis(hyp_root, samples[i].first.x(),
                                                      samples[i].first.y(), samples[i].first.z() );
          if( added ) {vcl_cout << " succeed!\n"; }
          else {vcl_cout << " not found in tree of tile: " << tile_id() << "!\n"; }
          }
        else
          {
          vcl_cout << samples[i].second.first.first << " is not inside ROI polygons!\n";
          }
        }
      vcl_cout << "after addition of ground locations, hyp_root " << t_id << " has total " << volm_geo_index::hypo_size(
        hyp_root) << " hypotheses in its leaves!\n";

      // write the hypos
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << t_id;
      vcl_cout << "\nwriting hypos to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str() );
      // write the hypos into kml file for visualization
      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
      for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[l_idx]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[l_idx]->hyps_->write_to_kml(out_file, inc_in_sec_rad, true);
        }
      return volm_io::SUCCESS;
      }

    // generate osm road network only locations database
    if( use_osm_roads() )
      {
      if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || osm_bin_file().compare("") == 0 ||
          osm_tree_folder().compare("") == 0 || world_id() < 0 ||
          land_class_map_folder().compare("") == 0 )
        {
        vul_arg_display_usage_and_exit();
        vcl_cout << "ERROR: missing input for generating phase 1a OSM road network " << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }
      unsigned t_id = tile_id();
      // load OSM database
      volm_osm_objects osm_objs;
      double           min_size;
      vcl_stringstream file_name_pre;
      file_name_pre << osm_tree_folder() << "geo_index2_wr" << world_id() << "_tile_" << t_id;
      volm_geo_index2_node_sptr root = volm_io_tools::read_osm_data_and_tree(file_name_pre.str(),
                                                                             osm_bin_file(), osm_objs, min_size);
      if( !root )
        {
        vcl_cout << "ERROR: loading osm database failed, check file " << file_name_pre.str() << " or "
                 << osm_bin_file() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }
      vcl_stringstream kml_roads;
      kml_roads << out_pre() << "/p1b_wr" << world_id() << "_tile_" << tile_id() << "_osm_roads.kml";
      osm_objs.write_lines_to_kml(kml_roads.str() );
      // load LIDAR height data
      vcl_vector<volm_img_info> lidar_infos;
      volm_io_tools::load_lidar_imgs(in_folder(), lidar_infos);
      // load land classification data
      vcl_vector<volm_img_info> class_infos;
      volm_io_tools::load_imgs(land_class_map_folder(), class_infos, true, true, true);
      vcl_cout << lidar_infos.size() << " LIDAR images are successfully loaded!" << vcl_endl;
      vcl_cout << class_infos.size() << " 2d class map are successfully loaded!" << vcl_endl;

      // add locations along the road network
      if( land().compare("") == 0 )
        {
        vcl_cout << "generate locations along the roads with constant density" << vcl_endl;
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        for( unsigned k = 0; k < roads.size(); k++ )
          {
          volm_osm_object_line_sptr r = roads[k];
          if( r->prop().name_ == "water_river" || r->prop().name_ == "water_stream" ||
              r->prop().name_ == "water_large_river"  || r->prop().name_ == "water_canal" )
            {
            continue;
            }
          vcl_vector<vgl_point_2d<double> > points = r->line();
          for( unsigned kk = 1; kk < points.size(); kk++ )
            {
            if( loc_inside_region(poly, points[kk - 1].x(), points[kk - 1].y() ) )
              {
              add_hypo(hyp_root, lidar_infos, class_infos, points[kk - 1], inc_in_sec_rad, true);
              }
            // now interpolate along a straight line, assume locally planar
            double   dif_dy = points[kk].y() - points[kk - 1].y();
            double   dif_dx = points[kk].x() - points[kk - 1].x();
            double   ds = vcl_sqrt(dif_dy * dif_dy + dif_dx * dif_dx);
            double   inc_dy = inc_in_sec * (dif_dy / ds);
            double   inc_dx = inc_in_sec * (dif_dx / ds);
            unsigned cnt = 0;
            while( ds > inc_in_sec )
              {
              ds = ds - inc_in_sec;  cnt++;
              double x = points[kk - 1].x() + inc_dx * cnt;  double y = points[kk - 1].y() + inc_dy * cnt;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, lidar_infos, class_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, true);
                }
              }
            }
          }
        }
      else
        {
        vcl_cout << "generate locations along the roads and location density is defined by NLCD land category"
                 << vcl_endl;
        vcl_vector<volm_img_info> info_tmp;
        volm_io_tools::load_nlcd_imgs(land(), info_tmp);
        if( info_tmp.size() != tiles.size() )
          {
          vcl_cout << "ERROR: mismatch in create tiles " << tiles.size() << " and loaded " << info_tmp.size()
                   << " nlcd images, check input nlcd folder\n";
          return false;
          }
        vcl_vector<volm_img_info> nlcd_infos;
        for( unsigned i = 0; i < tiles.size(); i++ )
          {
          for( unsigned j = 0; j < tiles.size(); j++ )
            {
            double diff_lon = tiles[i].bbox_double().min_x() - info_tmp[j].bbox.min_x();
            double diff_lat = tiles[i].bbox_double().min_y() - info_tmp[j].bbox.min_y();
            double diff = vcl_sqrt(diff_lon * diff_lon + diff_lat * diff_lat);
            if( diff < 0.25 )
              {
              nlcd_infos.push_back(info_tmp[j]);
              break;
              }
            }
          }
        vcl_cout << nlcd_infos.size() << " NLCD images are successfully loaded!\n";
        // start to add locations along all roads
        vcl_vector<volm_osm_object_line_sptr>& roads = osm_objs.loc_lines();
        vcl_cout << "will process " << roads.size() << " roads...\n" << vcl_flush;
        for( unsigned r_idx = 0; r_idx < roads.size(); r_idx++ )
          {
          if( r_idx % 1000 == 0 ) {vcl_cout << r_idx << '.' << vcl_flush; }
          volm_osm_object_line_sptr r = roads[r_idx];
          if( r->prop().name_ == "water_river" || r->prop().name_ == "water_stream" ||
              r->prop().name_ == "water_large_river"  || r->prop().name_ == "water_canal" )
            {
            continue;
            }
          // obtain the road network that is inside the tile
          vcl_vector<vgl_point_2d<double> > pts = r->line();
          vcl_string                        name = r->prop().name_;
          if( pts.empty() )
            {
            continue;
            }
          int type_prev = find_land_type(nlcd_infos, pts[0]);
          if( type_prev < 0 )
            {
            inc_in_sec = 4 * meter_to_sec;  // if the type is unknown, use the smallest possible interval to be safe
            }
          else
            {
            inc_in_sec = volm_osm_category_io::geo_land_hyp_increments[type_prev] * meter_to_sec;
            }
          inc_in_sec_rad = inc_in_sec * inc_in_sec_radius_ratio;
          double remainder = 0.0;
          if( loc_inside_region(poly, pts[0].x(), pts[0].y() ) )
            {
            add_hypo(hyp_root, lidar_infos, class_infos, pts[0], inc_in_sec_rad, true);
            }
          for( unsigned kk = 1; kk < pts.size(); kk++ )
            {
            double prev_x = pts[kk - 1].x();  double prev_y = pts[kk - 1].y();
            int    type = find_land_type(nlcd_infos, pts[kk]);
            if( type > 0 && type != type_prev )
              {
              double inc_now = volm_osm_category_io::geo_land_hyp_increments[type] * meter_to_sec;
              inc_in_sec = inc_in_sec < inc_now ? inc_in_sec : inc_now;  // pick the smaller of the increments if different types
              inc_in_sec_rad = inc_in_sec * inc_in_sec_radius_ratio;
              }
            // interpolate along a straight line, assume locally planar
            double dif_dy = pts[kk].y() - pts[kk - 1].y();
            double dif_dx = pts[kk].x() - pts[kk - 1].x();
            double ds = vcl_sqrt(dif_dy * dif_dy + dif_dx * dif_dx);
            if( inc_in_sec > ds )
              {
              remainder += ds;
              type_prev = type;
              continue;
              }
            double cos = dif_dx / ds;          double sin = dif_dy / ds;
            double inc_dy = inc_in_sec * sin;  double inc_dx = inc_in_sec * cos;
            // get rid of remainder first
            if( remainder < inc_in_sec )
              {
              double rem = inc_in_sec - remainder;
              double inc_dy_rem = rem * sin;
              double inc_dx_rem = rem * cos;
              double x = prev_x + inc_dx_rem;
              double y = prev_y + inc_dy_rem;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, lidar_infos, class_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, true);
                }
              prev_x = x;
              prev_y = y;
              ds -= rem;
              }
            while( ds > inc_in_sec )
              {
              ds -= inc_in_sec;
              double x = prev_x + inc_dx;  double y = prev_y + inc_dy;
              if( loc_inside_region(poly, x, y) )
                {
                add_hypo(hyp_root, lidar_infos, class_infos, vgl_point_2d<double>(x, y), inc_in_sec_rad, false);
                }
              prev_x = x;
              prev_y = y;
              }

            type_prev = type;
            remainder = ds;
            }
          } // end of road loop
        }   // end of if-else statement where NLCD is used or not

      // write the hypos
      vcl_stringstream file_name4;  file_name4 << out_pre() << "geo_index_tile_" << t_id;
      vcl_cout << "\nwriting hypos to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(hyp_root, file_name4.str() );
      vcl_cout << volm_geo_index::hypo_size(hyp_root) << " locations are generated in tile " << t_id << vcl_endl;

      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(hyp_root, leaves);
      for( unsigned jj = 0; jj < leaves.size(); jj++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[jj]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[jj]->hyps_->write_to_kml(out_file, inc_in_sec_rad);
        }
      return volm_io::SUCCESS;
      } // end of road locations generation

    // generate locations only on certain types of locations with a given density
    if( region_name().compare("") != 0 )
      {
      // check whether the region is defined
      if( volm_osm_category_io::volm_land_table_name.find(region_name() ) ==
          volm_osm_category_io::volm_land_table_name.end() )
        {
        vcl_cout << "ERROR: unknown region name: " << region_name() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }
      // check input
      if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || world_id() < 0 ||
          land_class_map_folder().compare("") == 0 )
        {
        vul_arg_display_usage_and_exit();
        vcl_cout << "ERROR: missing input for generating phase 1a region based location network" << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
        }

      double inc_in_meter = inc_in_sec * 21 / 0.000202;
      vcl_cout << "generate locations on land category: " << region_name()
               << " with location distance " << inc_in_meter << " meter. (" << inc_in_sec << " seconds)" << vcl_endl;
      // determine depth of the geo index depending on inc
      double size = nh() * inc_in_sec;
      vcl_cout << "generate geo_index based on location density, each leaf has size: " << size
               << " seconds in geographic coords..\n";

      volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[t_id], (float)size, poly);
      // write the geo index structure
      vcl_stringstream file_name;  file_name << out_pre() << "geo_index_tile_" << t_id << ".txt";
      volm_geo_index::write(root, file_name.str(), (float)size);
      unsigned         depth = volm_geo_index::depth(root);
      vcl_stringstream file_name3;
      file_name3 << out_pre() << "geo_index_tile_" << t_id << "_depth_" << depth << ".kml";
      volm_geo_index::write_to_kml(root, depth, file_name3.str() );
      // load LIDAR
      vcl_vector<volm_img_info> lidar_infos;
      volm_io_tools::load_lidar_imgs(in_folder(), lidar_infos);
      // load land class map
      vcl_vector<volm_img_info> class_infos;
      volm_io_tools::load_imgs(land_class_map_folder(), class_infos, true, true, true);
      vcl_cout << lidar_infos.size() << " LIDAR images are successfully loaded!" << vcl_endl;
      vcl_cout << class_infos.size() << " 2d class map are successfully loaded!" << vcl_endl;
      // loop over each leaf to add locations
      for( unsigned i = 0; i < lidar_infos.size(); i++ )
        {
        volm_img_info                        lidar_info = lidar_infos[i];
        vcl_vector<volm_geo_index_node_sptr> leaves;
        volm_geo_index::get_leaves(root, leaves, lidar_info.bbox);
        if( leaves.empty() )
          {
          continue;
          }
        float leaf_size = (float)leaves[0]->extent_.width();
        vcl_cout << leaves.size() << " leaves (" << leaf_size << " deg) intersects with lidar image: "
                 << lidar_info.name << vcl_endl;
        for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
          {
          if( !leaves[l_idx]->hyps_ )
            {
            leaves[l_idx]->hyps_ = new volm_loc_hyp();
            }
          float    lower_left_lon = (float)leaves[l_idx]->extent_.min_point().x();
          float    lower_left_lat = (float)leaves[l_idx]->extent_.min_point().y();
          unsigned nhi = (unsigned)vcl_ceil(leaf_size / inc_in_sec);
          for( unsigned hi = 0; hi < nhi; hi++ )
            {
            double lon = lower_left_lon + hi * inc_in_sec;
            for( unsigned hj = 0; hj < nhi; hj++ )
              {
              double               lat = lower_left_lat + hj * inc_in_sec;
              vgl_point_2d<double> pt(lon, lat);
              int                  type = find_land_type(class_infos, pt);
              if( type > 0 )
                {
                if( volm_osm_category_io::volm_land_table[(unsigned char)type].name_ == region_name() )
                  {
                  double u, v;
                  lidar_info.cam->global_to_img(lon, lat, 0.0, u, v);
                  int ii = (int)vcl_floor(u + 0.5);
                  int jj = (int)vcl_floor(v + 0.5);
                  if( lidar_info.valid_pixel(ii, jj) )
                    {
                    vil_image_view<float> img(lidar_info.img_r);
                    float                 z = img(ii, jj);
                    unsigned              id;
                    if( z > 0 && !(leaves[l_idx]->hyps_->exist(lat, lon, inc_in_sec_rad, id) ) )
                      {
                      leaves[l_idx]->hyps_->add(lat, lon, z);
                      }
                    }
                  }
                else if( region_name() == "all" )    // generate constant post locations everywhere
                  {
                  double u, v;
                  lidar_info.cam->global_to_img(lon, lat, 0.0, u, v);
                  int ii = (int)vcl_floor(u + 0.5);
                  int jj = (int)vcl_floor(v + 0.5);
                  if( lidar_info.valid_pixel(ii, jj) )
                    {
                    vil_image_view<float> img(lidar_info.img_r);
                    float                 z = img(ii, jj);
                    unsigned              id;
                    if( z > 0 && !(leaves[l_idx]->hyps_->exist(lat, lon, inc_in_sec_rad, id) ) )
                      {
                      leaves[l_idx]->hyps_->add(lat, lon, z);
                      }
                    }
                  }
                }
              }
            }
          } // end of loop over current lidar image
        }   // end of loop over all lidar images

      // write the hypo database
      vcl_vector<volm_geo_index_node_sptr> leaves;
      volm_geo_index::get_leaves_with_hyps(root, leaves);
      vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << t_id;
      vcl_cout << "\nwriting hypos to: " << file_name4.str() << vcl_endl;
      volm_geo_index::write_hyps(root, file_name4.str() );
      for( unsigned l_idx = 0; l_idx < leaves.size(); l_idx++ )
        {
        vcl_string out_file = vul_file::strip_extension(leaves[l_idx]->get_hyp_name(file_name4.str() ) ) + ".kml";
        leaves[l_idx]->hyps_->write_to_kml(out_file, inc_in_sec_rad, true);
        }
      vcl_cout << volm_geo_index::hypo_size(root) << " locations are generated in tile " << t_id << vcl_endl;
      return volm_io::SUCCESS;
      } // end of region generation

    } // end of p1A options

#if 0
  // for P1A and P1B regions reading (world_id 6 and 7 becomes desert and coast)
  // change to wr1 tiles for desert
  vcl_vector<volm_tile> tiles;
  if( world_id() == 1 ) {tiles = volm_tile::generate_p1b_wr1_tiles(); }
  else if( world_id() == 2 )
    {
    tiles = volm_tile::generate_p1b_wr2_tiles();
    }
  else if( world_id() == 3 )
    {
    tiles = volm_tile::generate_p1b_wr3_tiles();
    }
  else if( world_id() == 4 )
    {
    tiles = volm_tile::generate_p1b_wr4_tiles();
    }
  else if( world_id() == 5 )
    {
    tiles = volm_tile::generate_p1b_wr5_tiles();
    }
  else if( world_id() == 6 )
    {
    tiles = volm_tile::generate_p1_wr1_tiles();
    }
  else if( world_id() == 7 )
    {
    tiles = volm_tile::generate_p1_wr2_tiles();
    }
  vcl_cout << " number of tiles: " << tiles.size() << vcl_endl;
  unsigned i = tile_id();
  if( i >= tiles.size() )
    {
    vcl_cerr << "tile id: " << i << " is greater than number of tiles: " << tiles.size() << "!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
    }
#endif

  if( read() )
    {
    if( out_pre().compare("") == 0 )
      {
      vul_arg_display_usage_and_exit();
      return volm_io::EXE_ARGUMENT_ERROR;
      }
    unsigned                             t_id = tile_id();
    float                                min_s;
    vcl_stringstream                     file_name; file_name << out_pre() << "geo_index_tile_" << t_id;
    volm_geo_index_node_sptr             root = volm_geo_index::read_and_construct(file_name.str() + ".txt", min_s);
    vcl_vector<volm_geo_index_node_sptr> leaves2;
    volm_geo_index::get_leaves(root, leaves2);
    vcl_cout << "\t number of leaves: " << leaves2.size() << vcl_endl;
    volm_geo_index::read_hyps(root, file_name.str() );
    vcl_cout << " read hyps!\n";
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);
    vcl_cout << "Geo index for tile: " << t_id << " stored in: " << file_name.str() << '\n'
             << "\t number of leaves with hyps: " << leaves.size() << vcl_endl;
    unsigned size = volm_geo_index::hypo_size(root);
    vcl_cout << "\t total number of hypos: " << size << vcl_endl;
    for( unsigned l = 0; l < leaves.size(); l++ )
      {
      vcl_cout << " leaf_id = " << l << " leaf bbox = " << leaves[l]->extent_ << vcl_endl;
      unsigned num_hyps = leaves[l]->hyps_->size();
      for( unsigned h = 0; h < num_hyps; h++ )
        {
        vgl_point_3d<double> h_pt = leaves[l]->hyps_->locs_[h];
        }
      }
    return volm_io::SUCCESS;
    }

#if 0
  if( in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0 )
    {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
    }
  vcl_cout << "will use increments " << inc() << " arcseconds along north and east directions!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly() );
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  // determine depth of the geo index depending on inc, if we want to have 100x100 = 10K hyps in each leaf
  double size = nh() * inc_in_sec; // inc() is given in arcseconds, convert it to seconds;
  vcl_cout << " each leaf has size: " << size << " seconds in geographic coords..\n"
           << " increments in seconds: " << inc_in_sec << '\n'
           << " increments in meters: " << (inc_in_sec * 21 / 0.000202) << '\n'
           << " only putting hyps in UTM zone: " << utm_zone() << '\n';

  volm_geo_index_node_sptr root = volm_geo_index::construct_tree(tiles[i], (float)size, poly);

  // write the geo index and the hyps
  vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << ".txt";
  volm_geo_index::write(root, file_name.str(), (float)size);
  unsigned         depth = volm_geo_index::depth(root);
  vcl_stringstream file_name2; file_name2 << out_pre() << "geo_index_tile_" << i << ".kml";
  volm_geo_index::write_to_kml(root, 0, file_name2.str() );
  vcl_stringstream file_name3; file_name3 << out_pre() << "geo_index_tile_" << i << "_depth_" << depth << ".kml";
  volm_geo_index::write_to_kml(root, depth, file_name3.str() );

  // prune the out of zone leaves
  if( !volm_geo_index::prune_by_zone(root, utm_zone() ) )
    {
    vcl_cout << " root " << i << " is not in zone: " << utm_zone() << "! no hypotheses in its leaves!\n";
    return 0;
    }
  if( !only_gt() )
    {
    vcl_string file_glob = in_folder() + "/*.tif";
    unsigned   cnt = 0;
    for( vul_file_iterator fn = file_glob; fn; ++fn, ++cnt )
      {
      vcl_string tiff_fname = fn();

      vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str() );
      vil_image_view<float>    img(img_sptr);
      unsigned                 ni = img.ni(); unsigned nj = img.nj();
      volm_tile                t(tiff_fname, ni, nj);
      // t.write_kml(out_pre() + t.get_string() + ".kml", 0);

      // write the geo index and the hyps
      vcl_vector<volm_geo_index_node_sptr> leaves;
      vgl_box_2d<double>                   leaf_box = t.bbox_double();
      volm_geo_index::get_leaves(root, leaves, leaf_box);
      if( !leaves.size() )
        {
        continue;
        }
      float size_leaf = (float)leaves[0]->extent_.width();
#  if 0
      volm_index_node_sptr dummy_root = new volm_index_node(t.bbox() );
      dummy_root->children_ = leaves;
      vcl_stringstream file_name; file_name << out_pre() << "geo_index_tile_" << i << "_intersection_" << cnt << ".kml";
      volm_index::write_to_kml(dummy_root, 1, file_name.str() );
#  endif
      // generate the hyps and find heights from LIDAR
      for( unsigned j = 0; j < leaves.size(); ++j )
        {
        if( !leaves[j]->hyps_ )
          {
          leaves[j]->hyps_ = new volm_loc_hyp();
          }
        float    lower_left_lon = (float)leaves[j]->extent_.min_point().x();
        float    lower_left_lat = (float)leaves[j]->extent_.min_point().y();
        unsigned nhi = (unsigned)vcl_ceil(size_leaf / inc_in_sec);
        for( unsigned hi = 0; hi < nhi; ++hi )
          {
          double lon = lower_left_lon + hi * inc_in_sec;
          for( unsigned hj = 0; hj < nhi; ++hj )
            {
            double   lat = lower_left_lat + hj * inc_in_sec;
            vpgl_utm u; int zone;  double x, y;
            u.transform(lat, lon, x, y, zone);
            if( zone != (int)utm_zone() )
              {
              continue;
              }
            unsigned ii, jj;
            bool     contains = t.global_to_img(lon, lat, ii, jj);
            if( contains )
              {
              float    z = img(ii, jj);
              unsigned id;
              if( z > 0  && !(leaves[j]->hyps_->exist(lat, lon, inc_in_sec_rad, id) ) )
                {
                leaves[j]->hyps_->add(lat, lon, z);
                }
              }
            }
          }
        }

      // if (cnt > 0)
      //  break;
      }
    }
  unsigned r_cnt = volm_geo_index::hypo_size(root);
  vcl_cout << " root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";

  if( add_gt().compare("") != 0 )    // user passed the path to a text file with the gt locations

    {// load the images
    vcl_string                                                  file_glob = in_folder() + "/*.tif";
    vcl_vector<vcl_pair<vil_image_view_base_sptr,  volm_tile> > tiles;
    for( vul_file_iterator fn = file_glob; fn; ++fn )
      {
      vcl_string tiff_fname = fn();

      vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str() );
      unsigned                 ni = img_sptr->ni(); unsigned nj = img_sptr->nj();
      volm_tile                t(tiff_fname, ni, nj);
      tiles.push_back(vcl_pair<vil_image_view_base_sptr, volm_tile>(img_sptr, t) );
      }

#  if 0  // : add any gt positions if any
    if( volm_geo_index::add_hypothesis(root, -79.857689, 32.759063, 1.60) )
      {
      vcl_cout << " added p1a_test1_06-GROUNDTRUTH\n";
      }

    if( volm_geo_index::add_hypothesis(root, -79.813014, 32.775959, 4.41) )
      {
      vcl_cout << " added p1a_test1_28-GROUNDTRUTH\n";
      }
    if( volm_geo_index::add_hypothesis(root, -78.282153, 33.911997, 1.60) )
      {
      vcl_cout << " added p1a_test1_08-GROUNDTRUTH\n";
      }
    if( volm_geo_index::add_hypothesis(root, -78.281430, 33.912397, 1.60) )
      {
      vcl_cout << " added p1a_test1_46-GROUNDTRUTH\n";
      }
    if( volm_geo_index::add_hypothesis(root, -81.550366, 30.720336, 1.60) )
      {
      vcl_cout << " added p1a_test1_18-GROUNDTRUTH\n";
      }
    if( volm_geo_index::add_hypothesis(root, -79.951930, 32.648980, 1.60) )
      {
      vcl_cout << " added p1a_test1_38-GROUNDTRUTH\n";
      }
    if( volm_geo_index::add_hypothesis(root, -79.268871, 33.365799, 1.60) )
      {
      vcl_cout << " added p1a_test1_34-GROUNDTRUTH\n";
      }
#  endif

    vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
    int                                                                                           cnt =
      volm_io::read_gt_file(add_gt(), samples);
    vcl_cout << " adding " << cnt << " gt locs!\n";
    for( int j = 0; j < cnt; ++j )
      {
      vpgl_utm u; int zone;  double x, y;
      u.transform(samples[j].first.y(), samples[j].first.x(), x, y, zone);
      if( zone != (int)utm_zone() )
        {
        vcl_cout << samples[j].second.first.first << " is in zone: " << zone << " not in " << utm_zone()
                 << " skipping!\n";
        continue;
        }
      vcl_cout << samples[j].second.first.first << " adding.. " << samples[j].first.y() << ", "
               << samples[j].first.x() << ' ';

      // find which box contains it
      bool added = false;
      for( unsigned kk = 0; kk < tiles.size(); kk++ )
        {
        unsigned ii, jj;
        bool     contains = tiles[kk].second.global_to_img(samples[j].first.x(), samples[j].first.y(), ii, jj);
        if( contains )
          {
          vil_image_view<float> img(tiles[kk].first);
          float                 z = img(ii, jj);
          // check the neighborhood
          for( int ii2 = ii - 1; ii2 <= (int)(ii + 1); ii2++ )
            {
            for( int jj2 = jj - 1; jj2 <= (int)(jj + 1); jj2++ )
              {
              if( ii2 >= 0 && jj2 >= 0 && ii2 < (int)img.ni() && jj2 < (int)img.nj() )
                {
                if( z < img(ii2, jj2) ) {z = img(ii2, jj2); }
                }
              }
            }
          if( z > 0.0f )
            {
            vcl_cout << " corrected height from: " << samples[j].first.z() << " to: " << z + 1.6 << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), z + 1.6);
            }
          else
            {
            vcl_cout << " height from LIDAR is: " << z << " writing original height: " << samples[j].first.z() << '\n';
            added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(),
                                                   samples[j].first.z() );
            }
          break;
          }
        }
      // bool added = volm_geo_index::add_hypothesis(root, samples[j].first.x(), samples[j].first.y(), samples[j].first.z());
      if( added ) {vcl_cout << " success!\n"; }
      else {vcl_cout << " not found in tree of tile: " << tile_id() << "!\n"; }
      }

    unsigned r_cnt = volm_geo_index::hypo_size(root);
    vcl_cout << " after addition of gt locs, root " << i << " has total " << r_cnt << " hypotheses in its leaves!\n";
    }

  // write the hypos
  vcl_stringstream file_name4; file_name4 << out_pre() << "geo_index_tile_" << i;
  vcl_cout << "writing hyps to: " << file_name4.str() << vcl_endl;
  volm_geo_index::write_hyps(root, file_name4.str() );
// #if DEBUG
  vcl_vector<volm_geo_index_node_sptr> leaves;
  volm_geo_index::get_leaves_with_hyps(root, leaves);
  vcl_stringstream file_name5; file_name5 << out_pre() << "geo_index_tile_" << i << "_hyps.kml";
  leaves[0]->hyps_->write_to_kml(file_name5.str(), inc_in_sec);
// #endif

  vcl_cout << "total time: " << t.all() / 1000 << " seconds = " << t.all() / (1000 * 60) << " mins.\n";
  return volm_io::SUCCESS;
#endif

}
