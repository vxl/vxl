// This is brl/bpro/core/vpgl_pro/processes/vpgl_nitf_footprint_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
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
  // 1: (vcl_string) the filename containing a list of images to evaluate
  // 2: (vcl_string) the filename of the kml file to write footprints to
  // 3-5: (unsigned) the rgb value used for kml visualization

  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vcl_string");
  input_types.push_back("unsigned");
  input_types.push_back("unsigned");
  input_types.push_back("unsigned");

  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_nitf_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 5) {
    vcl_cout << "vpgl_nitf_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string in_img_list = pro.get_input<vcl_string>(0);
  vcl_string footprint_filename = pro.get_input<vcl_string>(1);
  unsigned r = pro.get_input<unsigned>(2);
  unsigned g = pro.get_input<unsigned>(3);
  unsigned b = pro.get_input<unsigned>(4);

  vcl_ofstream ofs(footprint_filename.c_str());
  if (!ofs.good()) {
    vcl_cerr << "ERROR: vpgl_nitf_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  bkml_write::open_document(ofs);

  vcl_ifstream ifs(in_img_list.c_str());
  vcl_string nitf_path;

  while (ifs.good()) {
    ifs >> nitf_path;

    vil_image_resource_sptr image =
        vil_load_image_resource(nitf_path.c_str());
    if (!image)
    {
      vcl_cout << "NITF image load failed in vpgl_nitf_footprint_process\n";
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


    vcl_string nitf_id = vul_file::strip_directory(nitf_path);
    vcl_string desc = nitf_path + " footprint";

    vcl_cout << "image " << desc << ":" << vcl_endl; 
    vcl_cout << "!!!! lower left lon: "  << meta.lower_left_.x()  << " lat: " << meta.lower_left_.y() << '\n';
    vcl_cout << "!!!! upper right lon: " << meta.upper_right_.x() << " lat: " << meta.upper_right_.y() << '\n';
    bkml_write::write_box(ofs, nitf_id, desc, ul, ur, ll, lr, (unsigned char)r, (unsigned char)g, (unsigned char)b);
  }
  bkml_write::close_document(ofs);

  return true;
}

