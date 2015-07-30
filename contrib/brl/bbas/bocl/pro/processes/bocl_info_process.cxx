//:
// \file
// \brief A Process to print all devices info
// \author Isabel Restrepo
// \date 9-Mar-2012

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vcl_fstream.h>
#include <bocl/bocl_manager.h>
#include <vcl_sstream.h>


//:global variables
namespace bocl_info_process_globals
{
  const unsigned n_inputs_ = 0;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bocl_info_process_cons(bprb_func_process& pro)
{
  using namespace bocl_info_process_globals;
  vcl_vector<vcl_string>  input_types_(n_inputs_);
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
  && pro.set_output_types(output_types_);
}


//:the process
bool bocl_info_process(bprb_func_process& pro)
{
  using namespace bocl_info_process_globals ;

  bocl_manager_child &mgr = bocl_manager_child::instance();

  //list GPU info
  vcl_cout<<"  GPUs:\n";
  for (unsigned int i=0; i<mgr.gpus_.size(); ++i) {
    vcl_cout<<"gpu"<<i<<", "<<*(mgr.gpus_[i]);
  }

  //list CPU info
  vcl_cout<<"  CPUs:\n";
  for (unsigned int i=0; i<mgr.cpus_.size(); ++i) {
    vcl_cout<<"cpu"<<i<<", "<<*(mgr.cpus_[i]);
  }

  vcl_cout<<vcl_endl;
  return true;
}

