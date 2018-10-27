// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_geotiff_to_point_cloud_process.cxx
#include <iostream>
#include <vil/vil_config.h>
#if HAS_GEOTIFF
#include <bprb/bprb_func_process.h>
//:
// \file
//   Process to convert WGS84 based geotiff to a ASCII xyz point cloud given a WGS84 based LVCS conversion
#include <vgl/vgl_point_3d.h>
#include <bprb/bprb_parameters.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

// Initialization
bool vpgl_convert_geotiff_to_point_cloud_process_cons(bprb_func_process& pro)
{
  // This process takes 4 inputs
  std::vector<std::string> input_types_(5);
  input_types_[0] = "vil_image_view_base_sptr";  // input float image
  input_types_[1] = "vpgl_camera_double_sptr";   // input geo camera
  input_types_[2] = "vpgl_lvcs_sptr";            // input lvcs
  input_types_[3] = "vcl_string";                // output xyz filename
  input_types_[4] = "bool";                      // option to choose whether convert z value
  // process takes 1 outputs
  std::vector<std::string> output_types_(1);
  output_types_[0] = "unsigned";                 // number of converted pixels
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// execute
bool vpgl_convert_geotiff_to_point_cloud_process(bprb_func_process& pro)
{
  // verify inputs
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Missing Inputs!!!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vpgl_lvcs_sptr           lvcs_sptr   = pro.get_input<vpgl_lvcs_sptr>(in_i++);
  std::string out_filename = pro.get_input<std::string>(in_i++);
  bool is_convert_z = pro.get_input<bool>(in_i++);

  // check and covert inputs
  auto* in_img = dynamic_cast<vil_image_view<float>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported input image pixel type: " << in_img_sptr->pixel_format() << ", only Float is supported!\n";
    return false;
  }
  auto* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    std::cerr << pro.name() << ": failed to load input geo-camera!\n";
    return false;
  }

  // perform conversion
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  std::vector<vgl_point_3d<double> > pts;
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      // convert pixel (i,j) to (lat, lon)
      double elev = (*in_img)(i,j);
      double lon, lat;
      in_cam->img_to_global(i, j, lon, lat);
      // convert (lon, lat, elev) to local coord (x, y, z)
      double x, y, z;
      lvcs_sptr->global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, x, y, z, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      vgl_point_3d<double>pt(x, y, z);
      if (!is_convert_z)
        pt.set(x, y, elev);
      pts.push_back(pt);
    }
  }
  // write to file
  std::ofstream ofs(out_filename.c_str());
  if (!ofs.is_open()) {
    std::cerr << "Failed to open file " << out_filename.c_str() << std::endl;
    return false;
  }
  for (auto & pt : pts) {
    ofs << pt.x() << ' ' << pt.y() << ' ' << pt.z() << '\n';
  }

  ofs.close();

  // output
  pro.set_output_val<unsigned>(0, pts.size());
  return true;
}


#endif // HAS_GEOTIFF
