// This is brl/bseg/bstm/pro/processes/bstm_create_cache_process.cxx
//:
// \file
// \brief  A process for creating cache.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>



namespace bstm_create_cache_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}
bool bstm_create_cache_process_cons(bprb_func_process& pro)
{
  using namespace bstm_create_cache_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_cache_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_create_cache_process(bprb_func_process& pro)
{
  using namespace bstm_create_cache_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene= pro.get_input<bstm_scene_sptr>(i++);
  vcl_string cache_type= pro.get_input<vcl_string>(i++);
  if(cache_type=="lru")
    bstm_lru_cache::create(scene);
  else
  {
    vcl_cerr << "Don't recognize cache type " << cache_type << " exiting..." << vcl_endl;
    return false;
  }

  // store scene smaprt pointer
  pro.set_output_val<bstm_cache_sptr>(0, bstm_cache::instance());
  return true;
}

