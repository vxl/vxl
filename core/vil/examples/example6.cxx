//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_rgb.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

//: Example function to return a 3-plane view of an RGB image
vil_image_view<vxl_byte> view_as_planes(const vil_image_view<vil_rgb<vxl_byte> >& v)
{
  assert(v.nplanes()==1);

  // Image is RGBRGBRGB so i step = 3*v.istep(), jstep=3*v.jstep()
  return vil_image_view<vxl_byte>(v.memory_chunk(),
                                  reinterpret_cast<vxl_byte const*>(v.top_left_ptr()),
                                  v.ni(),v.nj(),3,
                                  v.istep()*3,v.jstep()*3,1);
}

//: Example function to return an RGB view of a 3-plane image
vil_image_view<vil_rgb<vxl_byte> > view_as_rgb(const vil_image_view<vxl_byte>& v)
{
  assert(v.nplanes()==3);
  assert(v.planestep()==1);
  assert(v.istep()==3 || v.jstep()==3);

  return vil_image_view<vil_rgb<vxl_byte> >(v.memory_chunk(),
                                            reinterpret_cast<vil_rgb<vxl_byte> const*>(v.top_left_ptr()),
                                            v.ni(),v.nj(),1,
                                            v.istep()/3,v.jstep()/3,1);
}

int main()
{
  unsigned ni=6;
  unsigned nj=6;
  vil_image_view<vil_rgb<vxl_byte> > rgb_image(ni,nj);

  vcl_cout<<"Create an image of type vil_image_view<vil_rgb<vxl_byte> >\n";
  // Slow fill
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      rgb_image(i,j) = vil_rgb<vxl_byte>(i+10*j,i+10*j+100,i+10*j+200);

  vcl_cout<<"Original image:\n";
  vil_print_all(vcl_cout,rgb_image);

  vcl_cout<<"\nCreate a view of it as a set of planes:\n";
  vil_image_view<vxl_byte> plane_view = view_as_planes(rgb_image);
  vil_print_all(vcl_cout,plane_view);

  vcl_cout<<"\nCreate a view of this plane view as rgb:\n";
  vil_image_view<vil_rgb<vxl_byte> > rgb_image2 = view_as_rgb(plane_view);
  vil_print_all(vcl_cout,rgb_image2);

  return 0;
}
