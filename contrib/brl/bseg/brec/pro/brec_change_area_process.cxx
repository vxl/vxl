// This is brl/bseg/brec/pro/brec_change_area_process.cxx
#include "brec_change_area_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl2_pro/vidl2_pro_utils.h>


//: Constructor
brec_change_area_process::brec_change_area_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask

  //output
  output_data_.resize(2, brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0] = "float";  // expected area
  output_types_[1] = "float";  // expected area as a percentage of the total number of pixels
}

//: Execute the process
bool
brec_change_area_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In brec_change_area_process::execute() -"
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
  temp = input1->value();
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  vil_image_view<float> out(ni, nj, 1);
  out.fill(0);

  float sum = 0.0f;
  float count = 0.0f;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        sum += map(i,j);
        count += 1.0f;
      }
    }

  brdb_value_sptr output0 = new brdb_value_t<float>(sum);
  output_data_[0] = output0;

  brdb_value_sptr output1 = new brdb_value_t<float>((sum/count)*100.0f);
  output_data_[1] = output1;

  return true;
}

