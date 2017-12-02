
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include<bil/algo/bil_debayer_image.h>
#include <vul/vul_arg.h>
// Debayer the image ( GRBG) to RGB


//: Execute the process
int main(int argc, char ** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<std::string> inimg("-i", "input grayscale image", "");
  vul_arg<std::string> outimg("-o", "output RGB image", "");
  vul_arg<std::string> ftype("-f", "Filter type ( currently supports BGGR & GRBG )", "BGGR");
  vul_arg_parse(argc, argv);
  vil_image_view_base_sptr inimg_resc = vil_load(inimg().c_str());
  if(inimg_resc->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
  {
      std::cout<<"Error: Image is not grayscale "<<std::endl;
      return -1;
  }
  vil_image_view<vil_rgb<vxl_byte> > debayer_img(inimg_resc->ni(),inimg_resc->nj());
  if(ftype() == "BGGR")
  {
    bil_debayer_image::bil_debayer_BGGR(inimg_resc,&debayer_img);
  }
  else if (ftype() == "GRBG")
  {
    bil_debayer_image::bil_debayer_GRBG(inimg_resc,&debayer_img);
  }
  else
  {
      std::cout<<"Not implement for the specified filter"<<std::endl;
      return -1;
  }
  vil_save(debayer_img,outimg().c_str());
  return true;
}
