#include <image/Image_to_vil.h>
#include <image/vil_to_Image.h>
//#include <image/vbl_array_to_vil.h>
//#include <image/vil_to_vbl_array.h>

#include <vcl_cstring.h> // for memcmp()
#include <vil/vil_load.h>
#include <vnl/vnl_test.h>

static char* filename = 0;

void test_image_conversions()
{
  if (!filename) { vcl_cerr << "Give image file on command line\n"; return; }
  vil_image im1 = vil_load(filename);
  if (!im1) { vcl_cerr << filename << " is not a valid image file\n"; return; }
  int size = im1.get_size_bytes();
  char* buf1 = new char[size];
  im1.get_section(buf1, 0, 0, im1.width(), im1.height());

  Image* im2 = vil_to_Image(im1);
  TEST("vil_to_Image width", im1.width(), im2->GetSizeX());
  TEST("vil_to_Image height", im1.height(), im2->GetSizeY());
  char* buf2 = new char[size];
  im2->GetSection(buf2, 0, 0, im1.width(), im1.height());
  TEST("vil_to_Image data", 0, memcmp(buf1, buf2, size));

  vil_image im3 = Image_to_vil(im2);
  TEST("Image_to_vil width", im3.width(), im2->GetSizeX())
  TEST("Image_to_vil height", im3.height(), im2->GetSizeY())
  im3.get_section(buf1, 0, 0, im1.width(), im1.height());
  TEST("Image_to_vil data", 0, memcmp(buf1, buf2, size))

  delete[] buf1; delete[] buf2;
}

int main(int argc, char* argv[])
{
  vnl_test_start("test_image_conversions");
  if (argc > 1) filename = argv[1];
  test_image_conversions();
  return vnl_test_summary();
}
