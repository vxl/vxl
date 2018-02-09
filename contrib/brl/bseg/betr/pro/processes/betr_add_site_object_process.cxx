// This is brl/bseg/betr/pro/processes/betr_add_site_object_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <vcl_string.h>
//:
// \file
// \brief  A process for adding an site object to a site
//


#include <vcl_compiler.h>
#include <betr/betr_site.h>
#include <vgl/vgl_point_3d.h>

namespace betr_add_site_object_process_globals
{
  const unsigned n_inputs_  = 6;
  const unsigned n_outputs_ = 0;
}

bool betr_add_site_object_process_cons(bprb_func_process& pro)
{
  using namespace betr_add_site_object_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]  = "betr_site_sptr"; //site
  input_types_[1]  = "vcl_string";//name
  input_types_[2]  = "float";// lon
  input_types_[3]  = "float";// lat
  input_types_[4]  = "float";// elev
  input_types_[5]  = "vcl_string";// geom path
  // process has 0 outputs
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_add_site_object_process(bprb_func_process& pro)
{
  using namespace betr_add_site_object_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_site_sptr site = pro.get_input<betr_site_sptr>(i++);
  std::string name = pro.get_input<vcl_string>(i++);
  float lon = pro.get_input<float>(i++);
  float lat = pro.get_input<float>(i++);
  float elev = pro.get_input<float>(i++);
  std::string geom_path = pro.get_input<vcl_string>(i++);
  if(!site)
    return false;
  bool good = site->add_geo_object(name, lon, lat, elev, geom_path);
  return good;
}
