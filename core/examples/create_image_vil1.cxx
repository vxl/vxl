// There is a movement to replace vil with vil2. Consider looking at
// the other example file, create_image_vil2.cxx

#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

vil_image make_image(int wd, int ht)
{
  vil_memory_image_of<unsigned char> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++)
      image(x,y) = ((x-wd/2)*(y-ht/2)/16) % 256;
 
  return image;
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> output_filename(0, "output");
  vul_arg_parse(argc, argv);

  int sizex = 253;
  int sizey = 155;

  vil_image image = make_image(sizex, sizey);

  vil_save(image, output_filename().c_str());
}
