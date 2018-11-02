// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_local_to_global_coordinates_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
//: initialization
bool vpgl_convert_local_to_global_coordinates_array_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs:
  // 0: (vpgl_lvcs) lvcs
  // 1: (double)  latitude
  // 2: (double)  longitude
  // 3: (double)  elevation
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_lvcs_sptr");
  input_types.emplace_back("bbas_1d_array_double_sptr");
  input_types.emplace_back("bbas_1d_array_double_sptr");
  input_types.emplace_back("bbas_1d_array_double_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  // outputs x,y,z in local coordinates
  // 0: (double) x
  // 1: (double) y
  // 2: (double) z
  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_double_sptr");
  output_types.emplace_back("bbas_1d_array_double_sptr");
  output_types.emplace_back("bbas_1d_array_double_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;

}

//: Execute the process
bool vpgl_convert_local_to_global_coordinates_array_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != pro.input_types().size()) {
    std::cout << "vpgl_convert_local_to_global_coordinates_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }

  // get the inputs
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(0);
  bbas_1d_array_double_sptr x_lon_sptr = pro.get_input<bbas_1d_array_double_sptr>(1);
  bbas_1d_array_double_sptr y_lat_sptr = pro.get_input<bbas_1d_array_double_sptr>(2);
  bbas_1d_array_double_sptr z_alt_sptr = pro.get_input<bbas_1d_array_double_sptr>(3);

  if(x_lon_sptr->data_array.size() != y_lat_sptr->data_array.size() ||
     x_lon_sptr->data_array.size() != z_alt_sptr->data_array.size())
    return false;

  vbl_array_1d<double>::iterator x_lon_it = x_lon_sptr->data_array.begin();
  vbl_array_1d<double>::iterator y_lat_it = y_lat_sptr->data_array.begin();
  vbl_array_1d<double>::iterator z_alt_it = z_alt_sptr->data_array.begin();

  for(; x_lon_it < x_lon_sptr->data_array.end(); x_lon_it++, y_lat_it++, z_alt_it++)
  {
    lvcs->local_to_global(*x_lon_it, *y_lat_it, *z_alt_it,
                          vpgl_lvcs::wgs84,
                          *x_lon_it, *y_lat_it, *z_alt_it,
                          vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  }

  pro.set_output_val<bbas_1d_array_double_sptr>(0, y_lat_sptr);
  pro.set_output_val<bbas_1d_array_double_sptr>(1, x_lon_sptr);
  pro.set_output_val<bbas_1d_array_double_sptr>(2, z_alt_sptr);

  return true;
}
