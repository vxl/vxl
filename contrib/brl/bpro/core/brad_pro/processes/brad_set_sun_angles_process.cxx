//This is brl/bpro/core/brad_pro/processes/brad_set_sun_angles_process.cxx
//:
// \file
//     set the sun azimuth and elevation angles in a brad_image_metadata instance
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_set_sun_angles_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_image_metadata_sptr"); // image name
  input_types.emplace_back("float"); // sun azimuth
  input_types.emplace_back("float"); // sun elevation

  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool brad_set_sun_angles_process(bprb_func_process& pro)
{
  if ( pro.n_inputs() != pro.input_types().size() )
  {
    std::cout << pro.name() << " The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  //get the inputs
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(0);
  auto sun_az = pro.get_input<float>(1);
  auto sun_el = pro.get_input<float>(2);

  // set the sun angles
  mdata->sun_azimuth_ = sun_az;
  mdata->sun_elevation_ = sun_el;

  // no outputs
  return true;
}
