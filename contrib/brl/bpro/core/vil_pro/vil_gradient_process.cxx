// This is brl/bpro/core/vil_pro/vil_gradient_process.cxx
#include "vil_gradient_process.h"
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>

//: Constructor
vil_gradient_process::vil_gradient_process()
{
  //input
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0]="vil_image_view_base_sptr"; // input image
  
  //output
  output_data_.resize(3,brdb_value_sptr(0));
  output_types_.resize(3);
  output_types_[0]= "vil_image_view_base_sptr"; // dI/dx
  output_types_[1]= "vil_image_view_base_sptr"; // dI/dy
  output_types_[2]= "vil_image_view_base_sptr"; // Sqrt((dI/dx)^2 + (dI/dy)^2)
}


//: Destructor
vil_gradient_process::~vil_gradient_process()
{
}


//: Execute the process
bool
vil_gradient_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr input_image = input0->value();

  // convert image to float
  vil_image_view<float> fimage = *vil_convert_cast(float(), input_image);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();
  vil_image_view<float> Ix(ni, nj, np);
  vil_image_view<float> Iy(ni, nj, np);
  vil_image_view<float> mag(ni, nj, np);
  for(unsigned p = 0; p<np; ++p)
    {
      //extract plane
      vil_image_view<float> temp(ni, nj), gx(ni, nj), gy(ni,nj);
      for(unsigned j = 0; j<nj; ++j)
        for(unsigned i = 0; i<ni; ++i)
          temp(i,j)=static_cast<float>(fimage(i,j,p));

      //      vil_sobel_3x3(temp, gx, gy);
      float scale = 1.0f/6.0f;
      for(int j = 0; j<static_cast<int>(nj); ++j)
        for(int i = 0; i<static_cast<int>(ni); ++i){

          if(i==0||j ==0||i==(ni-1)||j ==(nj-1)){
            Ix(i,j,p) = 0.0f; Iy(i,j,p) = 0.0f; mag(i,j,p) = 0.0f;
            continue;
          }

          float gx = temp(i+1,j-1)+temp(i+1,j)+ temp(i+1,j-1)
            -temp(i-1,j-1) -temp(i-1,j) -temp(i-1,j-1);

          float gy = temp(i+1,j+1)+temp(i,j+1)+ temp(i-1,j+1)
            -temp(i+1,j-1) -temp(i,j-1) -temp(i-1,j-1);

          Ix(i,j,p) = gx*scale;
          Iy(i,j,p) = gy*scale;
          mag(i,j,p) = vcl_sqrt(gx*gx+gy*gy)*scale;
        }
    }
  
  brdb_value_sptr output0 = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(Ix));
  output_data_[0] = output0;

  brdb_value_sptr output1 = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(Iy));
  output_data_[1] = output1;

  brdb_value_sptr output2 = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(mag));
  output_data_[2] = output2;

  return true;
}

