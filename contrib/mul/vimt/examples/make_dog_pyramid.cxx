// Example tool to load in an image, create a DoG pyramid and write it out

#include <vimt/vimt_dog_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vil/vil_convert.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

void print_usage()
{
  vcl_cout<<"make_dog_pyramid -i input_image -o out_image"<<vcl_endl;
}

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image path");
  vul_arg<vcl_string> out_path("-o","Output image file (DOG)");
  vul_arg<vcl_string> smooth_path("-s","Output image file (smoothed)");
  vul_arg_parse(argc, argv);

  if(in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image = vil_load(in_path().c_str());
  vimt_image_2d_of<float> image_f,flat_dog,flat_smooth;
  vil_convert_cast(image,image_f.image());

  vimt_image_pyramid dog_pyramid,smooth_pyramid;
  vimt_dog_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.build_dog(dog_pyramid,smooth_pyramid,image_f);

  vimt_image_pyramid_flatten(flat_dog,dog_pyramid);
  vimt_image_pyramid_flatten(flat_smooth,smooth_pyramid);

  vil_image_view<vxl_byte> out_dog;
  vil_convert_stretch_range(flat_dog.image(),out_dog);
  vil_save(out_dog,out_path().c_str());
  vcl_cout<<"DoG saved to "<<out_path()<<vcl_endl;

  vil_image_view<vxl_byte> out_smooth;
  vil_convert_stretch_range(flat_smooth.image(),out_smooth);
  vil_save(out_smooth,smooth_path().c_str());
  vcl_cout<<"Smooth pyramid saved to "<<smooth_path()<<vcl_endl;

  return 0;
}
