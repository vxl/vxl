//:
// \file
// \brief Example of data scoping for views.
// \author Tim Cootes - Manchester

#include <vxl_config.h>
#include <vcl_iostream.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>


int main(int argc, char** argv)
{
  vcl_cout<<"Create an empty image"<<vcl_endl;
  vil2_image_view<vxl_byte> image_view1;

  {
    vcl_cout<<"Create a second image"<<vcl_endl;
    unsigned ni=9;
    unsigned nj=6;
    vil2_image_view<vxl_byte> image_view2(ni,nj);

    // Slow fill
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image_view2(i,j) = vxl_byte(i+10*j);

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
