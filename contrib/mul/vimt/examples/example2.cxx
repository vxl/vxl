//:
// \file
// \brief Example of sampling from images.
// \author Tim Cootes - Manchester

#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_sample_profile_bilin.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>

int main(int argc, char** argv)
{
  int nx=10;
  int ny=10;
  int nplanes=1;
  vimt_image_2d_of<vil_byte> base_image(nx,ny,nplanes);

  // Slow fill
  for (int p=0;p<nplanes;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        base_image.image()(x,y,p) = vil_byte(x+10*y+100*p);

  vimt_gaussian_pyramid_builder_2d<vil_byte> builder;
  vimt_image_pyramid image_pyr;
  builder.build(image_pyr,base_image);

  image_pyr.print_all(vcl_cout);
  vcl_cout<<vcl_endl;

  vgl_point_2d<double> p0(3,3);
  vgl_vector_2d<double> u(0.5,0.25);
  vnl_vector<double> v;
  vcl_cout<<"Sampling along (0.5,0.25) starting at (3,3) : "<<vcl_endl;
  vimt_sample_profile_bilin(v,(const vimt_image_2d_of<vil_byte>&) image_pyr(0),p0,u,7);
  vcl_cout<<"Level 0: "<<v<<vcl_endl;
  vimt_sample_profile_bilin(v,(const vimt_image_2d_of<vil_byte>&) image_pyr(1),p0,u,7);
  vcl_cout<<"Level 1: "<<v<<vcl_endl;

  vcl_cout<<"Notice that the projection of the world co-ordinates ";
  vcl_cout<<"into image coordinates is all handled for us."<<vcl_endl;
  return 0;
}
