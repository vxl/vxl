//:
// \file
// \brief Example of data scoping for views.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

int main()
{
  vcl_cout<<"Create an empty image\n";
  vil_image_view<vxl_byte> image_view1;

  {
    vcl_cout<<"Create a second image\n";
    unsigned ni=9;
    unsigned nj=6;
    vil_image_view<vxl_byte> image_view2(ni,nj);

    // Slow fill
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image_view2(i,j) = vxl_byte(i+10*j);

    vcl_cout<<"image_view2:\n";
    vil_print_all(vcl_cout,image_view2);

    vcl_cout<<"Set first image to be a view of the second one\n";
    image_view1 = image_view2;
    vcl_cout<<"image_view1:\n";
    vil_print_all(vcl_cout,image_view1);
  }

  vcl_cout<<"Image view2 has gone out of scope and is thus destroyed.\n"
          <<"However, the image data still lives on, until image_view1 dies.\n";

  vcl_cout<<"image_view1:\n";
  vil_print_all(vcl_cout,image_view1);

  return 0;
}
