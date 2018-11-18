//:
// \file
// \brief A Process to print all devices info
// \author Isabel Restrepo
// \date 9-Mar-2012

#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_manager.h>


//:global variables
namespace bocl_info_process_globals
{
  constexpr unsigned n_inputs_ = 0;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bocl_info_process_cons(bprb_func_process& pro)
{
  using namespace bocl_info_process_globals;
  std::vector<std::string>  input_types_(n_inputs_);
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
  && pro.set_output_types(output_types_);
}


//:the process
bool bocl_info_process(bprb_func_process&  /*pro*/)
{
  using namespace bocl_info_process_globals ;

  bocl_manager_child &mgr = bocl_manager_child::instance();

  //list GPU info
  std::cout<<"  GPUs:\n";
  for (unsigned int i=0; i<mgr.gpus_.size(); ++i) {
    std::cout<<"gpu"<<i<<", "<<*(mgr.gpus_[i]);
  }

  //list CPU info
  std::cout<<"  CPUs:\n";
  for (unsigned int i=0; i<mgr.cpus_.size(); ++i) {
    std::cout<<"cpu"<<i<<", "<<*(mgr.cpus_[i]);
  }

  std::cout<<std::endl;
  return true;
}
