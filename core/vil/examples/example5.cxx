//:
// \file
// \brief Example of data scoping for views.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>


int main(int argc, char** argv)
{
  vcl_cout<<"Create an empty image"<<vcl_endl;
  vil2_image_view<vil_byte> image_view1;

  {
    vcl_cout<<"Create a second image"<<vcl_endl;
    int nx=6;
    int ny=6;
    vil2_image_view<vil_byte> image_view2(nx,ny);

    // Slow fill
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image_view2(x,y) = vil_byte(x+10*y);

    vcl_cout<<"image_view2:"<<vcl_endl;
    vil2_print_all(vcl_cout,image_view2);

    vcl_cout<<"Set first image to be a view of the second one"<<vcl_endl;
    image_view1 = image_view2;
    vcl_cout<<"image_view1:"<<vcl_endl;
    vil2_print_all(vcl_cout,image_view1);
  }

  vcl_cout<<"Image view2 has gone out of scope and is thus destroyed."<<vcl_endl;
  vcl_cout<<"However, the image data still lives on, until image_view1 dies."<<vcl_endl;

  vcl_cout<<"image_view1:"<<vcl_endl;
  vil2_print_all(vcl_cout,image_view1);

  return 0;
}
