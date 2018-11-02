// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_compute_derivative_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A filtering process to compute derivative in x,y and z directions.
// The derivative kernels are specified as filenames.
// Files contain kernel coefficients. See contrib/brl/bseg/bvpl/doc/ for examples.
// The user can specify a probability threshold as well as a normal magnitude threshold.
// Points not meeting these criteria will simply not be saved. The user can also specify a specific block to
// run the process on. If no blocks are specified, it is run on all blocks.
//
// \author Ali Osman Ulusoy
// \date Oct 06, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/cpp/algo/boxm2_compute_derivative_function.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_compute_derivative_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_compute_derivative_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_compute_derivative_process_globals;

  //process takes 10 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //probability threshold
  input_types_[3] = "float"; //normal magnitude threshold
  input_types_[4] = "vcl_string"; //kernel_x name
  input_types_[5] = "vcl_string"; //kernel_y name
  input_types_[6] = "vcl_string"; //kernel_z name
  input_types_[7] = "int"; //id_x
  input_types_[8] = "int"; //id_y
  input_types_[9] = "int"; //id_z

  // process has 0 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good =pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default values
  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);

  brdb_value_sptr normal_t = new brdb_value_t<float>(0.0);
  pro.set_input(3, normal_t);

  brdb_value_sptr id_x = new brdb_value_t<int>(-1);
  pro.set_input(7, id_x);

  brdb_value_sptr id_y = new brdb_value_t<int>(-1);
  pro.set_input(8, id_y);

  brdb_value_sptr id_z = new brdb_value_t<int>(-1);
  pro.set_input(9, id_z);

  return good;
}

bool boxm2_cpp_compute_derivative_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_compute_derivative_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  auto prob_threshold = pro.get_input<float>(i++);
  auto normal_threshold = pro.get_input<float>(i++);
  std::string kernel_x_file_name =  pro.get_input< std::string>(i++);
  std::string kernel_y_file_name =  pro.get_input< std::string>(i++);
  std::string kernel_z_file_name =  pro.get_input< std::string>(i++);
  int id_x = pro.get_input<int>(i++);
  int id_y = pro.get_input<int>(i++);
  int id_z = pro.get_input<int>(i++);


  //if id's aren't specified, use all blocks in the scene.
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;
  if (id_x == -1 && id_y == -1 && id_z == -1)
  {
    std::cout << "Running boxm2_cpp_compute_derivative_process on all blocks..." << std::endl;
    blocks = scene->blocks();
  }
  else
  {
    std::cout << "Running boxm2_cpp_compute_derivative_process on block: (" << id_x << ',' << id_y << ',' << id_z << ")..." << std::endl;
    boxm2_block_id id(id_x,id_y,id_z);
    blocks[id] = scene->get_block_metadata(id);
  }

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout<<"Filtering Block: "<<id<<std::endl;

    boxm2_block *     blk = cache->get_block(scene,id);
    boxm2_data_base * alph = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //store normal directions
    int normalsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
    boxm2_data_base * normals = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_NORMAL>::prefix(),alph->buffer_length()/alphaTypeSize*normalsTypeSize,false);

    //store point locations
    int pointsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    boxm2_data_base * points = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_POINT>::prefix(),alph->buffer_length()/alphaTypeSize*pointsTypeSize,false);

    boxm2_block_metadata data = blk_iter->second;
    boxm2_compute_derivative_function(data, blk, alph, normals,points, prob_threshold, normal_threshold, kernel_x_file_name,kernel_y_file_name,kernel_z_file_name);

#ifdef DEBUG
    std::size_t dataSize = alph->buffer_length();
    boxm2_data_traits<BOXM2_POINT>::datatype *   point_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
    boxm2_data_traits<BOXM2_NORMAL>::datatype *   normal_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
    for (unsigned i = 0; i < dataSize /  boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());i++ ) {
#if 0
      std::cout << "p: " << point_data[i][0] << ' ' << point_data[i][1] << ' '<< point_data[i][2] << " n: "
               << normal_data[i][0] << ' '<< normal_data[i][0] << ' '<< normal_data[i][0] << std::endl;
#endif
      std::cout << point_data[i][3] << std::endl;
    }
#endif
  }

  return true;
}
