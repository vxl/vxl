// This is brl/bpro/core/vpgl_pro/processes/vpgl_geo_camera_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <bkml/bkml_write.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>

//: initialization
bool vpgl_load_geo_camera_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs and one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vpgl_lvcs_sptr");
  input_types.push_back("int");  // UTM zone, pass 0 if not UTM
  input_types.push_back("unsigned");  // UTM hemisphere, pass 0 for north, 1 for south
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  //camera output

  bool good = pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
  
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;  // initialize lvcs to empty
  pro.set_input(1, new brdb_value_t<vpgl_lvcs_sptr>(lvcs));

  return good;
}

//: Execute the process
bool vpgl_load_geo_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 4) {
    vcl_cout << "vpgl_load_geo_camera_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string tfw_filename = pro.get_input<vcl_string>(0);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(1);
  int utm_zone = pro.get_input<int>(2);
  unsigned northing = pro.get_input<unsigned>(3);

  vcl_ifstream ifs(tfw_filename.c_str());
  if(!ifs)
  {
      vcl_cout<<"Could not open file "<<vcl_endl;
      return false;
  }
  vnl_matrix<double> trans_matrix(4,4,0.0);
  ifs >> trans_matrix[0][0];
  ifs >> trans_matrix[0][1];
  ifs >> trans_matrix[1][0];
  ifs >> trans_matrix[1][1];
  ifs >> trans_matrix[0][3];
  ifs >> trans_matrix[1][3];
  trans_matrix[3][3] = 1.0;
  ifs.close();

  vcl_cout << "trans matrix: " << trans_matrix << vcl_endl;

  vpgl_geo_camera *cam = new vpgl_geo_camera(trans_matrix, lvcs);
  if (utm_zone != 0)
    cam->set_utm(utm_zone, northing);
  cam->set_scale_format(true);

  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}


//: initialization
bool vpgl_translate_geo_camera_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.push_back("double");
  input_types.push_back("double");
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  //camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_translate_geo_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    vcl_cout << "vpgl_translate_geo_camera_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  double tx = pro.get_input<double>(1);
  double ty = pro.get_input<double>(2);
  vpgl_geo_camera* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  vpgl_geo_camera* outcam = new vpgl_geo_camera(*geocam);
  outcam->translate(tx, ty, 0.0);
  pro.set_output_val<vpgl_camera_double_sptr>(0, outcam);
  return true;
}

//: initialization
bool vpgl_convert_geo_camera_to_generic_process_cons(bprb_func_process& pro)
{
  //this process takes 5 inputs and one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.push_back("int");
  input_types.push_back("int");
  input_types.push_back("double");
  input_types.push_back("int");
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // generic camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_geo_camera_to_generic_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 5) {
    vcl_cout << "vpgl_convert_geo_camera_to_generic_process: The number of inputs should be 5" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  int ni = pro.get_input<int>(1);
  int nj = pro.get_input<int>(2);
  double scene_height = pro.get_input<double>(3);
  int level = pro.get_input<int>(4);
  vpgl_geo_camera* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  vpgl_generic_camera<double> gcam;
  vpgl_generic_camera_convert::convert(*geocam, ni, nj, scene_height, gcam, level);

  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  return true;
}

//: initialization
bool vpgl_geo_footprint_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");  // as read from a geotiff image header or from a tfw file
  input_types.push_back("vcl_string");               // geotiff file, to determine image size
  input_types.push_back("vcl_string");               // the filename of the kml file to write footprints to
  input_types.push_back("bool");                     // put the tags do initialize and end the document, if writing only one file these are needed, otherwise add manually to the file
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_geo_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 4) {
    vcl_cout << "vpgl_geo_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string geotiff_filename = pro.get_input<vcl_string>(1);
  vil_image_resource_sptr img_res = vil_load_image_resource(geotiff_filename.c_str());
  vcl_string footprint_filename = pro.get_input<vcl_string>(2);
  bool init_finish = pro.get_input<bool>(3);
  unsigned ni = img_res->ni(); unsigned nj = img_res->nj();

  vcl_ofstream ofs(footprint_filename.c_str(), vcl_ios::app);
  if (!ofs.good()) {
    vcl_cerr << "ERROR: vpgl_geo_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  if (init_finish)
    bkml_write::open_document(ofs);

  vpgl_geo_camera* geocam=dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  if (!geocam) {
    vcl_cerr << "In vpgl_geo_footprint_process() - input camera is not valid!\n";
    return false;
  }

  double lon, lat;
  geocam->img_to_global(0,0,lon,lat);
  vnl_double_2 ul; ul[0] = lat; ul[1] = lon;

  geocam->img_to_global(0,nj,lon,lat);
  vnl_double_2 ur; ur[0] = lat; ur[1] = lon;

  geocam->img_to_global(ni,0,lon,lat);
  vnl_double_2 ll; ll[0] = lat; ll[1] = lon;

  geocam->img_to_global(ni,nj,lon,lat);
  vnl_double_2 lr; lr[0] = lat; lr[1] = lon;

  vcl_string g_id = vul_file::strip_directory(geotiff_filename);
  vcl_string desc = geotiff_filename + " footprint";

  bkml_write::write_box(ofs, g_id, desc, ul, ur, ll, lr);

  if (init_finish)
    bkml_write::close_document(ofs);

  return true;
}


bool vpgl_geo_cam_global_to_img_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and one output
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.push_back("double"); // lon
  input_types.push_back("double"); // lat
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");  // i
  output_types.push_back("int");  // j
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_geo_cam_global_to_img_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    vcl_cout << "vpgl_translate_geo_camera_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  double lon = pro.get_input<double>(1);
  double lat = pro.get_input<double>(2);
  vpgl_geo_camera* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  double u,v;
  geocam->global_to_img(lon, lat, 0.0, u, v);
  pro.set_output_val<int>(0, (int)u);
  pro.set_output_val<int>(1, (int)v);
  return true;
}

//: construct the camera from the name of the image with a known specific format
//  this geocam has no lvcs, so it's only useful for img_to_global or global_to_img operations
bool vpgl_load_geo_camera_process2_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // image name
  input_types.push_back("unsigned"); // image size ni
  input_types.push_back("unsigned"); // nj 
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  //camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_load_geo_camera_process2(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    vcl_cout << "vpgl_load_geo_camera_process2: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string filename = pro.get_input<vcl_string>(0);
  unsigned ni = pro.get_input<unsigned>(1);
  unsigned nj = pro.get_input<unsigned>(2);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;

  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(filename, ni, nj, lvcs, cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}

//: construct the camera from the header of the geoiff file
bool vpgl_load_geo_camera_process3_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // image name. will load resource from this file
  input_types.push_back("vpgl_lvcs_sptr"); // creates an empty one as a default if not available so do not set if not available
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  //camera output
  bool good = pro.set_input_types(input_types)
           && pro.set_output_types(output_types);

  //input default args
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  brdb_value_sptr vpgl_val = new brdb_value_t<vpgl_lvcs_sptr>(lvcs);
  pro.set_input(1, vpgl_val);

  return good;
}


//: Execute the process
bool vpgl_load_geo_camera_process3(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 2) {
    vcl_cout << "vpgl_load_geo_camera_process3: The number of inputs should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string filename = pro.get_input<vcl_string>(0);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(1);
  vil_image_resource_sptr img = vil_load_image_resource(filename.c_str());

  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(img, lvcs, cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}



//: construct the camera reading from a tfw file 
bool vpgl_save_geo_camera_tfw_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr"); 
  input_types.push_back("vcl_string"); 
  vcl_vector<vcl_string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_save_geo_camera_tfw_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 2) {
    vcl_cout << "vpgl_save_geo_camera_tfw_process: The number of inputs should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string filename = pro.get_input<vcl_string>(1);
  
  vpgl_geo_camera *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    vcl_cerr << "vpgl_save_geo_camera_tfw_process: Cannot cast camera to a geo cam! Exiting!\n";
    return false;
  }

  vcl_ofstream ofs(filename.c_str());
  vnl_matrix<double> trans_matrix = geo_cam->trans_matrix();
  ofs.precision(12);
  ofs << trans_matrix[0][0] << '\n';
  ofs << trans_matrix[0][1] << '\n';
  ofs << trans_matrix[1][0] << '\n';
  ofs << trans_matrix[1][1] << '\n';
  ofs << trans_matrix[0][3] << '\n';
  ofs << trans_matrix[1][3] << '\n';
  ofs.close();
  
  return true;
}
