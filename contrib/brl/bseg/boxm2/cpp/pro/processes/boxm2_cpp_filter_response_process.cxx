// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_filter_response_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief This process runs a number of kernels specified in files. Kernels are specified in text files named filter_base_name_i.txt where i is the kernel number.
//        The kernels should be formatted according to bvpl_kernel conventions. See bvpl_kernel.print_to_file(filename) function as an example.
//        The outputs are saved in response data types which can be specified by the user based on how the kernel responses are to be used and also
//        on the number of kernels. Note that boxm2_apply_filter_function which does the main work is templated over the responses type.
//        If a block is specified, the process is run on that block alone. Otherwise it is run on the whole scene.
//
// \author Ali Osman Ulusoy
// \date Dec 1, 2011

#include <vcl_fstream.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_apply_filter_function.h>
#include <bvpl/kernels/bvpl_kernel.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_filter_response_process_globals
{
  const unsigned n_inputs_ =  8;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_filter_response_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_response_process_globals;

  //process takes 10 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float";      //probability threshold
  input_types_[3] = "vcl_string"; //filter base name
  input_types_[4] = "unsigned";   //number of filters
  input_types_[5] = "int"; //id_x
  input_types_[6] = "int"; //id_y
  input_types_[7] = "int"; //id_z

  // process has 0 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good =pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default values
  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);

  brdb_value_sptr id_x = new brdb_value_t<int>(-1);
  pro.set_input(5, id_x);

  brdb_value_sptr id_y = new brdb_value_t<int>(-1);
  pro.set_input(6, id_y);

  brdb_value_sptr id_z = new brdb_value_t<int>(-1);
  pro.set_input(7, id_z);

  return good;
}

bool boxm2_cpp_filter_response_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_filter_response_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  float prob_threshold = pro.get_input<float>(i++);
  vcl_string kernel_base_file_name =  pro.get_input< vcl_string>(i++);
  unsigned num_kernels = pro.get_input<unsigned>(i++);
  int id_x = pro.get_input<int>(i++);
  int id_y = pro.get_input<int>(i++);
  int id_z = pro.get_input<int>(i++);


  //if id's aren't specified, use all blocks in the scene.
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks;
  if (id_x == -1 && id_y == -1 && id_z == -1)
  {
    vcl_cout << "Running boxm2_cpp_filter_response_process on all blocks..." << vcl_endl;
    blocks = scene->blocks();
  }
  else
  {
    vcl_cout << "Running boxm2_cpp_filter_response_process on block: (" << id_x << ',' << id_y << ',' << id_z << ")..." << vcl_endl;
    boxm2_block_id id(id_x,id_y,id_z);
    blocks[id] = scene->get_block_metadata(id);
  }

  //construct boxm2_apply_filter_function
  boxm2_apply_filter_function<BOXM2_FLOAT16> filter_function(kernel_base_file_name,num_kernels);
  if(filter_function.num_kernels() != num_kernels)
    return false;

  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    vcl_cout<<"Filtering Block: "<<id<<vcl_endl;

    boxm2_block *     blk     = cache->get_block(id);
    boxm2_data_base * alph    = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_block_metadata data = blk_iter->second;

    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //store responses
    int responseTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT16>::prefix());
    boxm2_data_base * response    = cache->get_data_base(id,boxm2_data_traits<BOXM2_FLOAT16>::prefix(),alph->buffer_length()/alphaTypeSize*responseTypeSize,false);

    //store point locations
    int pointsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    boxm2_data_base * points    = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix(),alph->buffer_length()/alphaTypeSize*pointsTypeSize,false);

    filter_function.apply_filter(data, blk, alph, response, points, prob_threshold);

  }

  return true;
}
