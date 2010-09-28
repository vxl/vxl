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

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_ocl_utils.h>


namespace boxm_ocl_convert_boxm_to_ocl_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm_ocl_convert_boxm_to_ocl_process_cons(bprb_func_process& pro)
{
  using namespace boxm_ocl_convert_boxm_to_ocl_process_globals;
  // process takes 4 inputs and no outputs
  // input[0]: inout scene binary file
  // input[1]: output scene dir
  // input[2]: max megabytes for OCL_Scene
  // input[3]: boolean (true = bit tree, false = ocl_scene)
  vcl_vector<vcl_string> input_types_(n_inputs_);
  vcl_vector<vcl_string> output_types_(n_outputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "int";
  input_types_[3] = "bool";

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm_ocl_convert_boxm_to_ocl_process(bprb_func_process& pro)
{
  using namespace boxm_ocl_convert_boxm_to_ocl_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);
  int max_mb = pro.get_input<int>(i++);
  bool bit_tree = pro.get_input<bool>(i++);

  // check the scene's appearance model
  switch (scene_ptr->appearence_model())
  {
    case BOXM_APM_MOG_GREY:
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());

      //convert
      if(bit_tree) {
        boxm_ocl_bit_scene bit_scene;
        boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::convert_bit_scene(scene, bit_scene, max_mb);
        vcl_cout<<bit_scene<<vcl_endl;
        bit_scene.save_scene(output_dir);
      }
      else {
        boxm_ocl_scene ocl_scene;
        boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::convert_scene(scene, ocl_scene, max_mb);
        vcl_cout<<ocl_scene<<vcl_endl;
        ocl_scene.save_scene(output_dir);
      }
      break;
    }
    case BOXM_APM_SIMPLE_GREY:
    {
        typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
        boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());

      if(bit_tree) {
        boxm_ocl_bit_scene bit_scene;
        boxm_ocl_convert<boxm_sample<BOXM_APM_SIMPLE_GREY> >::convert_bit_scene(scene, bit_scene, max_mb);
        vcl_cout<<bit_scene<<vcl_endl;
        bit_scene.save_scene(output_dir);
      }
      else {
        boxm_ocl_scene ocl_scene;
        boxm_ocl_convert<boxm_sample<BOXM_APM_SIMPLE_GREY> >::convert_scene(scene, ocl_scene, max_mb);
        vcl_cout<<ocl_scene<<vcl_endl;
        ocl_scene.save_scene(output_dir);
      }
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
