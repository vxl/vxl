//:
// \file
// \brief Experimental tool to load in an image and compute local entropy.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_fill.h>
#include <ipts/ipts_local_entropy.h>
#include <vil/algo/vil_suppress_non_max.h>
#include <vimt/algo/vimt_find_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  vcl_cout<<"show_local_entropy -i in_image -e entropy_image -o out_image -h half_width"<<vcl_endl;
  vcl_cout<<"Load in an image and generate entropy from square regions."<<vcl_endl;
  vcl_cout<<"Show peaks in entropy as crosses on original image."<<vcl_endl;
}

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> entropy_path("-e","Entropy image","entropy.jpg");
  vul_arg<vcl_string> out_path("-o","Output image","output.jpg");
  vul_arg<int> half_width("-h","Half width of RIO",10);
  vul_arg_parse(argc, argv);

  if(in_path() == "")
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

  vil_image_view<vxl_byte> grey_im;
  vil_math_mean_over_planes(src_im,grey_im);

  vil_image_view<float> entropy_im,entropy_max_im;
  ipts_local_entropy(grey_im,entropy_im,half_width());

//  vil_suppress_non_max_3x3(entropy_im,entropy_max_im);

  vil_image_view<vxl_byte> dest_im;
  vil_convert_stretch_range(entropy_im,dest_im);

  if (!vil_save(dest_im, entropy_path().c_str()))
  {
    vcl_cerr<<"Unable to save entropy image to "<<entropy_path()<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Saved image to "<<entropy_path()<<vcl_endl;

  vcl_vector<vgl_point_2d<unsigned> > peaks;
  vimt_find_image_peaks_3x3(peaks,entropy_im);
  for (unsigned i=0;i<peaks.size();++i)
    ipts_draw_cross(grey_im,peaks[i].x()+half_width(),peaks[i].y()+half_width(),half_width(),vxl_byte(255));

  if (!vil_save(grey_im, out_path().c_str()))
  {
    vcl_cerr<<"Unable to save result image to "<<out_path()<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Saved image to "<<out_path()<<vcl_endl;

  return 0;
}
