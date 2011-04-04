// This is brl/bpro/core/vil_pro/processes/vil_rgb_to_grey_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//:global variables
namespace vil_rgb_to_grey_process_globals
{
	const unsigned n_inputs_ = 1;
	const unsigned n_outputs_ = 1;
}

//: Constructor
bool vil_rgb_to_grey_process_cons(bprb_func_process& pro)
{
	using namespace vil_rgb_to_grey_process_globals;

	//this process takes one input:
	//input (0): the vil_image_view_base_sptr
	vcl_vector<vcl_string> input_types_(n_inputs_);
	input_types_[0] = "vil_image_view_base_sptr";
	

	//this process takes one output:
	//output (0): the vil_image_view_base_sptr
	vcl_vector<vcl_string> output_types_(n_outputs_);
	output_types_[0] = "vil_image_view_base_sptr";

	return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vil_rgb_to_grey_process(bprb_func_process& pro)
{
	using namespace vil_rgb_to_grey_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr input_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  

  vil_image_view_base_sptr output_sptr = vil_convert_to_grey_using_rgb_weighting(input_sptr);

  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++,output_sptr);
  return true;
}