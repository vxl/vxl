// This is brl/bpro/core/vil_pro/processes/vil_init_byte_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_init_byte_image_process_cons(bprb_func_process& pro)
{
  // input
  vcl_vector<vcl_string> input_types_;
  input_types_.push_back("unsigned");  // ni
  input_types_.push_back("unsigned");  // nj
  input_types_.push_back("unsigned");  // np
  input_types_.push_back("unsigned");  // initial value

  if (!pro.set_input_types(input_types_))
    return false;

  // input default values
  pro.set_input(0, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(1, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(2, brdb_value_sptr(new brdb_value_t<unsigned>(1)));
  pro.set_input(3, brdb_value_sptr(new brdb_value_t<unsigned>(0)));

  // output
  vcl_vector<vcl_string> output_types_;
  output_types_.push_back("vil_image_view_base_sptr");
  return pro.set_output_types(output_types_);
}

//: Execute the process
bool vil_init_byte_image_process(bprb_func_process& pro)
{
  unsigned int num_inputs = pro.input_types().size();
  // Sanity check
  if (pro.n_inputs() != num_inputs) {
    vcl_cout << pro.name() << ": The number of inputs should be " << num_inputs << vcl_endl;
    return false;
  }
  // get the input
  // get the inputs
  unsigned i = 0;
  unsigned int ni = pro.get_input<unsigned>(i++);
  unsigned int nj = pro.get_input<unsigned>(i++);
  unsigned int np = pro.get_input<unsigned>(i++);
  unsigned init_val_in = pro.get_input<unsigned>(i++);
  vxl_byte init_val = (unsigned char)init_val_in;
  vil_image_view<vxl_byte>* img = new vil_image_view<vxl_byte>(ni, nj, np);
  img->fill(init_val);

  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(img));
  return true;
}