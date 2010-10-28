// This is algo/bapl/bapl_make_pyramids.cxx
//:
// \file

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <bapl/bapl_lowe_pyramid_set.h>



int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> out_path("-o","Output Directory");
  vul_arg_parse(argc, argv);

  if(!in_path.set())
    vul_arg_display_usage_and_exit();

  vil_image_view<vxl_byte> image = vil_convert_to_grey_using_rgb_weighting (vil_load(in_path().c_str()));
  if (image.ni()==0)
  {
    vcl_cout<<"Failed to load image."<<vcl_endl;
    return 1;
  }

  vcl_cout << "Constructing Pyramids" << vcl_endl;


  vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(image);
  bapl_lowe_pyramid_set pyramid_set(image_sptr);

  // Save images
  vil_image_view<vxl_byte> temp;
  for(int lvl=0; lvl<pyramid_set.num_octaves(); ++lvl){
    for(int oct=0; oct<pyramid_set.octave_size(); ++oct){
      vcl_stringstream name_gauss, name_dog, name_grad_oreint, name_grad_mag;
      name_gauss << out_path() << "/gauss"<<lvl<<'_'<<oct<<".jpg";
      name_dog << out_path() << "/dog"<<lvl<<'_'<<oct<<".jpg";
      name_grad_oreint << out_path() << "/orient"<<lvl<<'_'<<oct<<".jpg";
      name_grad_mag << out_path() << "/mag"<<lvl<<'_'<<oct<<".jpg";

      vil_convert_stretch_range(pyramid_set.gauss_pyramid(lvl,oct),temp);
      vil_save(temp, name_gauss.str().c_str());
      vil_convert_stretch_range(pyramid_set.dog_pyramid(lvl,oct),temp);
      vil_save(temp, name_dog.str().c_str() );
      vil_convert_stretch_range(pyramid_set.grad_orient_pyramid(lvl,oct),temp);
      vil_save(temp, name_grad_oreint.str().c_str());
      vil_convert_stretch_range(pyramid_set.grad_mag_pyramid(lvl,oct),temp);
      vil_save(temp, name_grad_mag.str().c_str() );
    }
  }
 
  vcl_cout <<  "done!" <<vcl_endl;
  return 0;
}


