//:
// \file
// \brief Example of creating and filling a vil2_image_view<T>.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  unsigned ni=10;
  unsigned nj=10;
  unsigned nplanes=2;
  vil2_image_view<vxl_byte> image(ni,nj,nplanes);

  // Slow fill
  for (unsigned p=0;p<nplanes;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j,p) = vxl_byte(i+10*j+100*p);

  vcl_cout<<"Slow fill image"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  // Fast fill
  vxl_byte* plane = image.top_left_ptr();
  for (unsigned p=0;p<nplanes;++p,plane += image.planestep())
  {
    vxl_byte* row = plane;
    for (unsigned j=0;j<nj;++j,row += image.jstep())
    {
      vxl_byte* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=image.istep())
        *pixel = vxl_byte(i+10*j+100*p);
    }
  }

  vcl_cout<<"Fast fill image"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  return 0;
}
