// This is brl/bpro/core/vil_pro/vil_image_pair_process.cxx
#include "vil_image_pair_process.h"
//:
// \file

#include "vil_math_functors.h"
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
vil_image_pair_process::vil_image_pair_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vil_image_view_base_sptr";
  input_types_[1]="vil_image_view_base_sptr";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
vil_image_pair_process::~vil_image_pair_process()
{
}


//: Execute the process
bool
vil_image_pair_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve images from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr image0 = input0->value();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr image1 = input1->value();

  unsigned ni0 = image0->ni(), nj0 = image0->nj();
  unsigned ni1 = image1->ni(), nj1 = image1->nj();
  unsigned ni = ni0, nj = nj0;
  if(ni>ni1) ni = ni1;
  if(nj>nj1) nj = nj1;
  unsigned separating_bar = 5;
  unsigned nip = 2*ni + separating_bar;
  //only implement for byte grey scale and byte color

  vil_pixel_format fmt0 = image0->pixel_format(); 
  vil_pixel_format fmt1 = image1->pixel_format(); 
  if(fmt0!=fmt1)
    return false;
  vil_image_view_base_sptr out_image;
  switch(fmt0)
    {
    case  VIL_PIXEL_FORMAT_BYTE:
      {
        vil_image_view<vxl_byte> bimage0 = 
          vil_convert_cast(vxl_byte(), image0);

        vil_image_view<vxl_byte> bimage1 = 
          vil_convert_cast(vxl_byte(), image1);
        
        vil_image_view<vxl_byte>* comb = 
          new vil_image_view<vxl_byte>(nip, nj);

        for(unsigned j=0; j<nj; ++j){
          for(unsigned i=0; i<ni; ++i){
            (*comb)(i,j) = bimage0(i, j);
            (*comb)(ni+i+separating_bar,j) = bimage1(i, j);
          }
          for(unsigned i=ni; i<ni+separating_bar; ++i)
            (*comb)(i,j) = static_cast<vxl_byte>(255);
        }
        out_image = comb;
		break;
      }
    case VIL_PIXEL_FORMAT_RGB_BYTE:
      {
        vil_image_view<vil_rgb<vxl_byte> > cimage0 = 
          vil_convert_cast(vil_rgb<vxl_byte>(), image0);

        vil_image_view<vil_rgb<vxl_byte> > cimage1 = 
          vil_convert_cast(vil_rgb<vxl_byte>(), image1);

        vil_image_view<vil_rgb<vxl_byte> >* comb = 
          new vil_image_view<vil_rgb<vxl_byte> >(nip, nj);

        for(unsigned j=0; j<nj; ++j){
          for(unsigned i=0; i<ni; ++i){
            (*comb)(i,j) = cimage0(i, j);
            (*comb)(ni+i+separating_bar,j) = cimage1(i, j);
          }
          for(unsigned i=ni; i<ni+separating_bar; ++i)
            (*comb)(i,j) = vil_rgb<vxl_byte>(0, 255,0);
        }
        out_image = comb;
		break;
      }
    default:
      return false;
    }

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(out_image);
  output_data_[0] = output0;
  return true;
}

