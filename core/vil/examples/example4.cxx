//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>

//: Example function to return a transpose of the view
vil2_image_view<vil_byte> transpose_view(const vil2_image_view<vil_byte>& v)
{
  // Create view with x and y switched
  return vil2_image_view<vil_byte>(v.memory_chunk(),v.top_left_ptr(),
                                   v.ny(),v.nx(),v.nplanes(),
                                   v.ystep(),v.xstep(),v.planestep());
}

int main(int argc, char** argv)
{
  int nx=8;
  int ny=8;
  vil2_image_view<vil_byte> image(nx,ny);

  // Slow fill
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      image(x,y) = vil_byte(x+10*y);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create transposed view of plane"<<vcl_endl;
  vil2_image_view<vil_byte> transpose = transpose_view(image);
  vil2_print_all(vcl_cout,transpose);

  return 0;
}
