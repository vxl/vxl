//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_byte.h>
#include <vil/vil_rgb.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//: Example function to return a 3-plane view of an RGB image
vil2_image_view<vil2_byte> view_as_planes(const vil2_image_view<vil_rgb<vil2_byte> >& v)
{
  assert(v.nplanes()==1);

  // Image is RGBRGBRGB so i step = 3*v.istep(), jstep=3*v.jstep()
  return vil2_image_view<vil2_byte>(v.memory_chunk(),(vil2_byte const*) v.top_left_ptr(),
                                   v.ni(),v.nj(),3,
                                   v.istep()*3,v.jstep()*3,1);
}

//: Example function to return an RGB view of a 3-plane image
vil2_image_view<vil_rgb<vil2_byte> > view_as_rgb(const vil2_image_view<vil2_byte>& v)
{
  assert(v.nplanes()==3);
  assert(v.planestep()==1);
  assert(v.istep()==3 || v.jstep()==3);

  return vil2_image_view<vil_rgb<vil2_byte> >(v.memory_chunk(),
                                             (vil_rgb<vil2_byte> const*) v.top_left_ptr(),
                                             v.ni(),v.nj(),1,
                                             v.istep()/3,v.jstep()/3,1);
}

int main(int argc, char** argv)
{
  unsigned ni=6;
  unsigned nj=6;
  vil2_image_view<vil_rgb<vil2_byte> > rgb_image(ni,nj);

  vcl_cout<<"Create an image of type vil2_image_view<vil_rgb<vil2_byte> >"<<vcl_endl;
  // Slow fill
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      rgb_image(i,j) = vil_rgb<vil2_byte>(i+10*j,i+10*j+100,i+10*j+200);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,rgb_image);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create a view of it as a set of planes:"<<vcl_endl;
  vil2_image_view<vil2_byte> plane_view = view_as_planes(rgb_image);
  vil2_print_all(vcl_cout,plane_view);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create a view of this plane view as rgb:"<<vcl_endl;
  vil2_image_view<vil_rgb<vil2_byte> > rgb_image2 = view_as_rgb(plane_view);
  vil2_print_all(vcl_cout,rgb_image2);

  return 0;
}
