// This is brl/bseg/betr/pro/processes/betr_create_site_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a betr_site
//


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <betr/betr_site.h>
#include <vgl/vgl_point_3d.h>

namespace betr_create_site_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

bool betr_create_site_process_cons(bprb_func_process& pro)
{
  using namespace betr_create_site_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "float";
  input_types_[1] = "float";
  input_types_[2] = "float";

  // process has 1 output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "betr_site_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_create_site_process(bprb_func_process& pro)
{
  using namespace betr_create_site_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  auto org_x = pro.get_input<float>(i++);
  auto org_y = pro.get_input<float>(i++);
  auto org_z = pro.get_input<float>(i);
  vgl_point_3d<double> origin(org_x, org_y, org_z);
  betr_site_sptr site = new betr_site(origin);
  pro.set_output_val<betr_site_sptr>(0, site);
  return true;
}
