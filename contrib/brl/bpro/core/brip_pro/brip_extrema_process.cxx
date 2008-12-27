// This is brl/bpro/core/brip_pro/brip_extrema_process.cxx
#include "brip_extrema_process.h"
//:
// \file

#include <brip/brip_vil_float_ops.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_image_view_base.h>

//: Constructor
brip_extrema_process::brip_extrema_process()
{
  //input
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0]="vil_image_view_base_sptr"; // input image
  input_types_[1]="float"; // lambda0
  input_types_[2]="float"; // lambda1
  input_types_[3]="float"; // theta
  input_types_[4]="bool"; // bright = true, dark = false
  input_types_[5]="bool"; // use fast algorithm = true


  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr"; // point response
  output_types_[1]= "vil_image_view_base_sptr"; // kernel domain response

}


//: Destructor
brip_extrema_process::~brip_extrema_process()
{
}


//: Execute the process
bool
brip_extrema_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image_ptr = input0->value();
  vil_image_resource_sptr resc = vil_new_image_resource_of_view(*image_ptr);
  //lambda0
  brdb_value_t<float>* input1 =
    static_cast<brdb_value_t<float>* >(input_data_[1].ptr());
  float lambda0 = input1->value();

  //lambda1
  brdb_value_t<float>* input2 =
    static_cast<brdb_value_t<float>* >(input_data_[2].ptr());
  float lambda1 = input2->value();

  //theta
  brdb_value_t<float>* input3 =
    static_cast<brdb_value_t<float>* >(input_data_[3].ptr());
  float theta = input3->value();

  //bright vs. dark
  brdb_value_t<bool>* input4 =
    static_cast<brdb_value_t<bool>* >(input_data_[4].ptr());
  bool bright = input4->value();

  //fast or composed
  brdb_value_t<bool>* input5 =
    static_cast<brdb_value_t<bool>* >(input_data_[5].ptr());
  bool fast = input5->value();

  // convert input image to float on range 0-1
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(resc);
  if (image_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();
  vil_image_view<float> gimage;
  if(np>1){
    gimage.set_size(ni,nj);
    for(unsigned j = 0; j<nj; ++j)
      for(unsigned i = 0; i<ni; ++i)
        {
          float v = 0;
          for(unsigned p = 0; p<np; ++p)
            v += fimage(i,j,p);
          gimage(i,j) = v/np;
        }
  }
  else
    gimage = fimage;

  vil_image_view<float> out;
  if(fast)
    out = brip_vil_float_ops::fast_extrema(gimage, lambda0, lambda1,
                                           theta, bright, true);
  else
    out = brip_vil_float_ops::extrema(gimage, lambda0, lambda1,
                                      theta, bright, true);
  vil_image_view<float>* point = new vil_image_view<float>(ni, nj); 
  vil_image_view<float>* mask = new vil_image_view<float>(ni, nj); 
  
  for(unsigned j = 0; j<nj; ++j)
      for(unsigned i = 0; i<ni; ++i)
        {
          (*point)(i,j) = out(i,j,0);
          (*mask)(i,j) = out(i,j,1);
        }
  brdb_value_sptr output0 = 
    new brdb_value_t<vil_image_view_base_sptr>(point);
  output_data_[0] = output0;

  brdb_value_sptr output1 = 
    new brdb_value_t<vil_image_view_base_sptr>(mask);
  output_data_[1] = output1;

  return true;
}

