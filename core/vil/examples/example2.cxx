//:
// \file
// \brief Example of manipulating different views of a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_byte.h>
#include <vcl_iostream.h>

int main(int argc, char** argv)
{
  unsigned ni=8;
  unsigned nj=8;
  unsigned nplanes=2;
  vil2_image_view<vil2_byte> image(ni,nj,nplanes);

  // Slow fill
  for (unsigned p=0;p<nplanes;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j,p) = vil2_byte(i+10*j+100*p);

  vcl_cout<<"Original image:\n";
  vil2_print_all(vcl_cout,image);


  vcl_cout<<"\nView central square of image\n";
  vil2_image_view<vil2_byte> win1 = vil2_window(image,2,4,2,4);
  vil2_print_all(vcl_cout,win1);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Manipulate central square of image through the window\n";
  win1.fill(0);

  vcl_cout<<"Original image:\n";
  vil2_print_all(vcl_cout,image);

  vcl_cout<<"\nView first plane of image\n";
  vil2_image_view<vil2_byte> plane = vil2_plane(image,1);
  vil2_print_all(vcl_cout,plane);

  vcl_cout<<"\nCreate transposed view of plane\n";
  vil2_image_view<vil2_byte> transpose = vil2_flip_transpose(plane);
  vil2_print_all(vcl_cout,transpose);

  vcl_cout<<"\nCreate view of plane reflected along i\n";
  vil2_image_view<vil2_byte> ref_i = vil2_flip_lr(plane);
  vil2_print_all(vcl_cout,ref_i);

  vcl_cout<<"\nCreate view of plane reflected along j\n";
  vil2_image_view<vil2_byte> ref_j = vil2_flip_ud(plane);
  vil2_print_all(vcl_cout,ref_j);

  return 0;
}
