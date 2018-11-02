// This is brl/bbas/bocl/pro/processes/bocl_get_device_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_manager.h>

namespace bocl_get_device_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

bool bocl_get_device_process_cons(bprb_func_process& pro)
{
  using namespace bocl_get_device_process_globals;
  std::vector<std::string>  input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bocl_device_sptr";

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool bocl_get_device_process(bprb_func_process& pro)
{
  using namespace bocl_get_device_process_globals;
  unsigned i=0;
  std::string device_type = pro.get_input<std::string>(i++);
  bocl_manager_child &mgr = bocl_manager_child::instance();

  if (device_type=="gpu" || device_type=="gpu0" )
  {
    if (mgr.gpus_.size()==0) return false;
    bocl_device_sptr device = mgr.gpus_[0];
    pro.set_output_val<bocl_device_sptr>(0,device);
    std::cout<<"Using the following gpu device: \n" << *(device.ptr());
    return true;
  }
  else if (device_type=="cpu" || device_type=="cpu0")
  {
    if (mgr.cpus_.size()==0) return false;
    bocl_device_sptr device = mgr.cpus_[0];
    pro.set_output_val<bocl_device_sptr>(0,device);
    std::cout<<"Using the following cpu device: \n"<<*(device.ptr());
    return true;
  }

  //for multi-gpu setups
  std::string dev = device_type.substr(0,3);
  if (dev=="gpu") {
    std::string str = device_type.substr(3, device_type.size());
    unsigned int gpuIdx;
    std::istringstream ( str ) >> gpuIdx;
    std::cout<<"Setting GPU device #: " << gpuIdx <<std::endl;
    if (gpuIdx < mgr.gpus_.size()) {
      bocl_device_sptr device = mgr.gpus_[gpuIdx];
      pro.set_output_val<bocl_device_sptr>(0,device);
      std::cout<<*(device.ptr());
      return true;
    }
  }

  //at this point, device cannot be recognized
  std::cout<<"Cannot recognize device string.  Available devices:\n";

  //list gpu options
  std::cout<<"  GPUs: ";
  for (unsigned int i=0; i<mgr.gpus_.size(); ++i)
    std::cout<<"gpu"<<i<<", ";
  std::cout<<'\n';

  //list CPU options
  std::cout<<"  CPUs: ";
  for (unsigned int i=0; i<mgr.cpus_.size(); ++i)
    std::cout<<"cpu"<<i <<", ";

  std::cout<<std::endl;
  return false;
}
