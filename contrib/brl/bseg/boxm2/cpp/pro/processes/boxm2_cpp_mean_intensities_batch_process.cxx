// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_mean_intensities_batch_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for finding mean intensities at each voxel using a set of data blocks in a batch mode
//
// \author Ozge C. Ozcanli
// \date May 12, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/cpp/algo/boxm2_mean_intensities_batch_functor.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>
#include <boxm2/cpp/algo/boxm2_cast_intensities_functor.h>

namespace boxm2_cpp_mean_intensities_batch_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_mean_intensities_batch_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_mean_intensities_batch_process_globals;

  //process takes 3 inputs
  // 0) scene
  // 2) cache
  // 3) stream cache
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_mean_intensities_batch_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_mean_intensities_batch_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache =pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache= pro.get_input<boxm2_stream_cache_sptr>(i++);

  // assumes that the intensities of each image have been cast into data models of type ALPHA previously
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  id = blk_ids.begin();
  for (id = blk_ids.begin(); id != blk_ids.end(); ++id) {
    // we're assuming that we have enough RAM to store the whole output block for alpha
    boxm2_data_base *  output_alph = cache->get_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(),0,false);
    boxm2_mean_intensities_batch_functor data_functor;
    data_functor.init_data(output_alph, str_cache);
    int data_buff_length = (int) (output_alph->buffer_length()/alphaTypeSize);
    boxm2_data_serial_iterator<boxm2_mean_intensities_batch_functor>(data_buff_length,data_functor);
    cache->remove_data_base(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix());  // cache needs to be read-write cache for output alpha blocks to be written before being discarded
  }

  return true;
}

namespace boxm2_cpp_mean_intensities_print_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_mean_intensities_print_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_mean_intensities_print_process_globals;

  //process takes 3 inputs
  // 0) scene
  // 1) cache
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "boxm2_stream_cache_sptr";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_mean_intensities_print_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_mean_intensities_print_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
      std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
      return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache =pro.get_input<boxm2_cache_sptr>(i++);
  boxm2_stream_cache_sptr str_cache =pro.get_input<boxm2_stream_cache_sptr>(i++);

  // assumes that the intensities of each image have been cast into data models of type ALPHA previously
  int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
  // iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  for (auto & blk_id : blk_ids)
  {
    // we're assuming that we have enough RAM to store the whole output block for alpha
    boxm2_data_base * output_alph = cache->get_data_base(scene,blk_id,boxm2_data_traits<BOXM2_AUX0>::prefix());
    boxm2_mean_intensities_print_functor data_functor;
    data_functor.init_data(output_alph,str_cache);
    int data_buff_length = (int) (output_alph->buffer_length()/alphaTypeSize);
    boxm2_data_serial_iterator<boxm2_mean_intensities_print_functor>(data_buff_length,data_functor);
    cache->remove_data_base(scene,blk_id,boxm2_data_traits<BOXM2_AUX0>::prefix());
  }

  return true;
}

//: a process to be used for debugging purposes to see the values inside given datatypes
namespace boxm2_cpp_data_print_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_data_print_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_data_print_process_globals;

  //process takes 4 inputs
  // 0) scene
  // 1) cache
  // 2) data type string
  // 3) identifier (e.g. img_0)
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_cpp_data_print_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_data_print_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
      std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
      return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache =pro.get_input<boxm2_cache_sptr>(i++);
  std::string data_type = pro.get_input<std::string>(i++);
  std::string identifier = pro.get_input<std::string>(i++);

#if 0
  std::size_t TypeSize = boxm2_data_info::datasize(data_type);
  std::string prefix = data_type + "_" + identifier;

  //: iterate the scene block by block and write to output
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  for (std::vector<boxm2_block_id>::iterator id = blk_ids.begin(); id != blk_ids.end(); ++id)
  {
    boxm2_block * blk = cache->get_block(*id);
    //: we're assuming that we have enough RAM to store the whole output block for alpha
    boxm2_data_base * output = cache->get_data_base(*id,prefix);
    boxm2_data_print_functor data_functor;
    data_functor.init_data(output,TypeSize,prefix);
    int data_buff_length = (int) (output->buffer_length()/(int)TypeSize);
    boxm2_data_serial_iterator<boxm2_data_print_functor>(data_buff_length,data_functor);
    cache->remove_data_base(*id,prefix);
  }
#endif // 0
  return true;
}
