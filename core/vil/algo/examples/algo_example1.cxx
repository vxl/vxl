//:
// \file
// \brief Example of computing gradients of a vil_image_view<T>.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
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

  vcl_cout<<"Original image:\n";
  vil_print_all(vcl_cout,image);

    // Objects to hold gradients
  vil_image_view<float> grad_i,grad_j;

  vil_sobel_3x3(image,grad_i,grad_j);

  vcl_cout<<"\nSobel I Gradient:\n";
  vil_print_all(vcl_cout,grad_i);

  vcl_cout<<"\nSobel J Gradient:\n";
  vil_print_all(vcl_cout,grad_j);

  return 0;
}
