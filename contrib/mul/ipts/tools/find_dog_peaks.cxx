// Find all local peaks in DoG scale space of an image

#include <vimt/vimt_dog_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vgl/vgl_point_3d.h>
#include <ipts/ipts_scale_space_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  std::cout<<"find_dog_peaks -i input_image -o out_image -d dog_image"<<std::endl;
}

int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path("-i","Input image path");
  vul_arg<std::string> out_path("-o","Output image file (peaks)");
  vul_arg<std::string> dog_path("-d","Output image file (DOG)","dog.pnm");
  vul_arg<std::string> smooth_path("-s","Output image file (Smooth )","smooth.pnm");
  vul_arg<float> threshold("-t","Threshold on DoG value",2.0f);
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image = vil_load(in_path().c_str());
  if (image.ni()==0)
  {
    std::cout<<"Failed to load image."<<std::endl;
    return 1;
  }

  vimt_image_2d_of<float> image_f,flat_dog,flat_smooth;
  vil_convert_cast(image,image_f.image());

  // Invert image, so we pick out troughs
//  vil_math_scale_values(image_f.image(),-1.0);

  vimt_image_pyramid dog_pyramid,smooth_pyramid;
  vimt_dog_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.build_dog(dog_pyramid,smooth_pyramid,image_f,true);

  std::vector<vgl_point_3d<double> > peak_pts;
  ipts_scale_space_peaks_2d(peak_pts,dog_pyramid,threshold());
  std::cout<<"Found "<<peak_pts.size()<<" peaks."<<std::endl;

  for (auto & peak_pt : peak_pts)
  {
    if (peak_pt.z()>1.1)
    ipts_draw_cross(image,int(peak_pt.x()+0.5),
                     int(peak_pt.y()+0.5),
                     int(peak_pt.z()+0.5), vxl_byte(255) );
  }

  vimt_image_pyramid_flatten(flat_dog,dog_pyramid);
  vimt_image_pyramid_flatten(flat_smooth,smooth_pyramid);

  vil_save(image,out_path().c_str());
  std::cout<<"Image + pts saved to "<<out_path()<<std::endl;

  vil_image_view<vxl_byte> out_dog;
  vil_convert_stretch_range(flat_dog.image(),out_dog);
  vil_save(out_dog,dog_path().c_str());
  std::cout<<"DoG pyramid saved to "<<dog_path()<<std::endl;

  vil_image_view<vxl_byte> out_smooth;
  vil_convert_stretch_range(flat_smooth.image(),out_smooth);
  vil_save(out_smooth,smooth_path().c_str());
  std::cout<<"Smooth pyramid saved to "<<smooth_path()<<std::endl;

  return 0;
}
