//:
// \file
// \brief Example of computing gradients of a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/algo/vil2_algo_sobel_3x3.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>

int main(int argc, char** argv)
{
  int nx=8;
  int ny=8;
  int nplanes=1;
  vil2_image_view<vil_byte> image(nx,ny,nplanes);

  // Slow fill
  for (int i=0;i<nplanes;++i)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image(x,y,i) = vil_byte(x+10*y+100*i);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

    // Objects to hold gradients
  vil2_image_view<float> grad_x,grad_y;

  vil2_algo_sobel_3x3(grad_x,grad_y,image);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Sobel X Gradient:"<<vcl_endl;
  vil2_print_all(vcl_cout,grad_x);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Sobel Y Gradient:"<<vcl_endl;
  vil2_print_all(vcl_cout,grad_y);

  return 0;
}
