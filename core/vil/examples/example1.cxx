//:
// \file
// \brief Example of creating and filling a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>

int main(int argc, char** argv)
{
  int nx=10;
  int ny=10;
  int nplanes=2;
  vil2_image_view<vil_byte> image(nx,ny,nplanes);

  // Slow fill
  for (int p=0;p<nplanes;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image(x,y,p) = vil_byte(x+10*y+100*p);

  vcl_cout<<"Slow fill image"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  // Fast fill
  vil_byte* plane = image.top_left_ptr();
  for (unsigned int p=0;p<nplanes;++p,plane += image.planestep())
  {
    vil_byte* row = plane;
    for (int y=0;y<ny;++y,row += image.ystep())
    {
      vil_byte* pixel = row;
      for (int x=0;x<nx;++x,pixel+=image.xstep())
        *pixel = vil_byte(x+10*y+100*p);
    }
  }

  vcl_cout<<"Fast fill image"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  return 0;
}
