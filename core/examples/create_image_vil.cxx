#include <string>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

vil_image_view<unsigned char> make_image(int wd, int ht)
{
  vil_image_view<unsigned char> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x,y) = (unsigned char)(((x-wd/2)*(y-ht/2)/16) % 256);

  return image;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> output_filename(nullptr, "output filename");
  vul_arg_parse(argc, argv);

  int sizex = 253;
  int sizey = 155;

  vil_image_view<unsigned char> image = make_image(sizex, sizey);

  vil_save( image, output_filename().c_str());

  return 0;
}
