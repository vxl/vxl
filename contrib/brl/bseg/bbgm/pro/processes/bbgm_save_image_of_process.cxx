// This is brl/bseg/bbgm/pro/processes/bbgm_save_image_of_process.cxx
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

//: Process construct function
bool bbgm_save_image_of_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> in_types(2);
  in_types[0]="vcl_string";//path for saved distribution image
  in_types[1]="bbgm_image_sptr";//pointer to distribution image
  pro.set_input_types(in_types);
  return true;
}

//: Process execute function
bool bbgm_save_image_of_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_save_image_of_process::execute - invalid inputs\n";
    return false;
  }

  std::string binary_filename = pro.get_input<std::string>(0);

  vsl_b_ofstream ostr(binary_filename);
  if (!ostr) {
    std::cerr << "Failed to load background image from "
             << binary_filename << std::endl;
    return false;
  }

  auto& bos = static_cast<vsl_b_ostream&>(ostr);

  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(1);
  if (!bgm) {
    std::cerr << "Null background image\n";
    return false;
  }

  vsl_b_write(bos, bgm);

  return true;
}
