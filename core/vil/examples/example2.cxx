//:
// \file
// \brief Example of manipulating different views of a vil_image_view<T>.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_transpose.h>
#include <vil/vil_flip.h>
#include <vil/vil_plane.h>
#include <vil/vil_print.h>

int main()
{
  unsigned ni=8;
  unsigned nj=8;
  unsigned nplanes=2;
  vil_image_view<vxl_byte> image(ni,nj,nplanes);

  // Slow fill
  for (unsigned p=0;p<nplanes;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j,p) = vxl_byte(i+10*j+100*p);

  vcl_cout<<"Original image:\n";
  vil_print_all(vcl_cout,image);


  vcl_cout << "\nView central square of image\n";
  vil_image_view<vxl_byte> win1 = vil_crop(image,2,4,2,4);
  vil_print_all(vcl_cout,win1);

  vcl_cout << "\nManipulate central square of image through the window\n";
  win1.fill(0);

  vcl_cout<<"Original image:\n";
  vil_print_all(vcl_cout,image);

  vcl_cout<<"\nView first plane of image\n";
  vil_image_view<vxl_byte> plane = vil_plane(image,1);
  vil_print_all(vcl_cout,plane);

  vcl_cout<<"\nCreate transposed view of plane\n";
  vil_image_view<vxl_byte> transpose = vil_transpose(plane);
  vil_print_all(vcl_cout,transpose);

  vcl_cout<<"\nCreate view of plane reflected along i\n";
  vil_image_view<vxl_byte> ref_i = vil_flip_lr(plane);
  vil_print_all(vcl_cout,ref_i);

  vcl_cout<<"\nCreate view of plane reflected along j\n";
  vil_image_view<vxl_byte> ref_j = vil_flip_ud(plane);
  vil_print_all(vcl_cout,ref_j);

  return 0;
}
