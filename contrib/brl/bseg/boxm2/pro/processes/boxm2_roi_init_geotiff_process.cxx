// This is brl/bseg/boxm2/pro/processes/boxm2_roi_init_geotiff_process.cxx
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to retrieve geocam from a geotiff image and lvcs from the scene
// Also creates a generic camera
// and crops the portion of the scene for the given scene and return it
//
// \author Ozge C. Ozcanli
// \date May 07, 2012

#include <boxm2/boxm2_scene.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/file_formats/vil_geotiff_header.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm2_roi_init_geotiff_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_roi_init_geotiff_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_roi_init_geotiff_process_globals;

  //process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";  // geocam
  input_types_[2] = "vcl_string";  // tiff image
  input_types_[3] = "unsigned";  // resolution level, if 0 orig img res, 1 => downsample by 2^1, 2 => by 2^2

  // process has 2 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";   // output generic cam
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_roi_init_geotiff_process(bprb_func_process& pro)
{
  using namespace boxm2_roi_init_geotiff_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(1);
  std::string geotiff_fname = pro.get_input<std::string>(2);
  auto level = pro.get_input<unsigned>(3);

  vil_image_resource_sptr img_res = vil_load_image_resource(geotiff_fname.c_str());
  vpgl_geo_camera* geocam = nullptr;
  if (cam) {
    std::cout << "Using the provided loaded camera.\n";
    geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  } else {
    std::cout << "Using the camera initialized from the image.\n";
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
    vpgl_geo_camera::init_geo_camera(img_res, lvcs, geocam);
  }

  if (!geocam) {
    std::cerr << "In boxm2_roi_init_geotiff_process() - the geocam could not be initialized!\n";
    return false;
  }

  double e1,n1,e2,n2,elev,lat,lon;
  scene->lvcs().get_origin(lat, lon, elev);
  geocam->img_four_corners_in_utm(img_res->ni(), img_res->nj(), elev, e1, n1, e2, n2);
  std::cout << "img res ni: " << img_res->ni() << " nj: " << img_res->nj() << std::endl;

  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  std::cout.setf(std::ios::fixed, std::ios::floatfield);

  // crop the image
  vgl_box_2d<double> proj_bbox;
  double u,v;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));
  int min_i = int(std::max(0.0, std::floor(proj_bbox.min_x())));
  int min_j = int(std::max(0.0, std::floor(proj_bbox.min_y())));
  int max_i = int(std::min(img_res->ni()-1.0, std::ceil(proj_bbox.max_x())));
  int max_j = int(std::min(img_res->nj()-1.0, std::ceil(proj_bbox.max_y())));
  int crop_ni = max_i - min_i + 1;
  int crop_nj = max_j - min_j + 1;
  if (crop_ni < 0 || crop_nj < 0) {
    std::cerr << "Error: boxm2_roi_init_geotiff_process: the image does not contain scene!\n"
             << "scene bbox:\n " << scene_bbox << '\n';
    double n,e; int utm_zone;
    std::cerr.setf(std::ios::fixed, std::ios::floatfield); std::cerr.precision(2);
    geocam->local_to_utm(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), e, n, utm_zone);
    std::cerr << "in UTM bbox transforms to " << n << " N " << e << " E and ";
    geocam->local_to_utm(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), e, n, utm_zone);
    std::cerr << n << " N " << e << " E\n";
    return false;
  }

  vil_image_view_base_sptr crop_base = img_res->get_view((unsigned int)min_i, crop_ni, (unsigned int)min_j, crop_nj);
  if (!crop_base) {
    std::cerr << "Error: boxm2_roi_init_geotiff_process: could not crop the scene in this image!\n";
    return false;
  }

  // downsample the image based on the level
  if (level < 32) {
    crop_ni /= (1L<<level);
    crop_nj /= (1L<<level);
  }
  else {
    crop_ni = (unsigned int) (crop_ni*std::pow(0.5,static_cast<double>(level)));
    crop_nj = (unsigned int) (crop_nj*std::pow(0.5,static_cast<double>(level)));
  }

  // now use this camera to generate generic cam
  vpgl_generic_camera<double> gcam;
  // make the geocam local
  geocam->translate(min_i, min_j, 0);

  vpgl_generic_camera_convert::convert(*geocam, crop_ni, crop_nj, scene_bbox.max_z()+1.0, gcam, level);

  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);

  auto* crop_view = dynamic_cast<vil_image_view<vxl_byte>*>(crop_base.ptr());
  if (!crop_view) {
    auto* crop_view_f = dynamic_cast<vil_image_view<float>*>(crop_base.ptr());
    if (!crop_view_f) {
      std::cerr << "Error: boxm2_roi_init_geotiff_process: could not cast first return image to a vil_image_view<float>\n";
      return false;
    }
    auto* out_img = new vil_image_view<float>(crop_ni, crop_nj, crop_view_f->nplanes());
    vil_resample_bilin(*crop_view_f, *out_img, crop_ni, crop_nj);
    pro.set_output_val<vil_image_view_base_sptr>(1,new vil_image_view<float>(*out_img));
    return true;
  }

  //: downsample the image based on the level
  auto* out_img = new vil_image_view<vxl_byte>(crop_ni, crop_nj, crop_view->nplanes());
  vil_resample_bilin(*crop_view, *out_img, crop_ni, crop_nj);
  pro.set_output_val<vil_image_view_base_sptr>(1,new vil_image_view<vxl_byte>(*out_img));

  return true;
}
