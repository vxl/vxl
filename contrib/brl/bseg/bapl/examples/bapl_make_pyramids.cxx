// This is brl/bseg/bapl/examples/bapl_make_pyramids.cxx
//:
// \file

#include <sstream>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <bapl/bapl_lowe_pyramid_set.h>


int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path("-i","Input image");
  vul_arg<std::string> out_path("-o","Output Directory");
  vul_arg_parse(argc, argv);

  if (!in_path.set())
    vul_arg_display_usage_and_exit();

  vil_image_view<vxl_byte> image = vil_convert_to_grey_using_rgb_weighting (vil_load(in_path().c_str()));
  if (image.ni()==0)
  {
    std::cerr<<"Failed to load image.\n";
    return 1;
  }

  std::cout << "Constructing Pyramids ...";


  vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(image);
  bapl_lowe_pyramid_set pyramid_set(image_sptr);

  // Save images
  vil_image_view<vxl_byte> temp;
  for (int lvl=0; lvl<pyramid_set.num_octaves(); ++lvl){
    for (int octsz=0; octsz<pyramid_set.octave_size(); ++octsz){
      std::stringstream name_gauss, name_dog, name_grad_oreint, name_grad_mag;
      name_gauss << out_path() << "/gauss"<<lvl<<'_'<<octsz<<".jpg";
      name_dog << out_path() << "/dog"<<lvl<<'_'<<octsz<<".jpg";
      name_grad_oreint << out_path() << "/orient"<<lvl<<'_'<<octsz<<".jpg";
      name_grad_mag << out_path() << "/mag"<<lvl<<'_'<<octsz<<".jpg";

      vil_convert_stretch_range(pyramid_set.gauss_pyramid(lvl,octsz),temp);
      vil_save(temp, name_gauss.str().c_str());
      vil_convert_stretch_range(pyramid_set.dog_pyramid(lvl,octsz),temp);
      vil_save(temp, name_dog.str().c_str() );
      vil_convert_stretch_range(pyramid_set.grad_orient_pyramid(lvl,octsz),temp);
      vil_save(temp, name_grad_oreint.str().c_str());
      vil_convert_stretch_range(pyramid_set.grad_mag_pyramid(lvl,octsz),temp);
      vil_save(temp, name_grad_mag.str().c_str() );
    }
  }

  std::cout <<  " done!" <<std::endl;
  return 0;
}
