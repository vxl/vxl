//:
// \file
// \brief Example of computing gradients of a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_print.h>
#include <vil2/algo/vil2_algo_sobel_3x3.h>

int main(int argc, char** argv)
{
  unsigned ni=8;
  unsigned nj=15;
  unsigned nplanes=1;
  vil2_image_view<vxl_byte> image(ni,nj,nplanes);

  // Slow fill
  for (unsigned p=0;p<nplanes;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j,p) = vxl_byte(i+10*j+100*p);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

    // Objects to hold gradients
  vil2_image_view<float> grad_i,grad_j;

  vil2_algo_sobel_3x3(image,grad_i,grad_j);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Sobel I Gradient:"<<vcl_endl;
  vil2_print_all(vcl_cout,grad_i);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Sobel J Gradient:"<<vcl_endl;
  vil2_print_all(vcl_cout,grad_j);

  return 0;
}
