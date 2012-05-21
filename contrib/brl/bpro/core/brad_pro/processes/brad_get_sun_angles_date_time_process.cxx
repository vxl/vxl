//This is brl/bpro/core/brad_pro/processes/brad_get_sun_angles_date_time_process.cxx
//:
// \file
//     compute sun position for given location, date, time
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_sun_pos.h>

//: set input and output types
bool brad_get_sun_angles_date_time_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("float"); // latitude
  input_types.push_back("float"); // longitude
  input_types.push_back("int"); // year
  input_types.push_back("int"); // month
  input_types.push_back("int"); // day
  input_types.push_back("int"); // hour
  input_types.push_back("int"); // minute

  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");
  output_types.push_back("float");
  return pro.set_output_types(output_types);
}

bool brad_get_sun_angles_date_time_process(bprb_func_process& pro)
{
  if ( pro.n_inputs() != pro.input_types().size() )
  {
    vcl_cout << pro.name() << " The number of inputs should be " << pro.input_types().size() << vcl_endl;
    return false;
  }

  //get the inputs
  float lat = pro.get_input<float>(0);
  float lon = pro.get_input<float>(1);
  int year = pro.get_input<int>(2);
  int month = pro.get_input<int>(3);
  int day = pro.get_input<int>(4);
  int hour = pro.get_input<int>(5);
  int minute = pro.get_input<int>(6);

  // get sun position
  double sun_az, sun_el;
  brad_sun_pos(year, month,  day, hour, minute, 0, lon, lat, sun_az, sun_el);

  pro.set_output_val<float>(0, float(sun_az));
  pro.set_output_val<float>(1, float(sun_el));
  return true;
}

