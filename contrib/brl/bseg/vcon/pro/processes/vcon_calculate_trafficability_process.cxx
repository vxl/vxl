// This is brl/bseg/vcon/processes/vcon_calculate_trafficability_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Generate trafficability
//
// \author Aaron Gokaslan
// \date April 23, 2014
// \verbatim
//  Modifications
//
// \endverbatim
//

#include <vcl_compiler.h>
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
#include <vgl/vgl_line_segment_2d.h>
#include <bbas_pro/bbas_1d_array_float.h>
#include <vgl/vgl_vector_2d.h>
#include <assert.h>
#include <vector>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>


// process to project/crop single ASTER DEM image to the given satellite viewpoint
//: global variables and functions
namespace vcon_calculate_trafficability_process_globals
{
  const unsigned n_inputs_  = 4;
  const unsigned n_outputs_ = 2;

}
//: constructor
bool vcon_calculate_trafficability_process_cons(bprb_func_process& pro)
{
  using namespace vcon_calculate_trafficability_process_globals;
  // process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";               // ASTER DEM image name
  //input_types_[1] = "vpgl_camera_double_sptr";  // geocam if DEM image does not contain a camera.  Pass 0 means the camera will be loaded from geotiff header
  input_types_[1] = "bbas_1d_array_float_sptr"; //The X value
  input_types_[2] = "bbas_1d_array_float_sptr"; //The Y value
  input_types_[3] = "double"; //The width of the road
  // process takes 0 input
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";
  output_types_[1] = "int";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool vcon_calculate_trafficability_process(bprb_func_process& pro)
{
  using namespace vcon_calculate_trafficability_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": there should be " << n_inputs_ << " inputs" << std::endl;
    return false;
  }
  // get the input
  unsigned in_i = 0;
  std::string dem_file = pro.get_input<std::string>(in_i++);
  //vpgl_camera_double_sptr  dem_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  bbas_1d_array_float x_coords = *pro.get_input<bbas_1d_array_float_sptr>(in_i++);
  bbas_1d_array_float y_coords = *pro.get_input<bbas_1d_array_float_sptr>(in_i++);
  
  if (x_coords.data_array.size() != y_coords.data_array.size()){
      std::cout << pro.name() << ": there should be the same number of x values as y values" << std::endl;
      return false;
  }
  // load the dem image and dem camera
  if (!vul_file::exists(dem_file)) {
    std::cout << pro.name() << ": can not find dem image file: " << dem_file << std::endl;
    return false;
  }
  std::cout << "loading image: " << dem_file << std::endl;
  vil_image_resource_sptr dem_res = vil_load_image_resource(dem_file.c_str());
  vpgl_geo_camera* dem_cam = VXL_NULLPTR;
  /*if (dem_cam_sptr) {
    std::cout << "Using the input geo camera for dem image!\n";
    dem_cam = dynamic_cast<vpgl_geo_camera*>(dem_cam_sptr.ptr());
  }
  else {*/
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs();  // create a empty lvcs for dem_cam
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, dem_cam);
  //}
  if (!dem_cam) {
    std::cout << pro.name() << ": the geocam of dem image can not be initialized" << std::endl;
    return false;
  }
  
  std::cout << "geotiff camera matrix" << std::flush << std::endl;
  std::cout << dem_cam->trans_matrix() << std::endl;
  
  
  int dem_ni, dem_nj;
  dem_ni = dem_res->ni();  dem_nj = dem_res->nj();

  // load the dem image view
  vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, dem_ni, 0, dem_nj);
  vil_image_view<float>* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);
    vil_image_view<vxl_int_16>* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      vil_image_view<vxl_byte>* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        std::cout << pro.name() << ": The dem image pixel format, " << dem_view_base->pixel_format() << " is not supported!" << std::endl;
        return false;
      }
      else
        vil_convert_cast(*dem_view_byte, temp);
    }
    else
      vil_convert_cast(*dem_view_int, temp);
      dem_view = new vil_image_view<float>(temp); //TODO Investigate possible memory leak
  }
  
  std::cout << "image loading done -- img size: " << dem_view->ni() << ',' << dem_view->nj() << std::endl;
  
  // note the sat_cam can either be rational_camera or local_rational_camera
  bool rational_cam = true;
   
  double road_width = pro.get_input<double>(in_i++);// The road width We may need to feed this into the the function at a later time.
  
  
  std::vector<vgl_point_2d<float> > polygon_vec(x_coords.data_array.size() * 2);
  //TODO Move this into a function
  for(int i = 0, poly_size = x_coords.data_array.size() * 2 - 1; i + 1 < x_coords.data_array.size(); i++){ 
    vgl_point_2d<float> p_1 = vgl_point_2d<float>(x_coords.data_array[i], y_coords.data_array[i]); //Gets the first point of the road
    vgl_point_2d<float> p_2 = vgl_point_2d<float>(x_coords.data_array[i + 1], y_coords.data_array[i + 1]); //Gets the next point of the road
    vgl_line_segment_2d<float> road_center = vgl_line_segment_2d<float>(p_1, p_2); 
    vgl_vector_2d<float> road_normal = road_center.normal() * road_width; //This is actually twice the road width
    polygon_vec[i] = p_1 + road_normal;
    //std::cout << p_1 + road_normal;
    polygon_vec[poly_size - i] = p_1 - road_normal;
    if(i + 2 == x_coords.data_array.size()){ //Optimize later
      i++; //Finish it off later
      polygon_vec[i] = p_2 + road_normal;
      polygon_vec[poly_size - i] = p_2 - road_normal;
    }
  }
   
  vgl_polygon<float> polygon_obj = vgl_polygon<float>(polygon_vec);
  
  //std::cout << polygon_obj;
  
  vgl_polygon_scan_iterator<float> it(polygon_obj);
  
  /*std::cout << "BEGINNING THE FRAY! Max values " << std::endl;
  std::cout << dem_ni;
  std::cout << " ";
  std::cout << dem_nj;
  */  
  float roughness = 0;
  int pixel_num = 0;
  //Fetches the polygon iterator
  for (it.reset(); it.next();) {
    int y = it.scany();
    
    if(y < 0){
      continue;
    }
    if(y >= dem_nj){
      break;  
    }
    
    int start_x = it.startx();
    start_x = start_x < 0 ? 0 : start_x; 
    int end_x = it.endx();
    //std::cout << y;
    for(int x = start_x; x < end_x && x < dem_nj; x++){
      if(x < 0 || x >= dem_ni || y < 0 || y >= dem_nj){
	//std::cout << "FAIL x: " << x << "y: " << y << std::endl;
      } else {
	assert(x < dem_ni);
	assert(y < dem_nj);
	//std::cout << "x: " << x << "y: " << y;
	roughness += (*dem_view)(x, y);
	pixel_num++;
      }
    }
    //pixel_num += end_x - start_x;
  }
  if(pixel_num == 0){
    pixel_num = 1;
  }
  //std::cout << "roughness " << roughness << std::endl;
  //std::cout << "pixel_num " << pixel_num << std::endl;
  //double output = roughness/(double)pixel_num;
  //std::cout << "Output is " << output << std::endl;
  pro.set_output_val<double>(0, roughness);
  pro.set_output_val<int>(1, pixel_num);
  //std::cout << "MISSION COMPLETE" << std::endl;
  
  return true;
}