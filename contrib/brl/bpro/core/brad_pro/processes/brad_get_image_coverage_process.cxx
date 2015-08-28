// This is brl/bpro/core/brad_pro/processes/brad_get_image_coverage_process.cxx
//:
// \file
//     get the image lower left corner and the upper right corner of the 'extent' of the satellite image
//
//
#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

bool brad_get_image_coverage_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_image_metadata_sptr"); // image metadata
  vcl_vector<vcl_string> output_types;
  output_types.push_back("double");
  output_types.push_back("double");
  output_types.push_back("double");
  output_types.push_back("double");
  output_types.push_back("double");
  output_types.push_back("double");
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool brad_get_image_coverage_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << ": WRONG inputs!!!" << vcl_endl;
    return false;
  }

  //get the inputs
  brad_image_metadata_sptr meta = pro.get_input<brad_image_metadata_sptr>(0);

  double lower_left_lon  = meta->lower_left_.x();
  double lower_left_lat  = meta->lower_left_.y();
  double lower_left_elev = meta->lower_left_.z();
  double upper_right_lon = meta->upper_right_.x();
  double upper_right_lat = meta->upper_right_.y();
  double upper_right_elev = meta->upper_right_.z();
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