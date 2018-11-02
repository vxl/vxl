// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_paint_batch_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <boxm2/ocl/algo/boxm2_ocl_paint_batch.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_util.h>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//bsta sigma normalizer...
#include <bsta/algo/bsta_sigma_normalizer.h>
#include <vul/vul_timer.h>

namespace boxm2_ocl_paint_batch_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_ocl_paint_batch_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_paint_batch_process_globals;

  //process takes 4 inputs
  // 0) scene
  // 1) cache
  // 2) stream cache
  // 3) the pre-computed sigma normalizer table, for fast access to normalizer values given number of images
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "boxm2_stream_cache_sptr";
  input_types_[4] = "bsta_sigma_normalizer_sptr";
  // process has 0 output:
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_paint_batch_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_paint_batch_process_globals;

  //using namespace boxm2_ocl_paint_batch_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
      std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
      return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr           device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr           scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr    ocl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  boxm2_stream_cache_sptr    str_cache = pro.get_input<boxm2_stream_cache_sptr>(i++);
  bsta_sigma_normalizer_sptr n_table = pro.get_input<bsta_sigma_normalizer_sptr>(i++);

  //get scene data type and appTypeSize
  std::string data_type;
  int appTypeSize;
  std::vector<std::string> valid_types;
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix());
  if ( !boxm2_util::verify_appearance(*scene, valid_types, data_type, appTypeSize) ) {
    std::cout<<"boxm2_ocl_paint_batch ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  //command queue
  int status;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) {
    std::cout<<" ERROR in initializing a queue"<<std::endl;
    return false;
  }

  // iterate the scene block by block and write to output
  vul_timer totalTime;
  std::vector<boxm2_block_id> blk_ids = scene->get_block_ids();
  auto id = blk_ids.begin();
  for (; id != blk_ids.end(); ++id) {
    boxm2_block_id bid = *id;
    std::cout<<" block "<<bid<<std::endl;
    boxm2_ocl_paint_batch::paint_block(scene,device,str_cache,ocl_cache,queue,data_type,bid,n_table);
  }
  std::cout<<"boxm2_ocl_paint_batch_process:: Total time - "<<(float) totalTime.all()/1000.0f<<" sec"<<std::endl;

  return true;
}
