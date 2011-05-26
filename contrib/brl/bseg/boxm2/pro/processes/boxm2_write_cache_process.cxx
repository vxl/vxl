// This is brl/bseg/boxm2/pro/processes/boxm2_write_cache_process.cxx
//:
// \file
// \brief  Writes cache out to disk.  
//
// \author Andrew Miller
// \date May 26, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_nn_cache.h>



namespace boxm2_write_cache_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}
bool boxm2_write_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_write_cache_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_cache_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_write_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_write_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  cache->write_to_disk(); 
  return true;
}

