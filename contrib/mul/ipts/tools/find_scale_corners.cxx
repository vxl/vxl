// Find all local peaks in DoG scale space of an image

#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vgl/vgl_point_3d.h>
#include <ipts/ipts_corner_pyramid.h>
#include <ipts/ipts_scale_space_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  std::cout<<"find_scale_corners -i input_image -s scale_step -t threshold -o out_image -c corner_pyramid\n"
          <<"Generates scale pyramid, computes corners and looks for local peaks\n";
}


int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path("-i","Input image path");
  vul_arg<std::string> out_path("-o","Output image file (peaks)","output.pnm");
  vul_arg<std::string> corner_path("-c","Output image file (corners)");
  vul_arg<float> threshold("-t","Threshold on corner response",100.0f);
  vul_arg<double> scale("-s","Scale step",1.41);

//  vul_arg<std::string> smooth_path("-s","Output image file (Smooth )","smooth.pnm");
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image = vil_load(in_path().c_str());
  if (image.ni()==0)
  {
    std::cout<<"Failed to load image.\n";
    return 1;
  }

  vimt_image_2d_of<float> image_f,flat_corners,flat_smooth;
  vil_convert_cast(image,image_f.image());
  vimt_image_pyramid smooth_pyramid,corner_pyramid;
  ipts_corner_pyramid(image_f,corner_pyramid,smooth_pyramid,scale());

  std::vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,corner_pyramid,threshold());
  std::cout<<"Found "<<peak_pts.size()<<" peaks.\n";

  for (auto & peak_pt : peak_pts)
  {
    if (peak_pt.z()>1.1)
    ipts_draw_cross(image,int(peak_pt.x()+0.5),
                     int(peak_pt.y()+0.5),
                     int(peak_pt.z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_corners,corner_pyramid);

  vil_save(image,out_path().c_str());
  std::cout<<"Image + pts saved to "<<out_path()<<std::endl;

  if (corner_path()!="")
  {
    vil_image_view<vxl_byte> out_corners;
    // Apply sqrt to make structure more visible
    vil_math_sqrt(flat_corners.image());
    vil_convert_stretch_range(flat_corners.image(),out_corners);
    vil_save(out_corners,corner_path().c_str());
    std::cout<<"(sqrt) Corner pyramid saved to "<<corner_path()<<std::endl;
  }

  return 0;
}
