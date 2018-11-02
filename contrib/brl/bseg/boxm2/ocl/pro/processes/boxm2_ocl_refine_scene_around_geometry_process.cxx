// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_refine_scene_around_geometry.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for refining the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/ocl/algo/boxm2_ocl_refine_scene_around_geometry.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_ocl_refine_scene_around_geometry_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_ocl_refine_scene_around_geometry_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_refine_scene_around_geometry_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_opencl_cache_sptr";
  input_types_[2] = "bocl_device_sptr";
  input_types_[3] = "bvpl_kernel_vector_sptr";
  input_types_[4] = "int";
  input_types_[5] = "float";
  input_types_[6] = "bool";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_refine_scene_around_geometry_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_refine_scene_around_geometry_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::cout<<" got to process start"<<std::endl;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bvpl_kernel_vector_sptr filter_vector = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  int  num_times = pro.get_input<int>(i++);
  auto p_thresh = pro.get_input<float>(i++);
  bool refine_gpu = pro.get_input<bool>(i++);

  bool foundDataType = false;
  std::cout<<device<<std::endl;
  std::cout<<" got to refine"<<std::endl;
  boxm2_ocl_refine_scene_around_geometry refinement_engine(scene,cache,device,filter_vector,
                         num_times,p_thresh,refine_gpu);

  refinement_engine.refine();



  return true;

}
