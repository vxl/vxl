// This is core/vil1/tests/test_resample.cxx
// \author Peter Vanroose

#include <vil1/vil1_resample.h>

#include <vcl_iostream.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <testlib/testlib_test.h>

static char default_filename[] = "square.pgm";

template <class T> inline
T get_pixel(vil1_image const& i, int x, int y, T* /*dummy*/) { T t; i.get_section(&t,x,y,1,1); return t; }

static unsigned char* t = (unsigned char*)0;

static void test_resample(int argc, char* argv[])
{
  char* filename = argv[1];
  if (argc<2) {
    filename = default_filename;
    vcl_cerr << "Using default input image " << filename << '\n';
  }

  vil1_image a = vil1_load(filename);
  if ( !a ) {
    vcl_cerr << "Could not load image " << filename << '\n';
    testlib_test_perform(false);
    return;
  }

  int wd=a.width(), ht=a.height();

  if ( wd < 25 || ht < 31 ) {
    vcl_cerr << "Could not use this image " << filename << " since it is too small\n";
    testlib_test_perform(false);
    return;
  }
  if ( wd%2 || ht%2 ) {
    vcl_cerr << "Could not use this image " << filename << " since it has odd width or height\n";
    testlib_test_perform(false);
    return;
  }

  int a1 = get_pixel(a,  0,  0, t);
  int a11 = (a1 + get_pixel(a, 0, 1,t) + get_pixel(a, 1, 0,t) + get_pixel(a, 1, 1,t))/4;
  int a2 = get_pixel(a, 24, 16, t);
  int a22 = (a2 + get_pixel(a,24,17,t) + get_pixel(a,25,16,t) + get_pixel(a,25,17,t))/4;
  int a3 = get_pixel(a, 10, 38, t);
  int a33 = (a3 + get_pixel(a,10,39,t) + get_pixel(a,11,38,t) + get_pixel(a,11,39,t))/4;

  vcl_cout << "Simplest resampling: reduce the image by a factor 2 in both directions.\n";

  vil1_image b = vil1_resample(a,wd/2,ht/2);
  TEST("width", b.width(), wd/2);
  TEST("height", b.height(), ht/2);

  int b1 = get_pixel(b,  0,  0, t);
  int b2 = get_pixel(b, 12,  8, t);
  int b3 = get_pixel(b,  5, 19, t);
  TEST("subsampled pixel value", b1, a11); if (b1!=a11) vcl_cout<<b1<<"!="<<a11<<'\n';
  TEST("subsampled pixel value", b2, a22); if (b2!=a22) vcl_cout<<b2<<"!="<<a22<<'\n';
  TEST("subsampled pixel value", b3, a33); if (b3!=a33) vcl_cout<<b3<<"!="<<a33<<'\n';

  if (argc>2) vil1_save(b, argv[2]);

  vcl_cout << "Now enlarge the image by a factor 2 in both directions.\n";

  vil1_image c = vil1_resample(a,wd*2,ht*2);
  TEST("width", c.width(), wd*2);
  TEST("height", c.height(), ht*2);

  int c1 = get_pixel(c,  1,  1, t);
  int c2 = get_pixel(c, 49, 33, t);
  int c3 = get_pixel(c, 21, 77, t);
  TEST("subsampled pixel value", c1, a1); if (c1!=a1) vcl_cout<<c1<<"!="<<a1<<'\n';
  TEST("subsampled pixel value", c2, a2); if (c2!=a2) vcl_cout<<c2<<"!="<<a2<<'\n';
  TEST("subsampled pixel value", c3, a3); if (c3!=a3) vcl_cout<<c3<<"!="<<a3<<'\n';

  // Intermediate pixels were black (0), but are now equal to neighbour:
  c1 = get_pixel(c,  0,  0, t);
  c2 = get_pixel(c, 48, 32, t);
  c3 = get_pixel(c, 20, 76, t);
  TEST("intermediate pixel value", c1, a1); if (c1!=a1) vcl_cout<<c1<<"!="<<a1<<'\n';
  TEST("intermediate pixel value", c2, a2); if (c2!=a2) vcl_cout<<c2<<"!="<<a2<<'\n';
  TEST("intermediate pixel value", c3, a3); if (c3!=a3) vcl_cout<<c3<<"!="<<a3<<'\n';
  c1 = get_pixel(c,  1,  0, t);
  c2 = get_pixel(c, 49, 32, t);
  c3 = get_pixel(c, 21, 76, t);
  TEST("intermediate pixel value", c1, a1); if (c1!=a1) vcl_cout<<c1<<"!="<<a1<<'\n';
  TEST("intermediate pixel value", c2, a2); if (c2!=a2) vcl_cout<<c2<<"!="<<a2<<'\n';
  TEST("intermediate pixel value", c3, a3); if (c3!=a3) vcl_cout<<c3<<"!="<<a3<<'\n';
  c1 = get_pixel(c,  0,  1, t);
  c2 = get_pixel(c, 48, 33, t);
  c3 = get_pixel(c, 20, 77, t);
  TEST("intermediate pixel value", c1, a1); if (c1!=a1) vcl_cout<<c1<<"!="<<a1<<'\n';
  TEST("intermediate pixel value", c2, a2); if (c2!=a2) vcl_cout<<c2<<"!="<<a2<<'\n';
  TEST("intermediate pixel value", c3, a3); if (c3!=a3) vcl_cout<<c3<<"!="<<a3<<'\n';

  if (argc>3) vil1_save(c, argv[3]);
}

TESTMAIN_ARGS(test_resample);
