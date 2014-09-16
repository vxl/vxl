// This is brl/bpro/core/brip_pro/processes/brip_truncate_nitf_bit_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  process to ignored desired bits from the vxl_unit_16 nitf images
// \verbatim
//  Modifications
//
// \endverbatim

#include <brip/brip_vil_nitf_ops.h>
#include <vil/vil_image_view_base.h>



//: global variables
namespace brip_truncate_nitf_bit_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

//: constructor
bool brip_truncate_nitf_bit_process_cons(bprb_func_process& pro)
{
  using namespace brip_truncate_nitf_bit_process_globals;
  // input
  vcl_vector<vcl_string> input_types(n_inputs_);
  input_types[0] = "vil_image_view_base_sptr";    // input vxl_unit_16 image
  input_types[1] = "bool";                        // truncation option
  // output
  vcl_vector<vcl_string> output_types(n_outputs_);
  output_types[0] = "vil_image_view_base_sptr";  // output truncated image
  
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: execute the process
bool brip_truncate_nitf_bit_process(bprb_func_process& pro)
{
  using namespace brip_truncate_nitf_bit_process_globals;

  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  vil_image_view_base_sptr in_img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
  bool is_short = pro.get_input<bool>(i++);

  if (in_img_ptr->pixel_format() != VIL_PIXEL_FORMAT_UINT_16) {
    vcl_cout << pro.name() << ": Unsupported Pixel Format = " << in_img_ptr->pixel_format() << vcl_endl;
    return false;
  }

  vil_image_view<vxl_uint_16>* in_img = static_cast<vil_image_view<vxl_uint_16>*>(in_img_ptr.as_pointer());
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  unsigned np = in_img->nplanes();
  // being truncation
  vcl_cout<<"is_short "<<is_short<<vcl_endl;
  if (is_short) {
    
    // truncate the input 16 bits image to a byte image by ignoring the most significant 5 bits and less significant 3 bits
    vil_image_view<vxl_byte>* out_img = new vil_image_view<vxl_byte>(ni,nj,np);
    if (!brip_vil_nitf_ops::scale_nitf_bits(*in_img, *out_img)) {
      vcl_cout << pro.name() << ": truncate image from " << in_img_ptr->pixel_format() << " to " << out_img->pixel_format() << " failed!" << vcl_endl;
      return false;
    }
    // output
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
    return true;
  }
  else {
    // truncate the input image by ignoring the most significant 5 bits
    vil_image_view<vxl_uint_16>* out_img = new vil_image_view<vxl_uint_16>(ni, nj, np);
    if (!brip_vil_nitf_ops::truncate_nitf_bits(*in_img, *out_img)) {
      vcl_cout << pro.name() << ": truncating image from " << in_img->pixel_format() << " to " << out_img->pixel_format() << " failed!" << vcl_endl;
      return false;
    }
    // output
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
    return true;
  }
}
