//:
// \file
// \brief Example of taking sub-windows of an image.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vnl/vnl_vector.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_crop.h>
#include <vimt/vimt_sample_profile_bilin.h>

int main()
{
  int nx=10;
  int ny=10;
  int nplanes=1;
  vimt_image_2d_of<vxl_byte> image0(nx,ny,nplanes);

  // Slow fill
  for (int p=0;p<nplanes;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        image0.image()(x,y,p) = vxl_byte(x+10*y+100*p);

  image0.print_all(vcl_cout);

  vcl_cout<<"\nCreate a 5 x 5 window with corner at (3,3)\n";
  vimt_image_2d_of<vxl_byte> window = vimt_crop(image0, 3,5,3,5);
  window.print_all(vcl_cout);
  vcl_cout<<vcl_endl;

  vgl_point_2d<double> p0(4,4);
  vgl_vector_2d<double> u(0.5,0.25);
  vnl_vector<double> v;
  vcl_cout<<"Sampling along (0.5,0.25) starting at (4,4) :\n";
  vimt_sample_profile_bilin(v,static_cast<const vimt_image_2d_of<vxl_byte>&>(image0),p0,u,8);
  vcl_cout<<"Original Image View: "<<v<<vcl_endl;
  vimt_sample_profile_bilin(v,static_cast<const vimt_image_2d_of<vxl_byte>&>(window),p0,u,8);
  vcl_cout<<"Window on View     : "<<v<<vcl_endl

          <<"Notice that the projection of the world coordinates "
          <<"into image coordinates is all handled for us.\n"
          <<"The end of the second profile goes beyond the window, so is set to zero.\n";
  return 0;
}
