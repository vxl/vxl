//This is brl/bpro/core/brad_pro/processes/brad_nitf_read_metadata_process.cxx
//:
// \file
//     Read the metadata for the given satellite image,
//       the metadata files are assumed to be in the same folder as the imagefile
//       
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_nitf_read_metadata_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // image name
  input_types.push_back("vcl_string"); // meta folder if exists
  
  if (!pro.set_input_types(input_types))
    return false;

  // in case the 1st input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(1, idx);

  vcl_vector<vcl_string> output_types;
  output_types.push_back("brad_image_metadata_sptr"); 
  return pro.set_output_types(output_types);
}

bool brad_nitf_read_metadata_process(bprb_func_process& pro)
{
  if (pro.n_inputs()<1)
  {
    vcl_cout << pro.name() << " The input number should be " << 1 << vcl_endl;
    return false;
  }

  //get the inputs
  vcl_string nitf_img_name = pro.get_input<vcl_string>(0);
  vcl_string meta_folder = pro.get_input<vcl_string>(1);
  
  brad_image_metadata_sptr md = new brad_image_metadata;
  md->parse(nitf_img_name, meta_folder);
  pro.set_output_val<brad_image_metadata_sptr>(0, md);
  return true;
}

