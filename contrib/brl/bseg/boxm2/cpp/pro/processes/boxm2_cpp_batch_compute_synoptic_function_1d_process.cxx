// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_batch_compute_synoptic_function_1d_process.cxx
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
namespace boxm2_cpp_batch_compute_synoptic_function_1d_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_batch_compute_synoptic_function_1d_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_synoptic_function_1d_process_globals;

  //process takes 5 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) the sun elevation
  // 4) the sun azimuth
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  // process has 0 outputs:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_batch_compute_synoptic_function_1d_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_batch_compute_synoptic_function_1d_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
      std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
      return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache = pro.get_input<boxm2_stream_cache_sptr>(i++);

  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); id++) {
    boxm2_block *     blk = cache->get_block(scene,*id);
    boxm2_data_base *  alpha = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),0,false);
    boxm2_data_base *  cubic_model_data = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix("cubic_model"),alpha->buffer_length()* 8 ,false);
    boxm2_synoptic_fucntion_1d_functor data_functor;
    data_functor.init_data(str_cache,alpha,cubic_model_data);
    int phongs_model_TypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT8>::prefix());
    // check for invalid parameters
    if( phongs_model_TypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
      std::cout << "ERROR: phongs_model_TypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
      return false;
    }

    int data_buff_length = (int) (cubic_model_data->buffer_length()/phongs_model_TypeSize);

    boxm2_data_leaves_serial_iterator<boxm2_synoptic_fucntion_1d_functor>(blk,data_buff_length,data_functor);

    cache->remove_data_base(scene, *id, boxm2_data_traits<BOXM2_FLOAT8>::prefix("cubic_model") );
  }

  std::cout<<"Finished Cubic "<<std::endl;
  return true;
}
