// This is brl/bbas/volm/volm_pro/processes/vpgl_project_dem_to_sat_res.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Project and crop the ASTER DEM orthogonal images to a given satellite viewpoint
//
// \author Yi Dong
// \date April 23, 2014
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vcl_iostream.h>
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

// process to project/crop single ASTER DEM image to the given satellite viewpoint
//: global variables and functions
namespace volm_project_dem_to_sat_img_process_globals
{
  const unsigned n_inputs_  = 4;
  const unsigned n_outputs_ = 0;

  // function to project dem image pixel to satellite image pixel
  bool project_dem_to_sat(vpgl_geo_camera const& dem_cam, vpgl_rational_camera<double>* sat_cam, 
                          int const& dem_i,  int const& dem_j,  float const& dem_height,
                          int const& sat_ni, int const& sat_nj,
                          int& sat_img_i,    int& sat_img_j);
}
//: constructor
bool volm_project_dem_to_sat_img_process_cons(bprb_func_process& pro)
{
  using namespace volm_project_dem_to_sat_img_process_globals;
  // process takes 4 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // satellite viewpoint (rational camera)
  input_types_[1] = "vil_image_view_base_sptr"; // satellite image (will be overwritten)
  input_types_[2] = "vcl_string";               // ASTER DEM image name
  input_types_[3] = "vpgl_camera_double_sptr";  // geocam if DEM image does not contain a camera.  Pass 0 means the camera will be loaded from geotiff header
  // process takes 0 input
  vcl_vector<vcl_string> output_types_(n_outputs_); 
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool volm_project_dem_to_sat_img_process(bprb_func_process& pro)
{
  using namespace volm_project_dem_to_sat_img_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    vcl_cout << pro.name() << ": there should be " << n_inputs_ << " inputs" << vcl_endl;
    return false;
  }
  // get the input
  unsigned in_i = 0;
  vpgl_camera_double_sptr  sat_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr sat_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vcl_string dem_file = pro.get_input<vcl_string>(in_i++);
  vpgl_camera_double_sptr  dem_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  
  vil_image_view<float>* sat_img = dynamic_cast<vil_image_view<float>*>(sat_img_sptr.ptr());
  if (!sat_img) {
    vcl_cout << pro.name() << ": The image pixel format, " << sat_img_sptr->pixel_format() << " is not supported!" << vcl_endl;
    return false;
  }
  // note the sat_cam can either be rational_camera or local_rational_camera
  vpgl_rational_camera<double>* sat_cam = dynamic_cast<vpgl_rational_camera<double>*>(sat_cam_sptr.as_pointer());
  if (!sat_cam) {
    vcl_cerr << pro.name() << ": the input camera is not a rational camera" << vcl_endl;
    return false;
  }

  // load the dem image
  if (!vul_file::exists(dem_file)) {
    vcl_cout << pro.name() << ": can not find dem image file: " << dem_file << vcl_endl;
    return false;
  }
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vpgl_geo_camera* dem_cam = 0;
  if (dem_cam_sptr) {
    vcl_cout << "Using the input geo camera for dem image!\n";
    dem_cam = dynamic_cast<vpgl_geo_camera*>(dem_cam_sptr.ptr());
  }
  else {
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs();  // create a empty lvcs for dem_cam
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, dem_cam);
  }
  if (!dem_cam) {
    vcl_cout << pro.name() << ": the geocam of dem image can not be initialized" << vcl_endl;
    return false;
  }

  int dem_ni, dem_nj, sat_ni, sat_nj;
  dem_ni = dem_res->ni();  dem_nj = dem_res->nj();
  sat_ni = sat_img->ni();  sat_nj = sat_img->nj();
  // load the dem image view
  vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, dem_ni, 0, dem_nj);
  vil_image_view<float>* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);
    vil_image_view<vxl_int_16>* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      vil_image_view<vxl_byte>* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        vcl_cout << pro.name() << ": The dem image pixel format, " << dem_view_base->pixel_format() << " is not supported!" << vcl_endl;
        return false;
      }
      else
        vil_convert_cast(*dem_view_byte, temp);
    }
    else
      vil_convert_cast(*dem_view_int, temp);
    dem_view = new vil_image_view<float>(temp);
  }

  // start projection
  vcl_cout << "Projecting [" << dem_ni << 'x' << dem_nj << "] dem image to satellite image having size [" << sat_ni << 'x' << sat_nj << "]\n";
  //vcl_vector<vnl_int_2> valid_pixels;
  unsigned cnt = 0;
  for (int i = 0; i < dem_ni; i++) {
    if (i%200==0) vcl_cout << "." << i << vcl_flush;
    for (int j = 0; j < dem_nj; j++) {
      int sat_img_i, sat_img_j;
      if (project_dem_to_sat(*dem_cam, sat_cam, i, j, (*dem_view)(i,j), sat_ni, sat_nj, sat_img_i, sat_img_j)) {
        (*sat_img)(sat_img_i, sat_img_j) = (*dem_view)(i,j);
        cnt++;
      }
    }
  }
  vcl_cout << '\n' << cnt << " pixels in dem image are projected onto satellite image\n";
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
    vpgl_local_rational_camera<double>* local_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(sat_cam);
    double loc_x, loc_y, loc_z;
    local_cam->lvcs().global_to_local(lon, lat, dem_height, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
    local_cam->project(loc_x, loc_y, loc_z, iu, iv);
  }
  else
    sat_cam->project(lon, lat, dem_height, iu, iv);

  uu = (int)vcl_floor(iu+0.5);
  vv = (int)vcl_floor(iv+0.5);

  if (uu >= 0 && vv >= 0 && uu < sat_ni && vv < sat_nj) {
    sat_img_i = uu;  sat_img_j = vv;
#if 0
    vcl_cout << " dem_img_pixel: [" << dem_i << ',' << dem_j
             << "] --> [" << lon << ", " << lat << ", " << dem_height
             << "] --> [" << sat_img_i << "," << sat_img_j << "]" << vcl_endl;
#endif
    return true;
  }
  else
    return false;
}