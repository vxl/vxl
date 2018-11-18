// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_extract_point_cloud_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for extracting a point cloud from a boxm2 scene. The points correspond to the cell centroids of leaf cells in the scene.
//         There is a probability threshold on cells to save for convenience. The points are saved in the BOXM2_POINT datatype.
//
// \author Ali Osman Ulusoy
// \date Mar 21, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

#include <boxm2/cpp/algo/boxm2_extract_point_cloud.h>

namespace boxm2_extract_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_extract_point_cloud_process_cons (bprb_func_process& pro)
{
  using namespace boxm2_extract_point_cloud_process_globals;

  //process takes 3 inputs, no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //prob. threshold
  input_types_[3] = "unsigned"; //deptht of the tree ( 0,1,2,3)


  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);
  brdb_value_sptr default_depth = new brdb_value_t<unsigned int>(3);
  pro.set_input(3, default_depth);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_extract_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_extract_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  auto prob_t = pro.get_input<float>(i++);
  auto depth = pro.get_input<unsigned>(i++);

  return boxm2_extract_point_cloud::extract_point_cloud(scene, cache, prob_t, depth);

}
