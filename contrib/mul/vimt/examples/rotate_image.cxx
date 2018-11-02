//:
// \file
// \brief Example of rotating an image
// \author Tim Cootes - Manchester

#include <iostream>
#include <vxl_config.h> // for vxl_byte
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt/vimt_rotate.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_arg.h>

void print_usage()
{
  std::cout << "vimt_rotate_image -i image.jpg -A 15 -o output.jpg\n"
           << "Loads in named image.\n"
           << "Rotates by A (in degrees).\n"
           << "Saves output to given file.\n"
           << std::endl;
  vul_arg_display_usage_and_exit();
}

// Draw line between given world points
void draw_line(vimt_image_2d_of<vxl_byte>& image,
               double x0, double y0, double x1, double y1)
{
  vgl_point_2d<double> p0=image.world2im()(x0,y0);
  vgl_point_2d<double> p1=image.world2im()(x1,y1);

  vgl_vector_2d<double> u=p1-p0;
  double n = u.length();
  u/=n;
  for (unsigned i=0;i<n;++i)
  {
    vgl_point_2d<double> p=p0+i*u;
    image.image()(int(p.x()),int(p.y()))=vxl_byte(255);
  }
}

int main(int argc, char** argv)
{
  vul_arg<std::string> input_path("-i","Input image");
  vul_arg<std::string> output_path("-o","Output path","output.jpg");
  vul_arg<double> angle("-A","Angle",45.0);
  vul_arg_parse(argc,argv);

  if (input_path()=="")
  {
    print_usage();
    return 0;
  }


  vimt_image_2d_of<vxl_byte> image;
  image.image() = vil_load(input_path().c_str());
  if (image.image().size()==0)
  {
    std::cout<<"Failed to load in image from "<<input_path()<<std::endl;
    return 1;
  }

  vimt_image_2d_of<vxl_byte> rotated_im;

  vimt_rotate(image,rotated_im,angle());

  unsigned ni=image.image().ni();
  unsigned nj=image.image().nj();
  draw_line(rotated_im,0.5*ni,0.1*nj,  0.5*ni, 0.5*nj);

  vil_save(rotated_im.image(),output_path().c_str());

  return 0;
}
