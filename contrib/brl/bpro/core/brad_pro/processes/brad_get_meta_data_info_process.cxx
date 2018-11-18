//This is brl/bpro/core/brad_pro/processes/brad_get_sun_angles_process.cxx
//:
// \file
//     extract the sun azimuth and elevation angles from an image_metadata object
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>

//: set input and output types
bool brad_get_meta_data_info_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("brad_image_metadata_sptr"); // image name

  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("float"); // sun azimuth angle
  output_types.emplace_back("float"); // sun elevation
  output_types.emplace_back("int"); // year
  output_types.emplace_back("int"); // month
  output_types.emplace_back("int"); // day
  output_types.emplace_back("int"); // hour
  output_types.emplace_back("int"); // minute
  output_types.emplace_back("int"); // seconds
  output_types.emplace_back("float"); // ground sampling distance (GSD)
  output_types.emplace_back("vcl_string"); // satellite name
  output_types.emplace_back("float"); // sun azimuth angle
  output_types.emplace_back("float"); // sun elevation
  output_types.emplace_back("vcl_string");  // image band type
  return pro.set_output_types(output_types);
}

bool brad_get_meta_data_info_process(bprb_func_process& pro)
{
  if ( pro.n_inputs() != pro.input_types().size() )
  {
    std::cout << pro.name() << " The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  //get the inputs
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(0);

  pro.set_output_val<float>(0, float(mdata->sun_azimuth_));
  pro.set_output_val<float>(1, float(mdata->sun_elevation_));
  pro.set_output_val<int>(2, mdata->t_.year);
  pro.set_output_val<int>(3, mdata->t_.month);
  pro.set_output_val<int>(4, mdata->t_.day);
  pro.set_output_val<int>(5, mdata->t_.hour);
  pro.set_output_val<int>(6, mdata->t_.min);
  pro.set_output_val<int>(7, mdata->t_.sec);
  pro.set_output_val<float>(8, mdata->gsd_);
  pro.set_output_val<std::string>(9, mdata->satellite_name_);
  pro.set_output_val<float>(10, float(mdata->view_azimuth_));
  pro.set_output_val<float>(11, float(mdata->view_elevation_));
  pro.set_output_val<std::string>(12, mdata->band_);
  return true;
}
