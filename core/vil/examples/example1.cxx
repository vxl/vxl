//:
// \file
// \brief Example of manipulating vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>

int main(int argc, char** argv)
{
  int nx=256;
  int ny=256;
  int nplanes=3;
  vil2_image_view<vil_byte> image(nx,ny,nplanes);
  vil_byte* plane_data = image.top_left_ptr();
  for (int i=0;i<nplanes;++i)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image(x,y,i) = vil_byte(x+y+i);
  return 0;
}
