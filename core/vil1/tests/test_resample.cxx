// This is vxl/vil/tests/test_resample.cxx
// \author Peter Vanroose

#include <vil/vil_load.h>
#include <vil/vil_resample.h>

#include <vcl_iostream.h>

#include <testlib/testlib_test.h>

static char default_filename[] = "square.pgm";

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

  char aa[3];
  a.get_section(aa,  0, 0, 1,1);
  a.get_section(aa+1,24,16,1,1);
  a.get_section(aa+2,10,30,1,1);

  int wd=a.width(), ht=a.height();

  vil_image b = vil_resample(a,wd/2,ht/2);
  TEST("width", b.width(), wd/2);
  TEST("height", b.height(), ht/2);

  char bb[3];
  b.get_section(bb,  0, 0, 1,1);
  b.get_section(bb+1,12,8, 1,1);
  b.get_section(bb+2,5, 15,1,1);

  TEST("subsampled pixel values", bb[0]==aa[0] && bb[1]==aa[1] && bb[2]==aa[2], true);

  vil_image c = vil_resample(a,wd*2,ht*2);
  TEST("width", c.width(), wd*2);
  TEST("height", c.height(), ht*2);

  c.get_section(bb,  1, 1, 1,1);
  c.get_section(bb+1,49,33,1,1);
  c.get_section(bb+2,21,61,1,1);

  TEST("upsampled pixel values", bb[0]==aa[0] && bb[1]==aa[1] && bb[2]==aa[2], true);

  // Intermediate pixels are black (0):
  c.get_section(bb,  0, 0, 1,1);
  c.get_section(bb+1,48,32,1,1);
  c.get_section(bb+2,20,60,1,1);

  TEST("intermediate pixel values", bb[0]==0 && bb[1]==0 && bb[2]==0, true);

  SUMMARY();
}
