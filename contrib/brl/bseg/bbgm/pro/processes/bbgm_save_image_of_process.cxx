// This is brl/bseg/bbgm/pro/processes/bbgm_save_image_of_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//: Process construct function
bool bbgm_save_image_of_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(2);
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
    vcl_cerr << "In bbgm_save_image_of_process::execute - invalid inputs\n";
    return false;
  }

  vcl_string binary_filename = pro.get_input<vcl_string>(0);

  vsl_b_ofstream ostr(binary_filename);
  if (!ostr) {
    vcl_cerr << "Failed to load background image from "
             << binary_filename << vcl_endl;
    return false;
  }

  vsl_b_ostream& bos = static_cast<vsl_b_ostream&>(ostr);

  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(1);
  if (!bgm) {
    vcl_cerr << "Null background image\n";
    return false;
  }

  vsl_b_write(bos, bgm);

  return true;
}

