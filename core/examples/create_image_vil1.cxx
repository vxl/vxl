// There is a movement to replace vil1 with vil. Consider looking at
// the other example file, create_image_vil2.cxx

#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_memory_image_of.h>

vil1_image make_image(int wd, int ht)
{
  vil1_memory_image_of<unsigned char> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x,y) = ((x-wd/2)*(y-ht/2)/16) % 256;
 
  return image;
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> output_filename(0, "output");
  vul_arg_parse(argc, argv);

  int sizex = 253;
  int sizey = 155;

  vil1_image image = make_image(sizex, sizey);

  vil1_save(image, output_filename().c_str());

  return 0;
}
