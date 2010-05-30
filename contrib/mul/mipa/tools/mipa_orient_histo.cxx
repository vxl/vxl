//:
// \file
// \brief Tool to compute an orientation histogram image from an input image
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <mipa/mipa_orientation_histogram.h>
#include <vil/vil_convert.h>
#include <vul/vul_arg.h>

void print_usage()
{
  vcl_cout<<"mipa_orient_histo  -i src_image -o out_image -c 4\n"
          <<"Tool to compute an orientation histogram image from an input image.\n"
          <<"Uses 4 orientation bins, outputs 3-plane image with first three.\n"
          <<"Each histogram is computed from pixels in square of size cell_size"<<vcl_endl;
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> out_path("-o","Output path","output.jpg");
  vul_arg<unsigned> cell_size("-c","Cell size",4);
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> src_im = vil_load(in_path().c_str());
  if (src_im.size()==0)
  {
    vcl_cout<<"Unable to load source image from "<<in_path()<<vcl_endl;
    return 1;
  }

  vil_image_view<float> hog_image;

  if (src_im.nplanes()==1)
    mipa_orientation_histogram(src_im,hog_image,4,cell_size(),false);
  else
  {
    vil_image_view<float> grey_im;
    vil_convert_planes_to_grey(src_im,grey_im);
    mipa_orientation_histogram(grey_im,hog_image,4,cell_size(),false);
  }

  // Save first 3 planes, so can view as colour image.
  vil_image_view<vxl_byte> dest_im;
  vil_convert_stretch_range(vil_planes(hog_image,0,1,3),dest_im);
  if (!vil_save(dest_im, out_path().c_str()))
  {
    vcl_cerr<<"Unable to save result image to "<<out_path()<<vcl_endl;
    return 1;
  }

  return 0;
}
