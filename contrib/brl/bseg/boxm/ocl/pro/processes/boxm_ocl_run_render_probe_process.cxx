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
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
namespace boxm_ocl_run_render_probe_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 11;
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
  input_types_[4] = "float";

  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  for(unsigned i=0;i<10;i++)
    output_types_[i] = "bbas_1d_array_float_sptr";
  output_types_[10]="float";
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
  float intensity=pro.get_input<float>(i++);
  if(vpgl_perspective_camera<double> * cam=dynamic_cast<vpgl_perspective_camera<double>* >(cam_ptr.ptr()))
  {
      scene_mgr->set_ij((int)pi,(int)pj);
      scene_mgr->set_intensity(intensity);
      scene_mgr->set_persp_camera(cam);
      scene_mgr->write_persp_camera_buffers();
      scene_mgr->set_args();
      scene_mgr->run();
      scene_mgr->read_output_array();
      vcl_vector< vcl_vector<float> > out;
      scene_mgr->getoutputarray(out);
      bbas_1d_array_float_sptr array_sptr[10];
      for (unsigned i=0;i<out.size();i++)
      {
          array_sptr[i]=new bbas_1d_array_float(out[i].size());      
          for(unsigned j=0;j<out[i].size();j++)
             array_sptr[i]->data_array[j]=out[i][j];
      }
      float output=scene_mgr->get_output();
      for (unsigned i=0;i<out.size();i++)
          pro.set_output_val<bbas_1d_array_float_sptr>(i, array_sptr[i]);
      pro.set_output_val<float>(10,output);
      return true;
  }
  return false;
}
