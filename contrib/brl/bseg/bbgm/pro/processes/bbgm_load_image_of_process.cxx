// This is brl/bseg/bbgm/pro/processes/bbgm_load_image_of_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bbgm/bbgm_loader.h>

//: Process construct function
bool bbgm_load_image_of_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> in_types(1), out_types(1);
  in_types[0]="vcl_string"; //path to distribution image
  pro.set_input_types(in_types);
  out_types[0]="bbgm_image_sptr"; //loaded distribution image
  pro.set_output_types(out_types);
  return true;
}
//: Process execute function
bool bbgm_load_image_of_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_load_image_of_process::execute - invalid inputs\n";
    return false;
  }

  std::string binary_filename = pro.get_input<std::string>(0);

  vsl_b_ifstream istr(binary_filename);
  if (!istr) {
    std::cerr << "Failed to load background image from "
             << binary_filename << std::endl;
    return false;
  }
  //register different distributions for image content
  //the registration will only be done once since new instances of
  //the process are cloned  - maybe later make a separate registration step
  bbgm_loader::register_loaders();
  auto& bis = static_cast<vsl_b_istream&>(istr);
  bbgm_image_sptr image;
  vsl_b_read(bis, image);

  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(image);
  pro.set_output(0, output0);
  return true;
}
