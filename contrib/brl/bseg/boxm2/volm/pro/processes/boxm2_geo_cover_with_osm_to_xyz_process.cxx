// This is brl/bseg/boxm2/volm/pro/process/boxm2_geo_cover_with_osm_to_xyz_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to prepare x y z images from geo_cover iamge with open stree map objects ingested on it
// NOTE: GeoCover image name is assumed to contain the bounding box information and vpgl_geo_camera is loaded from image header
//
// \author Yi Dong
// \date August 17, 2013

#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_osm_objects.h>
#include <volm/volm_io.h>
#include <volm/volm_category_io.h>
#include <volm/volm_tile.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_utm.h>
#include <volm/volm_io_tools.h>

namespace boxm2_geo_cover_with_osm_to_xyz_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 5; // temporary set 6 output for debuging purpose
}

bool boxm2_geo_cover_with_osm_to_xyz_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_geo_cover_with_osm_to_xyz_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";   // boxm2 scene
  input_types_[1] = "vcl_string";         // tiff geo cover image name
  input_types_[2] = "vcl_string";         // open street map road binary

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image
  output_types_[3] = "vil_image_view_base_sptr";  // label image
  output_types_[4] = "vil_image_view_base_sptr";  // color label image (for debug purpose)
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_geo_cover_with_osm_to_xyz_process(bprb_func_process& pro)
{
  using namespace boxm2_geo_cover_with_osm_to_xyz_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  std::string img_fname = pro.get_input<std::string>(i++);
  std::string osm_bin = pro.get_input<std::string>(i++);

  // find the bbox of scene and resolution of scene
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
  std::vector<boxm2_block_id> blks = scene->get_block_ids();
  // fetch the minimum voxel length
  float vox_length = 1E6;
  for (const auto & blk : blks) {
    boxm2_scene_info* info = scene->get_blk_metadata(blk);
    float sb_length = info->block_len;
    if (sb_length/8.0f < vox_length)  vox_length = sb_length/8.0f;
  }
  double orig_lat, orig_lon, orig_elev; scene->lvcs().get_origin(orig_lat, orig_lon, orig_elev);
  std::cout << "scene voxel length: " << vox_length << std::endl;
  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  double min_lon, min_lat, gz, max_lon, max_lat;
  lvcs->local_to_global(scene_bbox.min_point().x(), scene_bbox.min_point().y(), 0, vpgl_lvcs::wgs84, min_lon, min_lat, gz);
  lvcs->local_to_global(scene_bbox.max_point().x(), scene_bbox.max_point().y(), 0, vpgl_lvcs::wgs84, max_lon, max_lat, gz);
  vgl_box_2d<double> sbbox(min_lon, max_lon, min_lat, max_lat);
  std::cout << " scene bbox in geo coords: " << sbbox << std::endl;

  // find the bounding box of geo cover image from filename
  volm_tile t(img_fname, 0, 0);
  vgl_box_2d<double> bbox = t.bbox_double();
  std::cout << " image bounding box in geo coords: " << bbox << std::endl;

  // check whether the image intersects with the scene
  if (vgl_area(vgl_intersection(bbox, sbbox)) <= 0) {
    std::cout << " scene does not intersect with the image: " << img_fname << std::endl;
    return false;
  }

  // load the geo_cover image
  if (!vul_file::exists(img_fname)) {
    std::cout << pro.name() << " can not find the image: " << img_fname << std::endl;
    return false;
  }
  if (!vul_file::exists(osm_bin)) {
    std::cout << pro.name() << " can not find the osm binary: " << osm_bin << std::endl;
    return false;
  }
  vil_image_view_base_sptr img_sptr = vil_load(img_fname.c_str());
  unsigned nii, nji;
  if (auto* img = dynamic_cast<vil_image_view<vxl_byte> * >(img_sptr.ptr())) {
    nii = img->ni();  nji = img->nj();
  }

  // get the camera from image name
  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(img_fname, nii, nji, lvcs, cam);
  double lon2, lat2;
  cam->img_to_global(nii, nji, lon2, lat2);
  vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);
  std::cout << "lower right corner in the image given by geocam is: " << lat2 << " " << lon2 <<  " zone: " << zone << std::endl;

  // prepare an image for the voxel resolution
  int ni = (int)std::ceil((scene_bbox.max_x()-scene_bbox.min_x()+1.0)/vox_length);
  int nj = (int)std::ceil((scene_bbox.max_y()-scene_bbox.min_y()+1.0)/vox_length);
  std::cout << " image size needs ni: " << ni << " nj: " << nj << " to support voxel res: " << vox_length << std::endl;

  // create x y z images
  auto* out_img_x = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_y = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_z = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_label = new vil_image_view<vxl_byte>(ni, nj, 1);
  auto* out_class_img = new vil_image_view<vil_rgb<vxl_byte> >(ni, nj, 1);
  auto* level_img = new vil_image_view<vxl_byte>(ni, nj, 1);
  // initialize the image
  out_img_x->fill(0.0f); out_img_y->fill(0.0f);
  // for z image, give a height below the scene to avoid rays that is outside of current geo_cover image
  out_img_z->fill((float)(scene_bbox.min_z()-100.0f));
  out_img_label->fill((vxl_byte)0);
  out_class_img->fill(volm_osm_category_io::volm_land_table[0].color_);
  level_img->fill(0);

  // iterator over the image an d fore each pixel, calculate, xyz in location coordinate system, ingest the geo cover label to the label image
  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      auto local_x = (float)(i*vox_length+scene_bbox.min_x()+vox_length/2.0);
      auto local_y = (float)(scene_bbox.max_y()-j*vox_length+vox_length/2.0);
      (*out_img_x)(i,j) = local_x;
      (*out_img_y)(i,j) = local_y;
      // transfer from scene local to geo coords
      double lon, lat, gz;
      lvcs->local_to_global(local_x, local_y, 0, vpgl_lvcs::wgs84, lon, lat, gz);
      // find the pixel in geo cover image
      double u,v;
      // transfer all wgs84 to positive value since geo_cam handles the hemipshere and direction already
      if (lon < 0)  lon = -lon;
      if (lat < 0)  lat = -lat;
      cam->global_to_img(lon, lat, gz, u, v);
      auto uu = (unsigned)std::floor(u + 0.5);
      auto vv = (unsigned)std::floor(v + 0.5);
      if (uu > 0 && vv > 0 && uu < nii && vv < nji) {
        if (auto* img = dynamic_cast<vil_image_view<vxl_byte> * >(img_sptr.ptr())) {
          (*out_img_z)(i,j) = (float)(scene_bbox.max_z()+100.0f);  // make the ray origin above all surface if the pixel is valid
          (*out_class_img)(i,j) = volm_osm_category_io::geo_land_table[(*img)(uu,vv)].color_;
          (*out_img_label)(i,j) = volm_osm_category_io::geo_land_table[(*img)(uu,vv)].id_;
        }
      }
    }
  }

  // ingest open street map to label image
  // create a level image to records the level of objects
  // load osm binary
  volm_osm_objects osm_obj(osm_bin);
  std::cout << " open street map data are retrieved from: " << osm_bin << std::endl;
  std::cout << " start to ingest open street map data into x y z image... " << std::endl;
  std::cout << " there are " << osm_obj.num_regions() << " regions and " << osm_obj.num_roads() << " roads in current geo_cover image region" << std::endl;

  unsigned cnt = 0;
  // ingest regions
  unsigned num_regions = osm_obj.num_regions();
  for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
    vgl_polygon<double> poly(osm_obj.loc_polys()[r_idx]->poly()[0]);
    // get rid off polygon with duplicated points
    bool ignore = false;
    for (unsigned i = 0; i < poly[0].size()-1; i++) {
      if (poly[0][i] == poly[0][i+1])
        ignore = true;
    }
    if (ignore)
      continue;
    // check whether this region intersects with scene bbox
    if (!vgl_intersection(sbbox, poly))
      continue;

    unsigned char curr_level = osm_obj.loc_polys()[r_idx]->prop().level_;
    // geo cover is already level 0 and therefore anything in osm with level 0 is ignored
    if (curr_level == 0)
      continue;
    // go from geo coord wgs84 to local
    vgl_polygon<double> img_poly(1);
    unsigned char curr_id = osm_obj.loc_polys()[r_idx]->prop().id_;
    vil_rgb<vxl_byte> curr_color = osm_obj.loc_polys()[r_idx]->prop().color_;
    for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
      double lx, ly, lz;
      lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = (lx - scene_bbox.min_x())/vox_length;
      double j = (scene_bbox.max_y() - ly)/vox_length;
      img_poly[0].push_back(vgl_point_2d<double>(i,j));
    }

    // using poly iterator to loop over all points inside the polygon and intersect with the scene cropped image
    cnt++;
    vgl_polygon_scan_iterator<double> it(img_poly, true);
    for (it.reset(); it.next();  )
    {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if ( x >= 0 && y >= 0 && x < out_img_label->ni() && y < out_img_label->nj()) {
          if (curr_level > (*level_img)(x, y)) {
            (*level_img)(x,y) = curr_level;
            (*out_img_label)(x,y) = curr_id;
            (*out_class_img)(x,y) = curr_color;
          }
        }
      }
    }
  }
  std::cout << " number of osm regions ingested into geo cover: " << cnt << std::endl;
#if 0
    // go from geo coord wgs84 to local
    vgl_polygon<double> img_poly(1);
    unsigned char curr_level = osm_obj.loc_polys()[r_idx]->prop().level_;
    unsigned char curr_id = osm_obj.loc_polys()[r_idx]->prop().id_;
    vil_rgb<vxl_byte> curr_color = osm_obj.loc_polys()[r_idx]->prop().color_;
    for (unsigned pt_idx = 0; pt_idx < poly[0].size(); pt_idx++) {
      double lx, ly, lz;
      lvcs->global_to_local(poly[0][pt_idx].x(), poly[0][pt_idx].y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = (lx - scene_bbox.min_x())/vox_length;
      double j = (scene_bbox.max_y() - ly)/vox_length;
      if (i >= 0 && j >= 0 && i < out_img_label->ni() && j < out_img_label->nj())
        img_poly[0].push_back(vgl_point_2d<double>(i,j));
    }
    // update the label
    cnt++;
    vgl_polygon_scan_iterator<double> it(img_poly, true);
    for (it.reset(); it.next();  ) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if ( x >= 0 && y >= 0 && x < out_img_label->ni() && y < out_img_label->nj()) {
          if (curr_level > (*level_img)(x, y)) {
            (*level_img)(x,y) = curr_level;
            (*out_img_label)(x,y) = curr_id;
            (*out_class_img)(x,y) = curr_color;
          }
        }
      }
    }
  }
  std::cout << " number of osm regions ingested into geo cover: " << cnt << std::endl;
#endif
  // ingest roads
  cnt = 0;
  unsigned num_roads = osm_obj.num_roads();
  for (unsigned r_idx = 0; r_idx < num_roads; r_idx++) {
    std::vector<vgl_point_2d<double> > road = osm_obj.loc_lines()[r_idx]->line();
    std::vector<vgl_point_2d<double> > line_geo = vgl_intersection(road, sbbox);
    // check whether current road intersects with scene bounding box
    if (line_geo.size() < 2)
      continue;
    // go from geo coord wgs84 to scene local coo
    std::vector<vgl_point_2d<double> > line_img;
    unsigned char curr_level = osm_obj.loc_lines()[r_idx]->prop().level_;
    unsigned char curr_id = osm_obj.loc_lines()[r_idx]->prop().id_;
    vil_rgb<vxl_byte> curr_color = osm_obj.loc_lines()[r_idx]->prop().color_;
    double width = osm_obj.loc_lines()[r_idx]->prop().width_;
    for (auto & pt_idx : line_geo) {
      double lx, ly, lz;
      lvcs->global_to_local(pt_idx.x(), pt_idx.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
      double i = (lx - scene_bbox.min_x())/vox_length;
      double j = (scene_bbox.max_y() - ly)/vox_length;
      if (i >= 0 && j >= 0 && i < out_img_label->ni() && j < out_img_label->nj())
        line_img.emplace_back(i,j);
    }
    if (line_img.size() < 2)
      continue;
    // expend the line to polygon given certain width
    if (width < vox_length) width = vox_length;
    vgl_polygon<double> img_poly;
    if (!volm_io_tools::expend_line(line_img, width/vox_length, img_poly)) {
      std::cout << pro.name() << " expending osm line " << r_idx << " failed for width " << width << std::endl;
      return false;
    }
    // update the label
    cnt++;
    vgl_polygon_scan_iterator<double> it(img_poly, true);
    for (it.reset(); it.next();  ) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if ( x >= 0 && y >= 0 && x < out_img_label->ni() && y < out_img_label->nj()) {
          if (curr_level > (*level_img)(x, y)) {
            (*level_img)(x,y) = curr_level;
            (*out_img_label)(x,y) = curr_id;
            (*out_class_img)(x,y) = curr_color;
          }
        }
      }
    }
  }
  std::cout << " number of osm regions ingested into geo cover: " << cnt << std::endl;

#if 0 // use this when we have satellite data
  // ingest points (will occupy a single pixel)
  cnt = 0;
  unsigned n_pts = osm_obj.num_locs();
  std::vector<volm_osm_object_point_sptr> loc_pts = osm_obj.loc_pts();
  for (unsigned p_idx = 0; p_idx < n_pts; p_idx++) {
    vgl_point_2d<double> pt = loc_pts[p_idx]->loc();
    if (!sbbox.contains(pt))
      continue;
    // transfer from geo coord to img pixel
    unsigned char curr_level = loc_pts[p_idx]->prop().level_;
    unsigned char curr_id = loc_pts[p_idx]->prop().id_;
    vil_rgb<vxl_byte> curr_color = loc_pts[p_idx]->prop().color_;
    double lx, ly, lz;
    lvcs->global_to_local(pt.x(), pt.y(), 0.0, vpgl_lvcs::wgs84, lx, ly, lz);
    double i = (lx - scene_bbox.min_x())/vox_length;
    double j = (scene_bbox.max_y() - ly)/vox_length;
    int x = (int)i;
    int y = (int)j;
    if (x >= 0 && y >= 0 && x < out_img_label->ni() && y < out_img_label->nj()) {
      if (curr_level > (*level_img)(x, y)) {
        cnt++;
        (*level_img)(x,y) = curr_level;
        (*out_img_label)(x,y) = curr_id;
        (*out_class_img)(x,y) = curr_color;
      }
    }
  }
  std::cout << " number of osm points ingested into geo cover: " << cnt << std::endl;
#endif

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);
  pro.set_output_val<vil_image_view_base_sptr>(3, out_img_label);
  pro.set_output_val<vil_image_view_base_sptr>(4, out_class_img);

  return true;
}
