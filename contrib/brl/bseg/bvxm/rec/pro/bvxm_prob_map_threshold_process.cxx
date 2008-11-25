// This is brl/bseg/bvxm/rec/pro/bvxm_prob_map_threshold_process.cxx
#include "bvxm_prob_map_threshold_process.h"
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
bvxm_prob_map_threshold_process::bvxm_prob_map_threshold_process()
{
  //input
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask
  input_types_[2]= "vil_image_view_base_sptr"; //orig_view
  input_types_[3]= "float"; //threshold

  //output - no outputs as yet
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0] = "vil_image_view_base_sptr";
}

//: Execute the process
bool
bvxm_prob_map_threshold_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bvxm_prob_map_threshold_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr temp = input0->value();
  vil_image_view<float> frame = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = frame.ni(), nj = frame.nj();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  temp = input1->value();
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  temp = input2->value();
  vil_image_view<vxl_byte> input_img(temp);

  brdb_value_t<float>* input3 = static_cast<brdb_value_t<float>* >(input_data_[3].ptr());
  float thres = input3->value();

  vil_image_view<vxl_byte> out(ni, nj, 3);
  out.fill(0);

  int count = 0;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        out(i,j,0)=input_img(i,j);
        out(i,j,1)=input_img(i,j);
        out(i,j,2)=input_img(i,j);
        if (frame(i,j)<=thres) {
          out(i,j,0) = 255;
          count++;
        }
      }
    }

  vcl_cout << "----------------------------------\n"
           << "\tthres: " << thres << '\n'
           << "\tnumber of CHANGE PIXELS: " << count << " out of " << ni*nj << " pixels: %" << ((float)count/(ni*nj))*100.0f << vcl_endl
           << "----------------------------------\n";
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out));
  output_data_[0] = output0;

  return true;
}

