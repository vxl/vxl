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

  // Slow fill
  for (int i=0;i<nplanes;++i)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image(x,y,i) = vil_byte(x+y+i);

  // Fast fill
  vil_byte* plane = image.top_left_ptr();
  for (unsigned int i=0;i<nplanes;++i,plane += image.planestep())
  {
    vil_byte* row = plane;
    for (int y=0;y<ny;++y,row += image.ystep())
    {
      vil_byte* p = row;
      for (int x=0;x<nx;++x,p+=image.xstep())
	    *p = vil_byte(x+y+i);
    }
  }

  return 0;
}
