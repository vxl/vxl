// This is brl/bpro/core/vpgl_pro/processes/vpgl_geo_camera_processes.cxx
#include <iostream>
#include <vil/vil_config.h>
#if HAS_GEOTIFF
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <bkml/bkml_write.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>


//: process to create a geo camera given its lower left corner and image size
bool vpgl_create_geo_camera_process_cons(bprb_func_process& pro)
{
  // this process takes 7 inputs
  std::vector<std::string> input_types_;
  input_types_.emplace_back("double");          // lower left lon
  input_types_.emplace_back("double");          // lower left lat
  input_types_.emplace_back("double");          // upper right lon
  input_types_.emplace_back("double");          // upper right lat
  input_types_.emplace_back("unsigned");        // image size ni
  input_types_.emplace_back("unsigned");        // image size nj
  input_types_.emplace_back("vpgl_lvcs_sptr");  // camera lvcs, empty by default
  // this process takes 1 output
  std::vector<std::string> output_types_;
  output_types_.emplace_back("vpgl_camera_double_sptr");  // camera output

  // set default lvcs
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  pro.set_input(6, new brdb_value_t<vpgl_lvcs_sptr>(lvcs));
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vpgl_create_geo_camera_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Input!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  auto ll_lon = pro.get_input<double>(in_i++);
  auto ll_lat = pro.get_input<double>(in_i++);
  auto ur_lon = pro.get_input<double>(in_i++);
  auto ur_lat = pro.get_input<double>(in_i++);
  auto ni = pro.get_input<unsigned>(in_i++);
  auto nj = pro.get_input<unsigned>(in_i++);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(in_i++);
  if (ni == 0 || nj == 0) {
    std::cerr << pro.name() << ": image size can not be zero -- ni: " << ni << ", nj: " << nj << "!\n";
    return false;
  }
  double scale_x = (ur_lon - ll_lon) / ni;
  double scale_y = (ll_lat - ur_lat) / nj;
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, lvcs);
  out_cam->set_scale_format(true);

  // output
  pro.set_output_val<vpgl_camera_double_sptr>(0, out_cam);
  return true;
}


//: initialization
bool vpgl_load_geo_camera_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs and one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vpgl_lvcs_sptr");
  input_types.emplace_back("int");  // UTM zone, pass 0 if not UTM
  input_types.emplace_back("unsigned");  // UTM hemisphere, pass 0 for north, 1 for south
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  //camera output

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
    std::cout << "vpgl_load_geo_camera_process: The number of inputs should be 4" << std::endl;
    return false;
  }

  // get the inputs
  std::string tfw_filename = pro.get_input<std::string>(0);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(1);
  int utm_zone = pro.get_input<int>(2);
  auto northing = pro.get_input<unsigned>(3);

  std::ifstream ifs(tfw_filename.c_str());
  if(!ifs)
  {
      std::cout<<"Could not open file "<<std::endl;
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

  std::cout << "trans matrix: " << trans_matrix << std::endl;

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
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.emplace_back("double");
  input_types.emplace_back("double");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  //camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_translate_geo_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    std::cout << "vpgl_translate_geo_camera_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  auto tx = pro.get_input<double>(1);
  auto ty = pro.get_input<double>(2);
  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  auto* outcam = new vpgl_geo_camera(*geocam);
  outcam->translate(tx, ty, 0.0);
  pro.set_output_val<vpgl_camera_double_sptr>(0, outcam);
  return true;
}

//: initialization
bool vpgl_convert_geo_camera_to_generic_process_cons(bprb_func_process& pro)
{
  //this process takes 5 inputs and one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.emplace_back("int");
  input_types.emplace_back("int");
  input_types.emplace_back("double");
  input_types.emplace_back("int");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // generic camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_geo_camera_to_generic_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 5) {
    std::cout << "vpgl_convert_geo_camera_to_generic_process: The number of inputs should be 5" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  int ni = pro.get_input<int>(1);
  int nj = pro.get_input<int>(2);
  auto scene_height = pro.get_input<double>(3);
  int level = pro.get_input<int>(4);
  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  vpgl_generic_camera<double> gcam;
  vpgl_generic_camera_convert::convert(*geocam, ni, nj, scene_height, gcam, level);

  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  return true;
}

//: initialization
//  also input a ray direction, and thus create a generic camera with parallel rays with the given direction
//  so this process creates an orthographic camera which is not necessarily nadir
bool vpgl_convert_non_nadir_geo_camera_to_generic_process_cons(bprb_func_process& pro)
{
  //this process takes 5 inputs and one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.emplace_back("int");
  input_types.emplace_back("int");
  input_types.emplace_back("double");
  input_types.emplace_back("int");
  input_types.emplace_back("double");  // ray direction x   (nadir direction would be (0,0,-1)
  input_types.emplace_back("double");  // ray direction y
  input_types.emplace_back("double");  // ray direction z
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // generic camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_non_nadir_geo_camera_to_generic_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 8) {
    std::cout << "vpgl_convert_geo_camera_to_generic_process: The number of inputs should be 5" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  int ni = pro.get_input<int>(1);
  int nj = pro.get_input<int>(2);
  auto scene_height = pro.get_input<double>(3);
  int level = pro.get_input<int>(4);
  auto dir_x = pro.get_input<double>(5);
  auto dir_y = pro.get_input<double>(6);
  auto dir_z = pro.get_input<double>(7);
  vgl_vector_3d<double> dir(dir_x, dir_y, dir_z);

  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  vpgl_generic_camera<double> gcam;
  vpgl_generic_camera_convert::convert(*geocam, ni, nj, scene_height, dir, gcam, level);

  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  return true;
}


//: initialization
bool vpgl_geo_footprint_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // as read from a geotiff image header or from a tfw file
  input_types.emplace_back("vcl_string");               // geotiff file, to determine image size
  input_types.emplace_back("vcl_string");               // the filename of the kml file to write footprints to
  input_types.emplace_back("bool");                     // put the tags do initialize and end the document, if writing only one file these are needed, otherwise add manually to the file
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_geo_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 4) {
    std::cout << "vpgl_geo_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  std::string geotiff_filename = pro.get_input<std::string>(1);
  vil_image_resource_sptr img_res = vil_load_image_resource(geotiff_filename.c_str());
  std::string footprint_filename = pro.get_input<std::string>(2);
  bool init_finish = pro.get_input<bool>(3);
  unsigned ni = img_res->ni(); unsigned nj = img_res->nj();

  std::ofstream ofs(footprint_filename.c_str(), std::ios::app);
  if (!ofs.good()) {
    std::cerr << "ERROR: vpgl_geo_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  if (init_finish)
    bkml_write::open_document(ofs);

  auto* geocam=dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  if (!geocam) {
    std::cerr << "In vpgl_geo_footprint_process() - input camera is not valid!\n";
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

  std::string g_id = vul_file::strip_directory(geotiff_filename);
  std::string desc = geotiff_filename + " footprint";

  bkml_write::write_box(ofs, g_id, desc, ul, ur, ll, lr);

  if (init_finish)
    bkml_write::close_document(ofs);

  return true;
}

//: process to return the lower left and upper right corners of a geotiff image
bool vpgl_geo_footprint_process2_cons(bprb_func_process& pro)
{
  // this process takes 4 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // as read from a geotiff image header or from a tfw file
  input_types.emplace_back("vcl_string");               // geotiff file, to determine image size
  input_types.emplace_back("vcl_string");               // the filename of the kml file to write footprints to
  input_types.emplace_back("bool");                     // put the tags do initialize and end the document, if writing only one file these are needed, otherwise add manually to the file
  // this process takes 4 outputs:
  std::vector<std::string> output_types;
  output_types.emplace_back("double");                  // lower left  longitude
  output_types.emplace_back("double");                  // lower left   latitude
  output_types.emplace_back("double");                  // upper right longitude
  output_types.emplace_back("double");                  // upper right  latitude
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool vpgl_geo_footprint_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!" << std::endl;
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vpgl_camera_double_sptr cam  = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  std::string geotiff_filename  = pro.get_input<std::string>(in_i++);
  std::string out_footprint_kml = pro.get_input<std::string>(in_i++);
  bool write_kml               = pro.get_input<bool>(in_i++);

  // get the image size
  vil_image_resource_sptr img_res = vil_load_image_resource(geotiff_filename.c_str());
  unsigned ni = img_res->ni();
  unsigned nj = img_res->nj();

  // get the geo camera of the input image
  auto* geocam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geocam) {
    std::cerr << pro.name() << ": input camera is not valid!\n";
    return false;
  }

  // obtain the lower left and upper right corner
  double ll_lon, ll_lat;
  geocam->img_to_global(0,  nj,  ll_lon, ll_lat);
  double ur_lon, ur_lat;
  geocam->img_to_global(ni,  0, ur_lon, ur_lat);

  // generate kml if required
  if (write_kml) {
    std::ofstream ofs(out_footprint_kml.c_str());
    if (!ofs.good()) {
      std::cerr << pro.name() << ": Error opening " << out_footprint_kml << " for writing footprint!\n";
      return false;
    }
    bkml_write::open_document(ofs);
    vgl_box_2d<double> bbox(ll_lon, ur_lon, ll_lat, ur_lat);
    std::cout << "bbox: " << bbox << std::endl;
    vnl_double_2 ul(bbox.max_y(), bbox.min_x());
    vnl_double_2 ur(bbox.max_y(), bbox.max_x());
    vnl_double_2 ll(bbox.min_y(), bbox.min_x());
    vnl_double_2 lr(bbox.min_y(), bbox.max_x());
    std::string img_name = vul_file::strip_directory(geotiff_filename);
    std::string desc     = geotiff_filename + " footPrint";
    bkml_write::write_box(ofs, img_name, desc, ul, ur, ll, lr, 0, 255, 0);
    bkml_write::close_document(ofs);
    ofs.close();
  }

  // output
  std::cout << " lower left: [" << ll_lon << "," << ll_lat << ']' << std::endl;
  std::cout << "upper right: [" << ur_lon << "," << ur_lat << ']' << std::endl;
  // generate output
  unsigned out_i = 0;
  pro.set_output_val<double>(out_i++, ll_lon);
  pro.set_output_val<double>(out_i++, ll_lat);
  pro.set_output_val<double>(out_i++, ur_lon);
  pro.set_output_val<double>(out_i++, ur_lat);

  return true;
}

bool vpgl_geo_cam_global_to_img_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // input geo camera
  input_types.emplace_back("double"); // lon
  input_types.emplace_back("double"); // lat
  std::vector<std::string> output_types;
  output_types.emplace_back("int");  // i
  output_types.emplace_back("int");  // j
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_geo_cam_global_to_img_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    std::cout << "vpgl_translate_geo_camera_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  auto lon = pro.get_input<double>(1);
  auto lat = pro.get_input<double>(2);
  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  double u,v;
  geocam->global_to_img(lon, lat, 0.0, u, v);
  pro.set_output_val<int>(0, (int)u);
  pro.set_output_val<int>(1, (int)v);
  return true;
}

bool vpgl_geo_cam_img_to_global_process_cons(bprb_func_process& pro)
{
  // this process takes 3 inputs and two outputs
  std::vector<std::string> input_types(3);
  input_types[0] = "vpgl_camera_double_sptr";  // input geo camera
  input_types[1] = "unsigned";                 // input pixel row
  input_types[2] = "unsigned";                 // input pixel column
  // this process takes 2 outputs
  std::vector<std::string> output_types(2);
  output_types[0] = "double";                  // lon
  output_types[1] = "double";                  // lat
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool vpgl_geo_cam_img_to_global_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto i = pro.get_input<unsigned>(in_i++);
  auto j = pro.get_input<unsigned>(in_i++);
  // convert
  auto* geocam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geocam) {
    std::cerr << pro.name() << ": Can not convert input camera into a geo camera!\n";
    return false;
  }
  double lon, lat;
  geocam->img_to_global(i, j, lon, lat);
  // output
  pro.set_output_val<double>(0, lon);
  pro.set_output_val<double>(1, lat);
  return true;
}

//: construct the camera from the name of the image with a known specific format
//  this geocam has no lvcs, so it's only useful for img_to_global or global_to_img operations
bool vpgl_load_geo_camera_process2_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // image name
  input_types.emplace_back("unsigned"); // image size ni
  input_types.emplace_back("unsigned"); // nj
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  //camera output
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_load_geo_camera_process2(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    std::cout << "vpgl_load_geo_camera_process2: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  std::string filename = pro.get_input<std::string>(0);
  auto ni = pro.get_input<unsigned>(1);
  auto nj = pro.get_input<unsigned>(2);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;

  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera_from_filename(filename, ni, nj, lvcs, cam);
  //vpgl_geo_camera::init_geo_camera(filename, ni, nj, lvcs, cam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}

//: construct the camera from the header of the geoiff file
bool vpgl_load_geo_camera_process3_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // image name. will load resource from this file
  input_types.emplace_back("vpgl_lvcs_sptr"); // creates an empty one as a default if not available so do not set if not available
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  //camera output
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
    std::cout << "vpgl_load_geo_camera_process3: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  std::string filename = pro.get_input<std::string>(0);
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
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vcl_string");
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_save_geo_camera_tfw_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 2) {
    std::cout << "vpgl_save_geo_camera_tfw_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  std::string filename = pro.get_input<std::string>(1);

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    std::cerr << "vpgl_save_geo_camera_tfw_process: Cannot cast camera to a geo cam! Exiting!\n";
    return false;
  }

  std::ofstream ofs(filename.c_str());
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
#endif
