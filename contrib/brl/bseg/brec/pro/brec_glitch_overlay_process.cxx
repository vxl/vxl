// This is brl/bseg/brec/pro/brec_glitch_overlay_process.cxx
#include "brec_glitch_overlay_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl2_pro/vidl2_pro_utils.h>
#include <brec/brec_glitch.h>

//: Constructor
brec_glitch_overlay_process::brec_glitch_overlay_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input img
  input_types_[2]= "unsigned"; // size of the inner-square for the glitch mask (e.g. 5 means we're detecting foreground islands of 5x5 on background)

  //output
  output_data_.resize(3, brdb_value_sptr(0));
  output_types_.resize(3);
  output_types_[0] = "vil_image_view_base_sptr";  // output float overlayed glitch map
  output_types_[1] = "vil_image_view_base_sptr";  // output float overlayed glitch map as stretched to byte img
  output_types_[2] = "vil_image_view_base_sptr";  // output the glitch map as overlayed on input img
}

//: Execute the process
bool
brec_glitch_overlay_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In brec_glitch_overlay_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr temp = input0->value();
  vil_image_view<float> map = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = map.ni(), nj = map.nj();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr img = input1->value();
  if (img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> input_img(img);

  brdb_value_t<unsigned>* input2 = static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned c_size = input2->value();  // center size

  vil_image_view<float> out(ni, nj, 1);
  brec_glitch::extend_prob_to_square_region(c_size, map, out);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(out));
  output_data_[0] = output0;

  vil_image_view<vxl_byte> out2(ni, nj, 3);
  if (input_img.nplanes() == 3) {
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        out2(i,j,0) = input_img(i,j,0);
        out2(i,j,1) = input_img(i,j,1);
        out2(i,j,2) = (vxl_byte)(vcl_floor(out(i,j)*255.0f+0.5f));
      }
  } else {
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        out2(i,j,0) = input_img(i,j,0);
        out2(i,j,1) = input_img(i,j,0);
        out2(i,j,2) = (vxl_byte)(vcl_floor(out(i,j)*255.0f+0.5f));
      }
  }

  float min, max;
  vil_math_value_range(out, min, max);
  vcl_cout << "\t glitch map overlayed min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> out_byte(ni, nj, 1);
  vil_convert_stretch_range_limited(out, out_byte, 0.0f, max);

  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte));
  output_data_[1] = output1;
  
  brdb_value_sptr output2 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out2));
  output_data_[2] = output2;

  return true;
}

