//:
// \file
// \brief Example of creating an image pyramid.
// \author Tim Cootes - Manchester

#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>

int main(int argc, char** argv)
{
  int nx=20;
  int ny=20;
  int nplanes=1;
  vimt_image_2d_of<vil_byte> base_image(nx,ny,nplanes);

  // Slow fill
  for (int p=0;p<nplanes;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        base_image.image()(x,y,p) = vil_byte(x+10*y+100*p);

  vcl_cout<<"Base image"<<vcl_endl;
  base_image.print_all(vcl_cout);
  vcl_cout<<vcl_endl;

  vimt_gaussian_pyramid_builder_2d<vil_byte> builder;
  vimt_image_pyramid image_pyr;
  builder.build(image_pyr,base_image);

  image_pyr.print_all(vcl_cout);
  vcl_cout<<vcl_endl;

  return 0;
}
