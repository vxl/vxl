#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view_base.h>
#include <vil2/vil2_image_view.h>

vil2_image_view_base_sptr make_image(int wd, int ht)
{
  vil2_image_view<unsigned char> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++)
      image(x,y) = ((x-wd/2)*(y-ht/2)/16) % 256;
 
  return &image;
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> output_filename(0, "output");
  vul_arg_parse(argc, argv);

  int sizex = 253;
  int sizey = 155;

  vil2_image_view_base_sptr image_p = make_image(sizex, sizey);

  vil2_save( *image_p, output_filename().c_str());
}
