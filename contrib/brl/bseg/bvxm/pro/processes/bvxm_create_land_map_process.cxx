// This is brl/bseg/bvxm/pro/processes/bvxm_create_land_map_process.cxx
#include "bvxm_create_land_map_process.h"
//:
// \file
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon_scan_iterator.h>

// set input and output type
bool bvxm_create_land_map_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_land_map_process_globals;
  // process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";      // voxel world spec
  input_types_[1] = "vcl_string";                 // geo cover image folder
  input_types_[2] = "vcl_string";                 // urban image folder
  input_types_[3] = "vcl_string";                 // open street map folder
  input_types_[4] = "bool";                       // option to ingest open street map points
  input_types_[5] = "bool";                       // option to ingest open street map road
  input_types_[6] = "bool";                       // option to ingest open street map region
  input_types_[7] = "bool";                       // option to covert pixel value from original image pixel to pre-define geo-label
  // process generates 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // land image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_create_land_map_process(bprb_func_process& pro)
{
  using namespace bvxm_create_land_map_process_globals;
  // get inputs
  unsigned in_i = 0;
  bvxm_voxel_world_sptr scene = pro.get_input<bvxm_voxel_world_sptr>(in_i++);
  std::string geo_cover_folder = pro.get_input<std::string>(in_i++);
  std::string urban_img_folder = pro.get_input<std::string>(in_i++);
  std::string osm_folder       = pro.get_input<std::string>(in_i++);
  bool       is_osm_pt        = pro.get_input<bool>(in_i++);
  bool       is_osm_rd        = pro.get_input<bool>(in_i++);
  bool       is_osm_rg        = pro.get_input<bool>(in_i++);
  bool       is_convert       = pro.get_input<bool>(in_i++);

  // get scene parameters
  bvxm_world_params_sptr params = scene->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();
  vgl_point_3d<float> lower_left = params->corner();
  vgl_vector_3d<unsigned> num_voxels = params->num_voxels();
  double dim_x, dim_y, dim_z;
  double lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev;
  lvcs->local_to_global(params->corner().x(), params->corner().y(), params->corner().z(), vpgl_lvcs::wgs84,
                        lower_left_lon, lower_left_lat, lower_left_elev);
  double dimx = params->num_voxels().x() * params->voxel_length();
  double dimy = params->num_voxels().y() * params->voxel_length();
  double dimz = params->num_voxels().z() * params->voxel_length();
  lvcs->local_to_global(params->corner().x() + dimx, params->corner().y() + dimy, params->corner().z() + dimz, vpgl_lvcs::wgs84,
                        upper_right_lon, upper_right_lat, upper_right_elev);
  vgl_box_2d<double> scene_bbox(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);

  // create an output images for the finest resolution
  unsigned ni, nj;
  ni = num_voxels.x();
  nj = num_voxels.y();
  auto* out_img = new vil_image_view<vxl_byte>(ni, nj, 1);
  out_img->fill(0);

  // load geo cover data
  std::vector<volm_img_info> geo_infos;
  volm_io_tools::load_geocover_imgs(geo_cover_folder, geo_infos);
  // load urban image data
  std::vector<volm_img_info> urban_infos;
  volm_io_tools::load_urban_imgs(urban_img_folder, urban_infos);
  // load osm data
  std::vector<volm_osm_objects> osm_objs;
  std::string file_glob = osm_folder + "//*.bin";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn)
    osm_objs.emplace_back(fn());

  std::cout << pro.name() << ":\n";
  std::cout << geo_infos.size() << " geo cover images are loaded from " << geo_cover_folder << std::endl;
  std::cout << urban_infos.size() << " urban images are loaded from " << urban_img_folder << std::endl;
  std::cout << osm_objs.size() << " OSM images are loaded form " << osm_folder << std::endl;

  // ingest geo cover images
  for (auto & geo_info : geo_infos)
  {
    if (vgl_intersection(scene_bbox, geo_info.bbox).is_empty())
      continue;
    auto* geo_img = dynamic_cast<vil_image_view<vxl_byte>*>(geo_info.img_r.ptr());
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        double lon, lat, gz;
        auto local_x = (float)(i+0.5);
        auto local_y = (float)(dimy - j + 0.5);
        lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
        double u, v;
        geo_info.cam->global_to_img(lon, lat, gz, u, v);
        auto uu = (unsigned)std::floor(u+0.5), vv = (unsigned)std::floor(v+0.5);
        if (uu < geo_info.ni && vv < geo_info.nj)
        {
          if (is_convert)
          {
            (*out_img)(i, j) = volm_osm_category_io::geo_land_table[(*geo_img)(uu,vv)].id_;
          }
          else
          {
            (*out_img)(i, j) = (*geo_img)(uu,vv);
          }
        }
      }
    }
  }
  // ingest urban image
  for (auto & urban_info : urban_infos)
  {
    if (vgl_intersection(scene_bbox, urban_info.bbox).is_empty())
      continue;
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        double lon, lat, gz;
        auto local_x = (float)(i+0+0.5);
          auto local_y = (float)(dimy-j+0.5);
          lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
          double u, v;
          urban_info.cam->global_to_img(lon, lat, gz, u, v);
          auto uu = (unsigned)std::floor(u+0.5);
          auto vv = (unsigned)std::floor(v+0.5);
          if (uu < urban_info.ni && vv < urban_info.nj)
            (*out_img)(i,j) = volm_osm_category_io::geo_land_table[volm_osm_category_io::GEO_URBAN].id_;
      }
    }
  }

  // ingest OSM data
  for (auto & osm_obj : osm_objs)
  {
    if (is_osm_rg)
    {
      unsigned n_regions = osm_obj.num_regions();
      for (unsigned r_idx = 0; r_idx < n_regions; r_idx++) {
        vgl_polygon<double> poly(osm_obj.loc_polys()[r_idx]->poly()[0]);
        // get rid off polygon with duplicated points
        bool ignore = false;
        for (unsigned i = 0; i < poly[0].size()-1; i++) {
          if (poly[0][i] == poly[0][i+1])
            ignore = true;
        }
        if (ignore)
          continue;
        if (!vgl_intersection(scene_bbox, poly))
          continue;
        unsigned char curr_level = osm_obj.loc_polys()[r_idx]->prop().level_;
        unsigned char curr_id = osm_obj.loc_polys()[r_idx]->prop().id_;
        // geo cover is already level 0 and therefore anything in osm with level 0 is ignored
        if (curr_level == 0 && curr_id != volm_osm_category_io::volm_land_table_name["Open_Water"].id_)
          continue;
        // go from geo coords wgs84 to local
        vgl_polygon<double> img_poly(1);

        if (curr_id != volm_osm_category_io::volm_land_table_name["building"].id_ &&
            curr_id != volm_osm_category_io::volm_land_table_name["Open_Water"].id_
            )  // only ingest buildings and water if necessary
            continue;
        for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
          double lx, ly, lz;
          lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
          double i = lx;  double j = dimy - ly;
          img_poly[0].push_back(vgl_point_2d<double>(i,j));
        }
        vgl_polygon_scan_iterator<double> it(img_poly, true);
        for (it.reset(); it.next();  ) {
          int y = it.scany();
          for (int x = it.startx(); x <= it.endx(); ++x) {
            if (x>=0 && y>=0 && x < (int)out_img->ni() && y < (int)out_img->nj())
              (*out_img)(x,y) = curr_id;
          }
        }
      }
    }
    if (is_osm_rd)
    {
      unsigned n_rds = osm_obj.loc_lines().size();
      for (unsigned r_idx = 0; r_idx < n_rds; r_idx++) {
        std::vector<vgl_point_2d<double> > road = osm_obj.loc_lines()[r_idx]->line();
        std::vector<vgl_point_2d<double> > line_geo;
        if (!volm_io_tools::line_inside_the_box(scene_bbox, road, line_geo))
          continue;
        std::vector<vgl_point_2d<double> > line_img;
        unsigned char curr_id = osm_obj.loc_lines()[r_idx]->prop().id_;
        double width = osm_obj.loc_lines()[r_idx]->prop().width_;
        for (auto & pt_idx : line_geo) {
          double lx, ly, lz;
          lvcs->global_to_local(pt_idx.x(), pt_idx.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
          double i = lx;
          double j = dimy - ly;
          if (i>=0 && j>= 0 && i<out_img->ni() && j<out_img->nj())
            line_img.emplace_back(i,j);
        }
        if (line_img.size() < 2)
          continue;
        if (width < 1.0) width = 1.1;
        vgl_polygon<double> img_poly;
        if (!volm_io_tools::expend_line(line_img, width, img_poly)) {
          std::cerr << pro.name() << ": expend line to polygon failed\n";
          return false;
        }
        // update the label
        vgl_polygon_scan_iterator<double> it(img_poly, true);
        for (it.reset(); it.next(); ) {
          int y = it.scany();
          for (int x = it.startx(); x <= it.endx(); ++x) {
            if (x>=0 && y>=0 && x < (int)out_img->ni() && y < (int)out_img->nj())
              (*out_img)(x,y) = curr_id;
          }
        }
      }
    }
    if (is_osm_pt)
    {
      std::vector<volm_osm_object_point_sptr> loc_pts = osm_obj.loc_pts();
      unsigned n_pts = loc_pts.size();
      for (unsigned p_idx = 0; p_idx < n_pts; p_idx++) {
        if (!scene_bbox.contains(loc_pts[p_idx]->loc()))
          continue;
        vgl_point_2d<double> pt = loc_pts[p_idx]->loc();
        double lx, ly, lz;
        lvcs->global_to_local(pt.x(), pt.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
        double i = lx;
        double j = dimy - ly;
        auto x = (unsigned)std::floor(i+0.5);
        auto y = (unsigned)std::floor(j+0.5);
        if (x < out_img->ni() && y < out_img->nj())
          (*out_img)(x,y) = loc_pts[p_idx]->prop().id_;
      }
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);
  return true;
}
