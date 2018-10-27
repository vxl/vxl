// This is brl/bseg/bstm/pro/processes/bstm_describe_scene_process.cxx
//:
// \file
// \brief  A process for descibing a BSTM scene
//
// \author Raphael Kargon
// \date Jul 11, 2017

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#include <bstm/bstm_scene.h>
#include <bstm/bstm_util.h>
#include <bstm/bstm_data_traits.h>

namespace bstm_describe_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 3;
}

bool bstm_describe_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_describe_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";

  // process has 2 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vcl_string";                    //path to model data
  output_types_[1] = "vcl_string";                    //appearance model type
  output_types_[2] = "double";                        // also compute and return voxel size in LVCS units // assumes cube voxels
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_describe_scene_process(bprb_func_process& pro)
{
  using namespace bstm_describe_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(0);
  if (!scene){
    std::cout << " null scene in bstm_describe_scene_process\n";
    return false;
  }
  //std::cout << *scene;

  //verifies that a scene has a valid appearance, spits out data type and appearance type size
  // NOTE: some data traits are commented out because they don't work (yet) on BSTM
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  // valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY_16>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW>::prefix());
  // valid_types.push_back(bstm_data_traits<BSTM_GAUSS_UV_VIEW>::prefix());
  std::string data_type;
  int appTypeSize;
  bstm_util::verify_appearance(*scene, valid_types, data_type, appTypeSize);
  std::cout<<"DATA_TYPE:"<<data_type<<std::endl;

  // compute voxel size using local bounding box
  vgl_box_3d<double> bbox = scene->bounding_box();

  // obtain the scene finest resolution
  // note that the sub block size is truncated to integer here
  std::map<bstm_block_id, bstm_block_metadata> blks = scene->blocks();
  double res_x = 1E5, res_y = 1E5;
  for (auto & blk : blks)
  {
    // only use x dimension, assumes cube voxels so it shouldn't matter which dimension to use
    double voxel_size_x = (blk.second.sub_block_dim_.x()) / (1 << (blk.second.max_level_ - blk.second.init_level_));
    if (res_x > voxel_size_x) res_x = voxel_size_x;
  }

  //set model dir as output
  std::string dataPath = scene->data_path();
  pro.set_output_val<std::string>(0, dataPath);
  pro.set_output_val<std::string>(1, data_type);
  pro.set_output_val<double>(2,res_x);
  return true;
}
