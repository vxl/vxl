// This is brl/bseg/boxm/ocl/pro/processes/boxm_generate_3d_point_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a rendering scene using OpenCL GPU acceleration
//
// \author Vishal Jain
// \date Sept 7, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_update_bit_scene_manager_sptr.h>
#include <boxm/basic/boxm_util_data_types.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
namespace boxm_generate_3d_point_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 3;
}

bool boxm_generate_3d_point_process_cons(bprb_func_process& pro)
{
  using namespace boxm_generate_3d_point_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_update_bit_scene_manager_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";  // i
  input_types_[3] = "unsigned"; // j
  input_types_[4] = "float";  // t

  if (!pro.set_input_types(input_types_))
    return false;

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  for (unsigned i=0;i<3;i++)
    output_types_[i] = "float";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_generate_3d_point_process(bprb_func_process& pro)
{
  using namespace boxm_generate_3d_point_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_update_bit_scene_manager_sptr   mgr = pro.get_input<boxm_update_bit_scene_manager_sptr>(i++);
  vpgl_camera_double_sptr              cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned pi=pro.get_input<unsigned>(i++);
  unsigned pj=pro.get_input<unsigned>(i++);
  float     t=pro.get_input<float>(i++);
  if (vpgl_perspective_camera<double> * cam=dynamic_cast<vpgl_perspective_camera<double>* >(cam_ptr.ptr()))
  {
      vgl_vector_3d<double> dir=cam->backproject(vgl_point_2d<double>((double)pi,(double)pj)).direction();
      dir=normalize(dir);
      vcl_cout<<cam->get_camera_center();
      vgl_point_3d<double> point3d=cam->get_camera_center()+t*dir;
      pro.set_output_val<float>(0, float(point3d.x()));
      pro.set_output_val<float>(1, float(point3d.y()));
      pro.set_output_val<float>(2, float(point3d.z()));
      return true;
  }
  return false;
}
