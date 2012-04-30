// This is brl/bpro/core/vil_pro/processes/vil_init_float_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_init_float_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("unsigned");//ni 
  input_types.push_back("unsigned");//nj
  input_types.push_back("unsigned");//np
  input_types.push_back("float");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //default values
  pro.set_input(0, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(1, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(2, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(3, brdb_value_sptr(new brdb_value_t<float>(0.0)));

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_init_float_image_process(bprb_func_process& pro)
{
  unsigned int num_inputs = pro.input_types().size();
  // Sanity check
  if (pro.n_inputs() != num_inputs) {
    vcl_cout << "vil_load_image_view_binary_process: The number of inputs should be " << num_inputs << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  unsigned int ni = pro.get_input<unsigned>(i++);
  unsigned int nj = pro.get_input<unsigned>(i++);
  unsigned int np = pro.get_input<unsigned>(i++);
  float init_val = pro.get_input<float>(i++);
 
  vil_image_view<float> *img = new vil_image_view<float>(ni,nj,np);
  img->fill(init_val);
  
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(img));
 
  return true;
}

