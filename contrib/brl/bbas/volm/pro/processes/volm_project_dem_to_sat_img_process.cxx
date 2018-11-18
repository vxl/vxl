// This is brl/bbas/volm/processes/vlm_project_dem_to_sat_img_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Project and crop the ASTER DEM orthogonal images to a given satellite viewpoint
//
// \author Yi Dong
// \date April 23, 2014
// \verbatim
//  Modifications
//   Yi Dong May 1, 2014  -- add a bounding box of satellite viewpoint to speed up the process execution
//   Ozge C. Ozcanli June 05, 2014 -- adding an option to use an ortho camera instead of a rational camera for the satellite view
//                                    DEMs or height maps can now be projected onto orthorectified satellite images or other types of ortho images
//                                    input cam should be vpgl_geo_camera
// \endverbatim
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_int_2.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_intersection.h>

// process to project/crop single ASTER DEM image to the given satellite viewpoint
//: global variables and functions
namespace volm_project_dem_to_sat_img_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;

  // function to project dem image pixel to satellite image pixel
  bool project_dem_to_sat(vpgl_geo_camera const& dem_cam, vpgl_rational_camera<double>* sat_cam,
                          int const& dem_i,  int const& dem_j,  float const& dem_height,
                          int const& sat_ni, int const& sat_nj,
                          int& sat_img_i,    int& sat_img_j);

  // function to project dem image pixel to ortho image pixel
  bool project_dem_to_ortho(vpgl_geo_camera const& dem_cam, vpgl_geo_camera* img_cam,
                            int const& dem_i,  int const& dem_j,  float const& dem_height,
                            int const& img_ni, int const& img_nj,
                            int& img_i,    int& img_j);
}
//: constructor
bool volm_project_dem_to_sat_img_process_cons(bprb_func_process& pro)
{
  using namespace volm_project_dem_to_sat_img_process_globals;
  // process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // satellite viewpoint (rational camera)
  input_types_[1] = "vil_image_view_base_sptr"; // satellite image (will be overwritten)
  input_types_[2] = "vcl_string";               // ASTER DEM image name
  input_types_[3] = "vpgl_camera_double_sptr";  // geocam if DEM image does not contain a camera.  Pass 0 means the camera will be loaded from geotiff header
  input_types_[4] = "double";                   // lower left lon of satellite region
  input_types_[5] = "double";                   // lower left lat of satellite region
  input_types_[6] = "double";                   // upper right lon of satellite region
  input_types_[7] = "double";                   // upper right lat of satellite region
  // process takes 0 input
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool volm_project_dem_to_sat_img_process(bprb_func_process& pro)
{
  using namespace volm_project_dem_to_sat_img_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": there should be " << n_inputs_ << " inputs" << std::endl;
    return false;
  }
  // get the input
  unsigned in_i = 0;
  vpgl_camera_double_sptr  sat_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr sat_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  std::string dem_file = pro.get_input<std::string>(in_i++);
  vpgl_camera_double_sptr  dem_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto lower_left_lon = pro.get_input<double>(in_i++);
  auto lower_left_lat = pro.get_input<double>(in_i++);
  auto upper_right_lon = pro.get_input<double>(in_i++);
  auto upper_right_lat = pro.get_input<double>(in_i++);

  auto* sat_img = dynamic_cast<vil_image_view<float>*>(sat_img_sptr.ptr());
  if (!sat_img) {
    std::cout << pro.name() << ": The image pixel format, " << sat_img_sptr->pixel_format() << " is not supported!" << std::endl;
    return false;
  }

  // load the dem image and dem camera
  if (!vul_file::exists(dem_file)) {
    std::cout << pro.name() << ": can not find dem image file: " << dem_file << std::endl;
    return false;
  }
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vpgl_geo_camera* dem_cam = nullptr;
  if (dem_cam_sptr) {
    std::cout << "Using the input geo camera for dem image!\n";
    dem_cam = dynamic_cast<vpgl_geo_camera*>(dem_cam_sptr.ptr());
  }
  else {
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs();  // create a empty lvcs for dem_cam
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, dem_cam);
  }
  if (!dem_cam) {
    std::cout << pro.name() << ": the geocam of dem image can not be initialized" << std::endl;
    return false;
  }

  int dem_ni, dem_nj, sat_ni, sat_nj;
  dem_ni = dem_res->ni();  dem_nj = dem_res->nj();
  sat_ni = sat_img->ni();  sat_nj = sat_img->nj();
  // if satellite region is given then check whether ortho image intersects with the satellite region to speed up
  if (lower_left_lon > 0) {
    vgl_box_2d<double> sat_bbox(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
    // obtain the bounding box of ortho image
    double lat, lon;
    dem_cam->img_to_global(0.0, dem_nj-1, lon, lat);
    vgl_point_2d<double> dem_lower_left(lon, lat);
    dem_cam->img_to_global(dem_ni-1, 0.0, lon, lat);
    vgl_point_2d<double> dem_upper_right(lon, lat);
    vgl_box_2d<double> dem_bbox(dem_lower_left, dem_upper_right);
    vgl_box_2d<double> intersection_box = vgl_intersection(sat_bbox, dem_bbox);
    if (intersection_box.is_empty()) {
      std::cout << pro.name() << ": The input dem image: " << dem_bbox
               << " doesn't overlap with satellite region: " << sat_bbox
               << ", IGNORE" << std::endl;
      return false;
    }
    else {
      std::cout << " The dem image: " << dem_bbox << " intersects with satellite region: " << sat_bbox << std::endl;
    }
  }

  // load the dem image view
  vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, dem_ni, 0, dem_nj);
  auto* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);
    auto* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      auto* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        std::cout << pro.name() << ": The dem image pixel format, " << dem_view_base->pixel_format() << " is not supported!" << std::endl;
        return false;
      }
      else
        vil_convert_cast(*dem_view_byte, temp);
    }
    else
      vil_convert_cast(*dem_view_int, temp);
    dem_view = new vil_image_view<float>(temp);
  }

  // note the sat_cam can either be rational_camera or local_rational_camera
  bool rational_cam = true;
  auto* sat_cam = dynamic_cast<vpgl_rational_camera<double>*>(sat_cam_sptr.as_pointer());
  vpgl_geo_camera* geo_cam;
  if (!sat_cam) {
    std::cout << pro.name() << ": the input camera is not a rational camera" << std::endl;
    rational_cam = false;

    geo_cam = dynamic_cast<vpgl_geo_camera*>(sat_cam_sptr.as_pointer());
    if (!geo_cam) {
      std::cerr << pro.name() << ": the input camera is neither a rational camera nor a geo camera!" << std::endl;
      return false;
    }
  }

  // start projection
  std::cout << "Projecting [" << dem_ni << 'x' << dem_nj << "] dem image to satellite image having size [" << sat_ni << 'x' << sat_nj << "]\n";
  //std::vector<vnl_int_2> valid_pixels;
  unsigned cnt = 0;
  if (rational_cam) {
    for (int i = 0; i < dem_ni; i++) {
      if (i%200==0) std::cout << "." << i << std::flush;
      for (int j = 0; j < dem_nj; j++) {
        int sat_img_i, sat_img_j;
        if (project_dem_to_sat(*dem_cam, sat_cam, i, j, (*dem_view)(i,j), sat_ni, sat_nj, sat_img_i, sat_img_j)) {
          (*sat_img)(sat_img_i, sat_img_j) = (*dem_view)(i,j);
          cnt++;
        }
      }
    }
  } else {  // using ortho cam
    for (int i = 0; i < dem_ni; i++) {
      if (i%200==0) std::cout << "." << i << std::flush;
      for (int j = 0; j < dem_nj; j++) {
        int sat_img_i, sat_img_j;
        if (project_dem_to_ortho(*dem_cam, geo_cam, i, j, (*dem_view)(i,j), sat_ni, sat_nj, sat_img_i, sat_img_j)) {
          (*sat_img)(sat_img_i, sat_img_j) = (*dem_view)(i,j);
          cnt++;
        }
      }
    }
  }

  std::cout << '\n' << cnt << " pixels in dem image are projected onto satellite image\n";
  return true;
}

bool volm_project_dem_to_sat_img_process_globals::project_dem_to_sat(vpgl_geo_camera const& dem_cam, vpgl_rational_camera<double>* sat_cam,
                                                                     int const& dem_i,  int const& dem_j,  float const& dem_height,
                                                                     int const& sat_ni, int const& sat_nj,
                                                                     int& sat_img_i,    int& sat_img_j)
{
  // obtain global coords from dem pixel
  double lon, lat;
  double iu, iv;
  int uu, vv;
  dem_cam.img_to_global(dem_i, dem_j, lon, lat);
  // project global coords to satellite image
  if (sat_cam->type_name() == "vpgl_local_rational_camera") {
    auto* local_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(sat_cam);
    double loc_x, loc_y, loc_z;
    local_cam->lvcs().global_to_local(lon, lat, dem_height, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
    local_cam->project(loc_x, loc_y, loc_z, iu, iv);
  }
  else
    sat_cam->project(lon, lat, dem_height, iu, iv);

  uu = (int)std::floor(iu+0.5);
  vv = (int)std::floor(iv+0.5);

  if (uu >= 0 && vv >= 0 && uu < sat_ni && vv < sat_nj) {
    sat_img_i = uu;  sat_img_j = vv;
#if 0
    std::cout << " dem_img_pixel: [" << dem_i << ',' << dem_j
             << "] --> [" << lon << ", " << lat << ", " << dem_height
             << "] --> [" << sat_img_i << "," << sat_img_j << "]" << std::endl;
#endif
    return true;
  }
  else
    return false;
}


// function to project dem image pixel to ortho image pixel
bool volm_project_dem_to_sat_img_process_globals::project_dem_to_ortho(vpgl_geo_camera const& dem_cam, vpgl_geo_camera* img_cam,
                            int const& dem_i,  int const& dem_j,  float const& dem_height,
                            int const& img_ni, int const& img_nj,
                            int& img_i,    int& img_j)
{
  // obtain global coords from dem pixel
  double lon, lat;
  double iu, iv;
  int uu, vv;
  dem_cam.img_to_global(dem_i, dem_j, lon, lat);
  // project global coords to satellite image
  img_cam->global_to_img(lon, lat, dem_height, iu, iv);   // actually the dem_height does not matter in the ortho case

  uu = (int)std::floor(iu+0.5);
  vv = (int)std::floor(iv+0.5);

  if (uu >= 0 && vv >= 0 && uu < img_ni && vv < img_nj) {
    img_i = uu;  img_j = vv;
#if 0
    std::cout << " dem_img_pixel: [" << dem_i << ',' << dem_j
             << "] --> [" << lon << ", " << lat << ", " << dem_height
             << "] --> [" << img_i << "," << img_j << "]" << std::endl;
#endif
    return true;
  }
  else
    return false;
}
