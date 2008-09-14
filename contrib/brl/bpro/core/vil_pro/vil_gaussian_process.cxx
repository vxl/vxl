// This is brl/bpro/core/vil_pro/vil_gaussian_process.cxx
#include "vil_gaussian_process.h"
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>

//: Constructor
vil_gaussian_process::vil_gaussian_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vil_image_view_base_sptr"; // input image
  input_types_[1]="double"; // gaussian standard deviation
  
  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr"; // gauss smoothed image(float)

}


//: Destructor
vil_gaussian_process::~vil_gaussian_process()
{
}


//: Execute the process
bool
vil_gaussian_process::execute()
{

  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr input_image = input0->value();

  //Gaussian standard deviation
  brdb_value_t<double>* input1 =
    static_cast<brdb_value_t<double>* >(input_data_[1].ptr());
  double sigma = input1->value();

  // convert input image to float
  vil_image_view<float> fimage = *vil_convert_cast(float(), input_image);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();

  vil_image_view<float> G(ni, nj, np);
  for(unsigned p = 0; p<np; ++p)
    {
      //extract plane
      vil_image_view<float> temp(ni, nj), g(ni,nj);
      for(unsigned j = 0; j<nj; ++j)
        for(unsigned i = 0; i<ni; ++i)
          temp(i,j)=static_cast<float>(fimage(i,j,p));

      vil_gauss_filter_2d(temp, g, sigma, 3);
      for(unsigned j = 0; j<nj; ++j)
        for(unsigned i = 0; i<ni; ++i)
          G(i,j,p) = g(i,j);
    }

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(G));
  output_data_[0] = output0;

  return true;
}

