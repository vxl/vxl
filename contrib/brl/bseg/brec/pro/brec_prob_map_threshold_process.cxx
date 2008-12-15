// This is brl/bseg/brec/pro/brec_prob_map_threshold_process.cxx
#include "brec_prob_map_threshold_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


//: Constructor
brec_prob_map_threshold_process::brec_prob_map_threshold_process()
{
  //input
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask
  input_types_[2]= "vil_image_view_base_sptr"; //orig_view
  input_types_[3]= "float"; //threshold

  //output - no outputs as yet
  output_data_.resize(2, brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0] = "vil_image_view_base_sptr";   // output a thresholded image where red channel is binarized wrt given threshold
  output_types_[1] = "vil_image_view_base_sptr";   // output a thresholded image where red channel is mapped wrt prob
}

//: Execute the process
bool
brec_prob_map_threshold_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In brec_prob_map_threshold_process::execute() -"
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
  vil_image_view<vxl_byte> out2(ni, nj, 3);
  out2.fill(0);


  int count = 0;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        out(i,j,0)=input_img(i,j);
        out(i,j,1)=input_img(i,j);
        out(i,j,2)=input_img(i,j);
        
        out2(i,j,0)=input_img(i,j);
        out2(i,j,1)=input_img(i,j);
        out2(i,j,2)=input_img(i,j);
        
        if (frame(i,j)<=thres) {
          out(i,j,0) = 255;
          float change = 1-frame(i,j); // invert input map
          //: assuming that change is in [0,1] where 0 is no change and 1 is change
          //  we set red channel so that: on the threshold value it becomes 255*0.01
          //  and it increases exponentially till 1 when it becomes 255
          out2(i,j,0) = (vxl_byte)(255*vcl_exp((vcl_log(0.01)/(thres-1))*(change-1)));
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

  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out2));
  output_data_[1] = output1;

  return true;
}

