// This is brl/bpro/core/vpgl_pro/processes/vpgl_nitf_footprint_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vul/vul_file.h>
#include <bkml/bkml_write.h>
#include <brad/brad_image_metadata.h>

//: initialization
bool vpgl_nitf_footprint_process_cons(bprb_func_process& pro)
{
  //this process takes 5 inputs:
  // 1: (std::string) the filename containing a list of images to evaluate
  // 2: (std::string) the filename of the kml file to write footprints to
  // 3-5: (unsigned) the rgb value used for kml visualization

  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("unsigned");
  input_types.emplace_back("unsigned");
  input_types.emplace_back("unsigned");

  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_nitf_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 5) {
    std::cout << "vpgl_nitf_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << std::endl;
    return false;
  }

  // get the inputs
  std::string in_img_list = pro.get_input<std::string>(0);
  std::string footprint_filename = pro.get_input<std::string>(1);
  auto r = pro.get_input<unsigned>(2);
  auto g = pro.get_input<unsigned>(3);
  auto b = pro.get_input<unsigned>(4);

  std::ofstream ofs(footprint_filename.c_str());
  if (!ofs.good()) {
    std::cerr << "ERROR: vpgl_nitf_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  bkml_write::open_document(ofs);

  std::ifstream ifs(in_img_list.c_str());
  std::string nitf_path;

  while (ifs.good()) {
    ifs >> nitf_path;

    vil_image_resource_sptr image =
        vil_load_image_resource(nitf_path.c_str());
    if (!image)
    {
      std::cout << "NITF image load failed in vpgl_nitf_footprint_process\n";
      return false;
    }

    // use brad_image_metadata to obtain the image foot print (it will use IMD or PVL when IGEOLO is not available)
    brad_image_metadata meta(nitf_path,"");

    vgl_box_2d<double> bbox(meta.lower_left_.x(), meta.upper_right_.x(),
                            meta.lower_left_.y(), meta.upper_right_.y());

    vnl_double_2 ul(bbox.max_y(), bbox.min_x());
    vnl_double_2 ur(bbox.max_y(), bbox.max_x());
    vnl_double_2 ll(bbox.min_y(), bbox.min_x());
    vnl_double_2 lr(bbox.min_y(), bbox.max_x());


    std::string nitf_id = vul_file::strip_directory(nitf_path);
    std::string desc = nitf_path + " footprint";

    std::cout << "image " << desc << ":" << std::endl;
    std::cout << "!!!! lower left lon: "  << meta.lower_left_.x()  << " lat: " << meta.lower_left_.y() << '\n';
    std::cout << "!!!! upper right lon: " << meta.upper_right_.x() << " lat: " << meta.upper_right_.y() << '\n';
    bkml_write::write_box(ofs, nitf_id, desc, ul, ur, ll, lr, (unsigned char)r, (unsigned char)g, (unsigned char)b);
  }
  bkml_write::close_document(ofs);

  return true;
}

//: initialization
bool vpgl_nitf_footprint_process2_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  // 1: (std::string) the image filename
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("bool");
  input_types.emplace_back("vcl_string"); // metadata folder

  if(!pro.set_input_types(input_types) )
    return false;

  pro.set_input(3, new brdb_value_t<std::string>("")); // empty metadata folder

  //this process takes 4 outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("double");  // lower left  lon
  output_types.emplace_back("double");  // lower left  lat
  output_types.emplace_back("double");  // lower left  elev
  output_types.emplace_back("double");  // upper right lon
  output_types.emplace_back("double");  // upper right lat
  output_types.emplace_back("double");  // upper right elev
  pro.set_output_types(output_types);
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: execute the process
bool vpgl_nitf_footprint_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Invalid inputs\n";
    return false;
  }

  // get the inputs
  std::string img_file = pro.get_input<std::string>(0);
  std::string kml_file = pro.get_input<std::string>(1);
  bool         is_kml = pro.get_input<bool>(2);
  std::string metadatafolder = pro.get_input<std::string>(3);
  if (!vul_file::exists(img_file)) {
    std::cerr << pro.name() << ": can not find input image file: " << img_file << "!\n";
    return false;
  }


  // load image metadata
  brad_image_metadata meta(img_file,metadatafolder);
  double lower_left_lon  = meta.lower_left_.x();
  double lower_left_lat  = meta.lower_left_.y();
  double lower_left_elev = meta.lower_left_.z();
  double upper_right_lon = meta.upper_right_.x();
  double upper_right_lat = meta.upper_right_.y();
  double upper_right_elev = meta.upper_right_.z();

  // create kml file
  if (is_kml)
  {
    std::ofstream ofs(kml_file.c_str());
    if (!ofs.good()) {
      std::cerr << pro.name() << ": Error opening " << kml_file << " for write!\n";
      return false;
    }

    bkml_write::open_document(ofs);
    vgl_box_2d<double> bbox(meta.lower_left_.x(), meta.upper_right_.x(),
                              meta.lower_left_.y(), meta.upper_right_.y());

    vnl_double_2 ul(bbox.max_y(), bbox.min_x());
    vnl_double_2 ur(bbox.max_y(), bbox.max_x());
    vnl_double_2 ll(bbox.min_y(), bbox.min_x());
    vnl_double_2 lr(bbox.min_y(), bbox.max_x());
    std::string nitf_id = vul_file::strip_directory(img_file);
    std::string desc = img_file + " footprint";
    bkml_write::write_box(ofs, nitf_id, desc, ul, ur, ll, lr, 0, 255, 0);
    bkml_write::close_document(ofs);
    ofs.close();
  }

  std::cout << " lower left: " << meta.lower_left_ << std::endl;
  std::cout << "upper right: " << meta.upper_right_ << std::endl;
  // generate output
  unsigned i = 0;
  pro.set_output_val<double>(i++,  lower_left_lon);
  pro.set_output_val<double>(i++,  lower_left_lat);
  pro.set_output_val<double>(i++,  lower_left_elev);
  pro.set_output_val<double>(i++, upper_right_lon);
  pro.set_output_val<double>(i++, upper_right_lat);
  pro.set_output_val<double>(i++, upper_right_elev);

  return true;
}
