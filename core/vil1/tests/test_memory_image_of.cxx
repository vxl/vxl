#include <vcl_iostream.h>

#include <vpl/vpl.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vil/vil_rgb.h>

#include <vil/vil_test.h>
#include <vcl_cstdio.h>

const int W = 768;
const int H = 256;

void test_vil_memory_image_of()
{
  char const* TMPNAM2 = tempnam(0,"vil_test_memory_image_of.pgm");
  char const* file_name_2 = TMPNAM2 ? TMPNAM2 : "/tmp/vil_test_memory_image_of.pgm";
  {
    vcl_cout << "unsigned char" << vcl_endl;
    vil_memory_image_of<unsigned char> image(W,H);

    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
        image(x,y) = ((x - W/2) * (y - H/2) / 16) % 256;
      }
    vil_save(image, file_name_2);
    vcl_cout << "Saved image to " << file_name_2 << vcl_endl;
  }

  // Don't leave the images behind by default. If you need to debug, comment
  // these out temporarily and make sure you put them back in before committing
  // your changes.
  vpl_unlink(file_name_2);

  char const* TMPNAM1 = tempnam(0,"vil_test_memory_image_of.ppm");
  char const* file_name_1 = TMPNAM1 ? TMPNAM1 : "/tmp/vil_test_memory_image_of.ppm";

  {
    vcl_cout << "vil_rgb_byte" << vcl_endl;
    vil_memory_image_of<vil_rgb<unsigned char> > image(W,H);

    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
        vil_rgb<unsigned char>& p = image(x,y);
        p.r = x;
        p.g = ((x - W/2) * (y - H/2) / 16) % 256;
        p.b = y/3;
      }
    vil_save(image, file_name_1);
    vcl_cout << "Saved image to " << file_name_1 << vcl_endl;
  }

  // Don't leave the images behind by default. If you need to debug, comment
  // these out temporarily and make sure you put them back in before committing
  // your changes.
  vpl_unlink(file_name_1);


  {
    vcl_cout << "bool" << vcl_endl;
    vil_memory_image_of<bool> map(451, 349);
    for (int x=0; x<map.width(); ++x)
      for (int y=0; y<map.height(); ++y)
        map(x, y) = false;
  }

  {
    vcl_cout << "external_buffer" << vcl_endl;
    unsigned char buf[] = { 1, 2, 3, 4, 5, 6 };
    vil_memory_image_of<unsigned char> image(buf,3,2);

    unsigned char&
    p = image(0,0); TEST ("p == 1", p, 1);
    p = image(1,0); TEST ("p == 2", p, 2);
    p = image(2,0); TEST ("p == 3", p, 3);
    p = image(0,1); TEST ("p == 4", p, 4);
    p = image(1,1); TEST ("p == 5", p, 5);
    p = image(2,1); TEST ("p == 6", p, 6);
  }

  {
  }
}

TESTMAIN(test_vil_memory_image_of);
