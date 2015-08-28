//This is brl/bpro/core/brad_pro/processes/brad_get_cloud_coverage_process.cxx
//:
// \file
//     extract the cloud coverage from an image_metadata object
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

bool brad_get_cloud_coverage_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_image_metadata_sptr"); // image metadata
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool brad_get_cloud_coverage_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << ": WRONG inputs!!!" << vcl_endl;
    return false;
  }
  //get the inputs
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(0);

  pro.set_output_val<float>(0, float(mdata->cloud_coverage_percentage_));
  return true;
}