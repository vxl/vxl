// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_update_nonsurface_model_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to update the scene using a set of data blocks in a batch mode
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data_base.h>

//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_synoptic_function_functors.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

//: run batch update
namespace boxm2_cpp_batch_update_nonsurface_model_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_update_nonsurface_model_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_update_nonsurface_model_process_globals;

  //process takes 3 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_update_nonsurface_model_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_update_nonsurface_model_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache = pro.get_input<boxm2_stream_cache_sptr>(i++);

  // assumes that the data of each image has been created in the data models previously
  // (but unused:) int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  for (id = blk_ids.begin(); id != blk_ids.end(); id++) {
    boxm2_data_base *  alpha = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,true);

    // pass num_bytes = 0 to make sure disc is read if not already in memory
    boxm2_data_base *  entropy_histo_air = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix("entropy_histo_air"),alpha->buffer_length(),false);
    boxm2_compute_empty_model_gradient_functor data_functor;
    data_functor.init_data(entropy_histo_air, str_cache);
    int histo_entropy_airTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    // check for invalid parameters
    if( histo_entropy_airTypeSize == 0 ) {
    //This should never happen, it will result in division by zero later
        std::cout << "ERROR: histo_entropy_airTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
        return false;
    }

    int data_buff_length = (int)(entropy_histo_air->buffer_length()/histo_entropy_airTypeSize);
    boxm2_data_serial_iterator<boxm2_compute_empty_model_gradient_functor>(data_buff_length,data_functor);

    cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix("entropy_histo_air"));
  }
  return true;
}
