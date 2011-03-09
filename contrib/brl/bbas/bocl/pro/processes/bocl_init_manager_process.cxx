// This is brl/bbas/bocl/pro/processes/bocl_init_manager_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a bocl manager
//
// \author Vishal Jain
// \date Mar 8, 2011
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <bocl/bocl_manager.h>


namespace bocl_init_manager_process_globals
{
  const unsigned n_inputs_ = 0;
  const unsigned n_outputs_ = 1;
}

bool bocl_init_manager_process_cons(bprb_func_process& pro)
{
  using namespace bocl_init_manager_process_globals;

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bocl_manager_child_sptr";

  return pro.set_output_types(output_types_);
}

bool bocl_init_manager_process(bprb_func_process& pro)
{
  using namespace bocl_init_manager_process_globals;
  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  pro.set_output_val<bocl_manager_child_sptr>(0,mgr);
  return true;
}
