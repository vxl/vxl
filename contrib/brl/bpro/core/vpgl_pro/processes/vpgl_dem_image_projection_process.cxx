// This is brl/bpro/core/vpgl_pro/processes/vpgl_dem_image_projection_process.cxx
#include <iostream>
#include <limits>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include "../vpgl_dem_manager.h"

//: initialization
bool vpgl_dem_image_projection_process_cons(bprb_func_process& pro)
{
  // this process takes 6 inputs
  std::vector<std::string> input_types_(9);
  input_types_[0] = "vil_image_view_base_sptr";   // input reference image
  input_types_[1] = "vpgl_camera_double_sptr";    // input reference camera
  input_types_[2] = "vpgl_dem_manager_sptr";      // DEM manager
  input_types_[3] = "vpgl_camera_double_sptr";    // input target camera
  input_types_[4] = "unsigned";                   // output image pixel (0,0) relative to target image domain (i0, j0)
  input_types_[5] = "unsigned";                   // output image pixel (0,0) relative to target image domain (i0, j0)
  input_types_[6] = "unsigned";                   // output image size -- col
  input_types_[7] = "unsigned";                   // output image size -- row
  input_types_[8] = "double";                     // back-projection error tolerance
  // this process takes 2 output
  std::vector<std::string> output_types_(2);
  output_types_[0] = "vil_image_view_base_sptr";  // output image
  output_types_[1] = "unsigned";                  // number of failed pixels
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool vpgl_dem_image_projection_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << " -- Wrong inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr ref_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  ref_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vpgl_dem_manager_sptr    dem_mgr = pro.get_input<vpgl_dem_manager_sptr>(in_i++);
  vpgl_camera_double_sptr  tgr_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto i0     = pro.get_input<unsigned>(in_i++);
  auto j0     = pro.get_input<unsigned>(in_i++);
  auto out_ni = pro.get_input<unsigned>(in_i++);
  auto out_nj = pro.get_input<unsigned>(in_i++);
  auto err_tol  = pro.get_input<double>(in_i++);
  // convert all input images/cameras
  if (ref_img_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cerr << pro.name() << " -- unsupported input image pixel format: " << ref_img_sptr->pixel_format()
              << ", only float is supported for now!!!\n";
    return false;
  }
  vil_image_view<float> ref_img(ref_img_sptr);
  auto* ref_cam = dynamic_cast<vpgl_rational_camera<double>*>(ref_cam_sptr.as_pointer());
  if (!ref_cam) {
    std::cerr << pro.name() << " -- failed to load satellite rational camera!!\n";
    return false;
  }
  if(!dem_mgr || !tgr_cam_sptr){
   std::cerr << pro.name() << " -- Null input dem_mgr or target camera" << std::endl;
    return false;
  }

  // generate output image
  vil_image_view<float> out_img(out_ni, out_nj);
  out_img.fill(-1*std::numeric_limits<float>::max());
  if (err_tol <= 0)
    err_tol = 1.0;
  // start backprojection/projection
  unsigned cnt = 0;
  unsigned err_cnt = 0;
  int ref_ni = (int)ref_img.ni();
  int ref_nj = (int)ref_img.nj();
  std::cout << "start projection for image (" << out_ni << ", " << out_nj << ")" << std::endl;
  for (unsigned i = 0; i < out_ni; i++)
  {
    double tgr_i = i0 + i;
    if (i%100 == 0)
      std::cout << "." << i << "(" << tgr_i << ")" << std::flush;
    for (unsigned j = 0; j < out_nj; j++)
    {
      // back-projection from target image (i, j) to WGS84 (lon, lat, elev)
      double lon, lat, elev;
      double tgr_j = j0 + j;
      if (!dem_mgr->back_project(tgr_cam_sptr.ptr(), tgr_i, tgr_j, lon, lat, elev, err_tol)) {
        err_cnt++;
        continue;
      }
      // project WGS84 (lon, lat, elev) to reference image (u, v)
      double uu, vv;
      if (ref_cam->type_name() == "vpgl_local_rational_camera") {
        auto* local_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(ref_cam);
        double loc_x, loc_y, loc_z;
        local_cam->lvcs().global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
        local_cam->project(loc_x, loc_y, loc_z, uu, vv);
      }
      else
        ref_cam->project(lon, lat, elev, uu, vv);
      int u = (int)std::floor(uu + 0.5f);
      int v = (int)std::floor(vv + 0.5f);
      if ( u >= 0 && v >= 0 && u < ref_ni && v < ref_nj)
        out_img(i,j) = ref_img(u, v);
      else
        err_cnt++;
    }
  }
  std::cout << "\n";
  // output
  vil_image_view_base_sptr out_img_sptr = new vil_image_view<float>(out_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  pro.set_output_val<unsigned>(1, err_cnt);
  return true;
}
