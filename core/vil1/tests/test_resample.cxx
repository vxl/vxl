// This is vxl/vil/tests/test_resample.cxx
// \author Peter Vanroose

#include <vil/vil_resample.h>

#include <vcl_iostream.h>

#include <vil/vil_load.h>
#include <testlib/testlib_test.h>

static char default_filename[] = "square.pgm";

template <class T> inline
T get_pixel(vil_image const& i, int x, int y, T* /*dummy*/) { T t; i.get_section(&t,x,y,1,1); return t; }

MAIN(test_resample)
{
  START("vil_resample");

  char* filename = argv[1];
  if (argc<2) {
    filename = default_filename;
    vcl_cerr << "Using default input image " << filename << '\n';
  }

  vil_image a = vil_load(filename);
  if ( !a ) {
    vcl_cerr << "Could not load image " << filename << '\n';
    return 1;
  }

  int wd=a.width(), ht=a.height();

  if ( wd < 25 || ht < 31 ) {
    vcl_cerr << "Could not use this image " << filename << " since it is too small\n";
    return 1;
  }

  unsigned char a1 = get_pixel(a,  0,  0, (unsigned char*)0);
  unsigned char a2 = get_pixel(a, 24, 16, (unsigned char*)0);
  unsigned char a3 = get_pixel(a, 10, 30, (unsigned char*)0);

  vcl_cout << "Simplest resampling: reduce the image by a factor 2 in both directions.\n";

  vil_image b = vil_resample(a,wd/2,ht/2);
  TEST("width", b.width(), wd/2);
  TEST("height", b.height(), ht/2);

  TEST("subsampled pixel value", get_pixel(b,  0,  0, (unsigned char*)0), a1);
  TEST("subsampled pixel value", get_pixel(b, 12,  8, (unsigned char*)0), a2);
  TEST("subsampled pixel value", get_pixel(b,  5, 15, (unsigned char*)0), a3);

  vcl_cout << "Now enlarge the image by a factor 2 in both directions.\n";

  vil_image c = vil_resample(a,wd*2,ht*2);
  TEST("width", c.width(), wd*2);
  TEST("height", c.height(), ht*2);

  TEST("upsampled pixel value", get_pixel(c,  1,  1, (unsigned char*)0), a1);
  TEST("upsampled pixel value", get_pixel(c, 49, 33, (unsigned char*)0), a2);
  TEST("upsampled pixel value", get_pixel(c, 21, 61, (unsigned char*)0), a3);

  // Intermediate pixels were black (0), but are now equal to neighbour:
  TEST("intermediate pixel value", get_pixel(c,  0,  0, (unsigned char*)0), a1);
  TEST("intermediate pixel value", get_pixel(c, 48, 32, (unsigned char*)0), a2);
  TEST("intermediate pixel value", get_pixel(c, 20, 60, (unsigned char*)0), a3);

  SUMMARY();
}
