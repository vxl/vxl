// This is brl/bseg/boxm2/pro/processes/boxm2_create_cache_process.cxx
//:
// \file
// \brief  A process for creating cache. 
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_nn_cache.h>



namespace boxm2_create_cache_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}
bool boxm2_create_cache_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_cache_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_cache_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_cache_process(bprb_func_process& pro)
{
  using namespace boxm2_create_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene= pro.get_input<boxm2_scene_sptr>(i++);
  vcl_string cache_type= pro.get_input<vcl_string>(i++);
  if(cache_type=="lru")
  {
      vcl_cout<<"Create Cache"<<vcl_endl;
      boxm2_lru_cache::create(scene);
  }
  else if (cache_type=="nn")
  {
     // boxm2_nn_cache::create(scene);

      return false;
  }
  else
  {
    return false;
  }
  i=0;
  
  // store scene smaprt pointer
  pro.set_output_val<boxm2_cache_sptr>(i++, boxm2_cache::instance());
  return true;
}

