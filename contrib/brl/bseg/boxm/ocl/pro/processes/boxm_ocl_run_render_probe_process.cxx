// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_run_render_probe_process.cxx
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
#include <boxm/ocl/boxm_render_probe_manager.h>
#include <boxm/ocl/boxm_render_probe_manager_sptr.h>
#include <boxm/basic/boxm_util_data_types.h>
namespace boxm_ocl_run_render_probe_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm_ocl_run_render_probe_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_run_render_probe_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_render_probe_manager_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm_array_1d_float_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;

}

bool boxm_ocl_run_render_probe_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_run_render_probe_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_render_probe_manager_sptr scene_mgr = pro.get_input<boxm_render_probe_manager_sptr>(i++);
  vpgl_camera_double_sptr              cam_ptr = pro.get_input<vpgl_camera_double_sptr>(i++);

  unsigned pi=pro.get_input<unsigned>(i++);
  unsigned pj=pro.get_input<unsigned>(i++);
  if(vpgl_perspective_camera<double> * cam=dynamic_cast<vpgl_perspective_camera<double>* >(cam_ptr.ptr()))
  {
      scene_mgr->set_ij((int)pi,(int)pj);
      scene_mgr->set_persp_camera(cam);
      scene_mgr->write_persp_camera_buffers();
      scene_mgr->set_args();
      scene_mgr->run();
      scene_mgr->read_output_array();
      vcl_vector<float> out;
      scene_mgr->getoutputarray(out);
      boxm_array_1d_float_sptr array_sptr=new boxm_array_1d_float(out.size());
      for (unsigned i=0;i<out.size();i++)
      {
          array_sptr->data_array[i]=out[i];
      }
      pro.set_output_val<boxm_array_1d_float_sptr>(0, array_sptr);

      return true;
  }
  return false;
}
