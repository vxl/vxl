// This is brl/bseg/boxm/ocl/pro/processes/boxm_ocl_convert_boxm_to_ocl_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for converting boxm scene to boxm ocl scene 
//
// \author Vishal jain
// \date July 8, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/ocl/boxm_ocl_render_expected.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/ocl/boxm_ocl_scene.h>

#include <boxm/ocl/boxm_ocl_utils.h>


namespace boxm_ocl_convert_boxm_to_ocl_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm_ocl_convert_boxm_to_ocl_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_convert_boxm_to_ocl_process_globals;
  // process takes 5 inputs
  // input[0]: scene binary file
  // input[1]: camera
  // input[2]: ni of the expected image
  // input[3]: nj of the expected image
  // input[4]: black background?
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";

  if (!pro.set_input_types(input_types_))
    return false;

  // process has 2 outputs:
  // output[0]: rendered image
  // output[1]: mask
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_ocl_convert_boxm_to_ocl_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_convert_boxm_to_ocl_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);

  // check the scene's appearance model
  switch (scene_ptr->appearence_model())
  {
    case BOXM_APM_MOG_GREY:
    {
        typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_ocl_scene ocl_scene=boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::convert_scene(scene, 10000,10000);
        break;
    }
    case BOXM_APM_SIMPLE_GREY:
    {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
        boxm_ocl_scene ocl_scene=boxm_ocl_convert<boxm_sample<BOXM_APM_SIMPLE_GREY> >::convert_scene(scene, 10000,10000);
        break;
    }
    default:
    {
      vcl_cout << "boxm_ocl_convert_boxm_to_ocl_process: unsupported APM type" << vcl_endl;
      return false;
    }
  }


  return true;
}
