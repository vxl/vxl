// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_minfo_process.cxx
//:
// \file
// \brief  A process computing the MI between two regions in the 4d world. The fixed region is specified with the center of the
//         box and its dimensions. The relative position of the second regions is specified with rotation (Rodrigues) vector (R_x,R_y,R_z)
//         and translation (T_x,T_y,T_z). Also, the time frames in which to evaluate these regions must be specified.
//         A current limitation is that we assume the time frames are contained in the same time block.
//
// \author Ali Osman Ulusoy
// \date Jan 30, 2013

//directory utility
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vcl_where_root_dir.h>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <brdb/brdb_value.h> //brdb stuff
#include <bstm/ocl/algo/bstm_ocl_minfo_function.h>



namespace bstm_ocl_minfo_process_globals
{
  constexpr unsigned n_inputs_ = 18;
  constexpr unsigned n_outputs_ = 1;
}

bool bstm_ocl_minfo_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_minfo_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "float"; //center x
  input_types_[4] = "float"; //center y
  input_types_[5] = "float"; //center z
  input_types_[6] = "float"; //len x
  input_types_[7] = "float"; //len y
  input_types_[8] = "float"; //len z
  input_types_[9] = "float";  //R_x
  input_types_[10] = "float"; //R_y
  input_types_[11] = "float"; //R_z
  input_types_[12] = "float"; //T_x
  input_types_[13] = "float"; //T_y
  input_types_[14] = "float"; //T_z
  input_types_[15] = "float"; //time frame for fixed template
  input_types_[16] = "float"; //time frame for relative template
  input_types_[17] = "int"; //number of bins to quantize histogram

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_minfo_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_minfo_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_opencl_cache_sptr opencl_cache= pro.get_input<bstm_opencl_cache_sptr>(i++);
  auto center_x = pro.get_input<float>(i++);
  auto center_y = pro.get_input<float>(i++);
  auto center_z = pro.get_input<float>(i++);
  auto len_x = pro.get_input<float>(i++);
  auto len_y = pro.get_input<float>(i++);
  auto len_z = pro.get_input<float>(i++);
  auto R_x = pro.get_input<float>(i++);
  auto R_y = pro.get_input<float>(i++);
  auto R_z = pro.get_input<float>(i++);
  auto T_x = pro.get_input<float>(i++);
  auto T_y = pro.get_input<float>(i++);
  auto T_z = pro.get_input<float>(i++);
  auto time_1 = pro.get_input<float>(i++);
  auto time_2 = pro.get_input<float>(i++);
  int nbins = pro.get_input<int>(i++);

  //get scene data type and appTypeSize
  std::string data_type;
  int apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    std::cout<<"bstm_ocl_minfo_process ERROR: scene doesn't have BSTM_MOG6_VIEW_COMPACT data type"<<std::endl;
    return false;
  }
  std::string options = bstm_ocl_util::mog_options(data_type);

  //construct R,T
  vnl_vector_fixed<double,3> rot_vector (R_x,R_y,R_z);
  vgl_rotation_3d<double> R(rot_vector);
  vgl_vector_3d<double> T (T_x,T_y,T_z);

  //construct bounding box
  vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> bb(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);

  bstm_ocl_minfo_function fn(device,scene,opencl_cache,bb,R,T, time_1, time_2, options, nbins);
  float mi = fn.evaluate();

  int argIdx = 0;
  pro.set_output_val<float>(argIdx, mi);
  return true;
}
