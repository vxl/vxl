// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_refine_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for refining a scene using OpenCL GPU acceleration
//
// \author Andrew Miller
// \date June 16, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/ocl/boxm_ocl_refine_scene.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>


namespace boxm_ocl_refine_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm_ocl_refine_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_refine_process_globals;
  // process takes 5 inputs
  // input[0]: scene binary file
  // input[1]: camera
  // input[2]: ni of the expected image
  // input[3]: nj of the expected image
  // input[4]: black background?
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";

  if (!pro.set_input_types(input_types_))
    return false;

  // process has 2 outputs:
  // output[0]: rendered image
  // output[1]: mask
  //vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "vil_image_view_base_sptr";
  //output_types_[1] = "vil_image_view_base_sptr";
  //if (!pro.set_output_types(output_types_))
  //  return false;

  return true;
}

bool boxm_ocl_refine_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_refine_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  float prob_thresh = pro.get_input<float>(i++);

  // check the scene's appearance model
  switch (scene_ptr->appearence_model())
  {
    case BOXM_APM_MOG_GREY:
    {
      if (!scene_ptr->multi_bin())
      {
        typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_ocl_refine_scene<BOXM_APM_MOG_GREY>(scene, prob_thresh);
      }
      else
      {
        vcl_cout<<"OpenCL rendering of multi-bin scenes not implemented"<<vcl_endl;
        return false;
      }
      break;
    }
    case BOXM_APM_SIMPLE_GREY:
    {
      if (!scene_ptr->multi_bin()) {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_ocl_refine_scene<BOXM_APM_SIMPLE_GREY>(scene, prob_thresh);
      }
      else
      {
        vcl_cout<<"OpenCL rendering of multi-bin scenes not implemented"<<vcl_endl;
        return false;
      }
      break;
    }
    default:
    {
      vcl_cout << "boxm_ocl_render_expected_process: unsupported APM type" << vcl_endl;
      return false;
    }
  }
  return true;
}
