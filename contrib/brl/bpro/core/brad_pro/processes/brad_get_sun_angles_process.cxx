//This is brl/bpro/core/brad_pro/processes/brad_get_sun_angles_process.cxx
//:
// \file
//     extract the sun azimuth and elevation angles from an image_metadata object
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_get_sun_angles_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brad_image_metadata_sptr"); // image name

  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");
  output_types.push_back("float");
  return pro.set_output_types(output_types);
}

bool brad_get_sun_angles_process(bprb_func_process& pro)
{
  if ( pro.n_inputs() != pro.input_types().size() )
  {
    vcl_cout << pro.name() << " The number of inputs should be " << pro.input_types().size() << vcl_endl;
    return false;
  }

  //get the inputs
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(0);

  pro.set_output_val<float>(0, float(mdata->sun_azimuth_));
  pro.set_output_val<float>(1, float(mdata->sun_elevation_));
  return true;
}

