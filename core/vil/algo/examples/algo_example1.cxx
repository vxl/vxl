//:
// \file
// \brief Example of computing gradients of a vil_image_view<T>.
// \author Tim Cootes - Manchester

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_sobel_3x3.h>

int main()
{
  unsigned ni=8;
  unsigned nj=15;
  unsigned nplanes=1;
  vil_image_view<vxl_byte> image(ni,nj,nplanes);

  for (unsigned p=0;p<nplanes;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j,p) = vxl_byte(i+10*j+100*p);

  std::cout<<"Original image:\n";
  vil_print_all(std::cout,image);

    // Objects to hold gradients
  vil_image_view<float> grad_i,grad_j;

  vil_sobel_3x3(image,grad_i,grad_j);

  std::cout<<"\nSobel I Gradient:\n";
  vil_print_all(std::cout,grad_i);

  std::cout<<"\nSobel J Gradient:\n";
  vil_print_all(std::cout,grad_j);

  return 0;
}
