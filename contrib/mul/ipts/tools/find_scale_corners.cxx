// Find all local peaks in DoG scale space of an image

#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <ipts/ipts_corner_pyramid.h>
#include <ipts/ipts_scale_space_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  vcl_cout<<"find_scale_corners -i input_image -s scale_step -t threshold -o out_image -c corner_pyramid\n"
          <<"Generates scale pyramid, computes corners and looks for local peaks\n";
}


int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image path");
  vul_arg<vcl_string> out_path("-o","Output image file (peaks)");
  vul_arg<vcl_string> corner_path("-c","Output image file (corners)");
  vul_arg<float> threshold("-t","Threshold on corner respose",100.0f);
  vul_arg<double> scale("-s","Scale step",1.41);

//  vul_arg<vcl_string> smooth_path("-s","Output image file (Smooth )","smooth.pnm");
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image = vil_load(in_path().c_str());
  if (image.ni()==0)
  {
    vcl_cout<<"Failed to load image.\n";
    return 1;
  }

  vimt_image_2d_of<float> image_f,flat_corners,flat_smooth;
  vil_convert_cast(image,image_f.image());
  vimt_image_pyramid smooth_pyramid,corner_pyramid;
  ipts_corner_pyramid(image_f,corner_pyramid,smooth_pyramid,scale());

  vcl_vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,corner_pyramid,threshold());
  vcl_cout<<"Found "<<peak_pts.size()<<" peaks.\n";

  for (unsigned i=0;i<peak_pts.size();++i)
  {
    if (peak_pts[i].z()>1.1)
    ipts_draw_cross(image,int(peak_pts[i].x()+0.5),
                     int(peak_pts[i].y()+0.5),
                     int(peak_pts[i].z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_corners,corner_pyramid);

  vil_save(image,out_path().c_str());
  vcl_cout<<"Image + pts saved to "<<out_path()<<vcl_endl;

  if (corner_path()!="")
  {
    vil_image_view<vxl_byte> out_corners;
    vil_convert_stretch_range(flat_corners.image(),out_corners);
    vil_save(out_corners,corner_path().c_str());
    vcl_cout<<"Corner pyramid saved to "<<corner_path()<<vcl_endl;
  }

  return 0;
}
