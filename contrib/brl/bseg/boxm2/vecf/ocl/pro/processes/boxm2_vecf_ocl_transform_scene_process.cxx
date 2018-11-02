// This is brl/bseg/boxm2/vecf/ocl/pro/processes/boxm2_vecf_ocl_transform_scene_process.cxx
//:
// \file
// \brief  A process for transforming a scene
//
// \author J.L. Mundy
// \date October 5, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/vecf/ocl/boxm2_vecf_ocl_transform_scene.h>

namespace boxm2_vecf_ocl_transform_scene_process_globals
{
  constexpr unsigned n_inputs_ = 18;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_vecf_ocl_transform_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_vecf_ocl_transform_scene_process_globals;

  //process takes 18 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";     // source scene
  input_types_[1] = "boxm2_scene_sptr";     // target scene
  input_types_[2] = "boxm2_opencl_cache_sptr";
  // inverse transformation -- target to source
  input_types_[3] = "double"; // translation
  input_types_[4] = "double";
  input_types_[5] = "double";
  input_types_[6] = "double";  // rot[0][i]
  input_types_[7] = "double";
  input_types_[8] = "double";
  input_types_[9] = "double";  // rot[1][i]
  input_types_[10] = "double";
  input_types_[11] = "double";
  input_types_[12] = "double"; // rot[2][i]
  input_types_[13] = "double";
  input_types_[14] = "double";
  input_types_[15] = "double"; // scale
  input_types_[16] = "double";
  input_types_[17] = "double";
  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_vecf_ocl_transform_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_vecf_ocl_transform_scene_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  int i = 0;
  // two scenes are stored in the same cache
  boxm2_scene_sptr source_scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_scene_sptr target_scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bocl_device_sptr device = cache->get_device();
  //translation vector
  vgl_vector_3d<double> trans;
  trans.set(pro.get_input<double>(3),
  pro.get_input<double>(4),
            pro.get_input<double>(5));
  //rotation matrix
  i = 6;
  vnl_matrix_fixed<double, 3, 3> R;
  for(int r = 0; r<3; ++r)
    for(int c = 0; c<3; ++c, i++)
      R[r][c] = pro.get_input<double>(i);
  vgl_rotation_3d<double> rot(R);

  //diagonal elements of the scale matrix
  vgl_vector_3d<double> scale;
  scale.set(pro.get_input<double>(15),
          pro.get_input<double>(16),
          pro.get_input<double>(17));

  boxm2_vecf_ocl_transform_scene tscn(source_scene, target_scene, cache);
  //  if(!tscn.transform(rot, trans, scale))
  if(!tscn.transform_1_blk(rot, trans, scale))
    return false;
  pro.set_output_val<boxm2_scene_sptr>(0, target_scene);
  return true;
}
