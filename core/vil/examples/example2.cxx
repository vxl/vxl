//:
// \file
// \brief Example of manipulating different views of a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>

int main(int argc, char** argv)
{
  int nx=8;
  int ny=8;
  int nplanes=2;
  vil2_image_view<vil_byte> image(nx,ny,nplanes);

  // Slow fill
  for (int i=0;i<nplanes;++i)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image(x,y,i) = vil_byte(x+10*y+100*i);

  vcl_cout<<"Original image:"<<vcl_endl;
  image.print_all(vcl_cout);


  vcl_cout<<vcl_endl;
  vcl_cout<<"View central square of image"<<vcl_endl;
  vil2_image_view<vil_byte> win1 = image.window(2,4,2,4);
  win1.print_all(vcl_cout);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Manipulate central square of image through the window"<<vcl_endl;
  win1.fill(0);

  vcl_cout<<"Original image:"<<vcl_endl;
  image.print_all(vcl_cout);

  vcl_cout<<vcl_endl;
  vcl_cout<<"View first plane of image"<<vcl_endl;
  vil2_image_view<vil_byte> plane = image.plane(1);
  plane.print_all(vcl_cout);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create transposed view of plane"<<vcl_endl;
  vil2_image_view<vil_byte> transpose = plane.transpose();
  transpose.print_all(vcl_cout);



  return 0;
}
