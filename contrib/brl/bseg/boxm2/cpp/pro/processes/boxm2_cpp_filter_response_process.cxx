// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_filter_response_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief This process evaluates a kernel over the scene and saves the responses to a specified datatype.
// Kernels are specified in text files named filterBaseName_id.txt where id is the kernel number.
// The basename and id are inputs to the process. The kernels should be formatted according to bvpl_kernel conventions.
// See bvpl_kernel.print_to_file(filename) function as an example. Kernels are currently
// evaluated only at the leaf cells with prob. higher than a probability threshold.
// To evaluate the kernel, a fixed size grid is overlaid centered at the current voxel.
// The size of each cell in the grid matches the size of a cell at octree_level, which is another input.
// The process is templated over the response type. Currently, BOXM2_FLOAT is available.
// The outputs are saved in response datatypes concatenated to the id of the kernel, e.g.,
// 4th kernel response for block 0_0_0 is saved in float_4_id_0_0_0.bin
// The interface of this process makes it easy to run multiple kernels concurrently using the multi-threading capabilities of Python.
// Finally, see process boxm2OclAggregateNormalFromFilterProcess as an example of aggregating filter responses to BOXM2_NORMAL.
//
// \author Ali Osman Ulusoy
// \date Dec 1, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_apply_filter_function.h>
#include <bvpl/kernels/bvpl_kernel.h>


namespace boxm2_cpp_filter_response_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;

  typedef boxm2_data_traits<BOXM2_FLOAT> RESPONSE_DATATRAIT;
}

bool boxm2_cpp_filter_response_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_response_process_globals;

  //process takes 10 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float";      //probability threshold
  input_types_[3] = "vcl_string"; //filter base name
  input_types_[4] = "unsigned";   //id of filter
  input_types_[5] = "unsigned";   //octree level to eval kernel

  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good =pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default values
  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);

  brdb_value_sptr octree_lvl = new brdb_value_t<unsigned>(4); //default octree level is the smallest cell size.
  pro.set_input(5, octree_lvl);

  return good;
}

bool boxm2_cpp_filter_response_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_response_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  auto prob_threshold = pro.get_input<float>(i++);
  std::string kernel_base_file_name =  pro.get_input< std::string>(i++);
  auto id_kernel = pro.get_input<unsigned>(i++);
  auto octree_lvl = pro.get_input<unsigned>(i++);

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::cout << "Running boxm2_cpp_filter_response_process ..." << std::endl;

  //construct boxm2_apply_filter_function
  boxm2_apply_filter_function<BOXM2_FLOAT> filter_function(kernel_base_file_name,id_kernel);

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout<<"Filtering Block: "<<id<<std::endl;

    boxm2_block *     blk = cache->get_block(scene,id);
    boxm2_data_base * alph = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_block_metadata data = blk_iter->second;

    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //store responses
    int responseTypeSize = (int)boxm2_data_info::datasize(RESPONSE_DATATRAIT::prefix());
    std::string kernel_name = vul_file::strip_directory(kernel_base_file_name);
    std::stringstream ss; ss << kernel_name << "_" << id_kernel;
    std::cout << "Data type: " << RESPONSE_DATATRAIT::prefix(ss.str()) << std::endl;
    boxm2_data_base * response = cache->get_data_base(scene,id,RESPONSE_DATATRAIT::prefix(ss.str()),alph->buffer_length()/alphaTypeSize*responseTypeSize,false);

    filter_function.apply_filter(data, blk, alph, response, prob_threshold,  octree_lvl);
  }

  return true;
}
