//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//: Example function to return a 3-plane view of an RGB image
vil2_image_view<vil_byte> view_as_planes(const vil2_image_view<vil_rgb<vil_byte> >& v)
{
  assert(v.nplanes()==1);

  // Image is RGBRGBRGB so x step = 3*v.xstep(), ystep=3*v.ystep()
  return vil2_image_view<vil_byte>(v.memory_chunk(),(vil_byte*) v.top_left_ptr(),
                                   v.nx(),v.ny(),3,
                                   v.xstep()*3,v.ystep()*3,1);
}

//: Example function to return an RGB view of a 3-plane image
vil2_image_view<vil_rgb<vil_byte> > view_as_rgb(const vil2_image_view<vil_byte>& v)
{
  assert(v.nplanes()==3);
  assert(v.planestep()==1);
  assert(v.xstep()==3 || v.ystep()==3);

  return vil2_image_view<vil_rgb<vil_byte> >(v.memory_chunk(),
                                             (vil_rgb<vil_byte>*) v.top_left_ptr(),
                                             v.nx(),v.ny(),1,
                                             v.xstep()/3,v.ystep()/3,1);
}

int main(int argc, char** argv)
{
  int nx=6;
  int ny=6;
  vil2_image_view<vil_rgb<vil_byte> > rgb_image(nx,ny);

  vcl_cout<<"Create an image of type vil2_image_view<vil_rgb<vil_byte> >"<<vcl_endl;
  // Slow fill
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      rgb_image(x,y) = vil_rgb<vil_byte>(x+10*y,x+10*y+100,x+10*y+200);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,rgb_image);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create a view of it as a set of planes:"<<vcl_endl;
  vil2_image_view<vil_byte> plane_view = view_as_planes(rgb_image);
  vil2_print_all(vcl_cout,plane_view);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create a view of this plane view as rgb:"<<vcl_endl;
  vil2_image_view<vil_rgb<vil_byte> > rgb_image2 = view_as_rgb(plane_view);
  vil2_print_all(vcl_cout,rgb_image2);

  return 0;
}
