// Find all local peaks in orientation entropy of scale space of an image

#include <ipts/ipts_entropy_pyramid.h>
#include <ipts/ipts_orientation_pyramid.h>
#include <vimt/vimt_scale_pyramid_builder_2d.h>
#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vgl/vgl_point_3d.h>
#include <ipts/ipts_scale_space_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  std::cout<<"find_scale_orient_entropy_peaks "
          <<"-i input_image -s scale_step -h half_width -t threshold -o out_image -e entropy_pyramid\n"
          <<"Generates scale pyramid, computes orientations at each level then\n"
          <<"computes local orientation entropy in squares with given half width\n"
          <<"looks for local peaks in the entropy pyramid\n";
}

int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path("-i","Input image path");
  vul_arg<std::string> out_path("-o","Output image file (peaks)","output.pnm");
  vul_arg<int> half_width("-h","Half-width of square for entropy measurements",5);
  vul_arg<float> grad_threshold("-gt","Threshold on gradients",10.0f);
  vul_arg<float> threshold("-t","Threshold for peak detection",-999);
  vul_arg<std::string> entropy_path("-e","Output image file for entropy pyramid)");
  vul_arg<double> scale("-s","Scale step",1.41);

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

  vimt_image_2d_of<vxl_byte> image0,flat_smooth;
  vimt_image_2d_of<float> image_f,flat_entropy;

  vil_convert_cast(image,image_f.image());

  unsigned n_orientations = 64;

  vimt_image_pyramid smooth_pyramid,entropy_pyramid,orient_pyramid;

  vimt_scale_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.set_scale_step(scale());
  pyr_builder.build(smooth_pyramid,image_f);

  ipts_orientation_pyramid(smooth_pyramid,orient_pyramid,grad_threshold(),n_orientations);

  ipts_entropy_pyramid(orient_pyramid,entropy_pyramid,
                       half_width(),0,n_orientations);


  std::vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,entropy_pyramid,threshold());
  std::cout<<"Found "<<peak_pts.size()<<" peaks.\n";

  for (auto & peak_pt : peak_pts)
  {
    if (peak_pt.z()>1.1)
    ipts_draw_cross(image,int(peak_pt.x()+0.5),
                     int(peak_pt.y()+0.5),
                     int(peak_pt.z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_entropy,entropy_pyramid);
//  vimt_image_pyramid_flatten(flat_smooth,smooth_pyramid);

  vil_save(image,out_path().c_str());
  std::cout<<"Image + pts saved to "<<out_path()<<std::endl;


  if (entropy_path()!="")
  {
    vil_image_view<vxl_byte> out_entropy;
    vil_convert_stretch_range(flat_entropy.image(),out_entropy);
    vil_save(out_entropy,entropy_path().c_str());
    std::cout<<"entropy pyramid saved to "<<entropy_path()<<std::endl;
  }

  return 0;
}
