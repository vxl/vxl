// This is core/vil/tests/test_rotate_image.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_rotate.h>

static void test_rotate_image()
{
  vcl_cout << "**************************\n"
           << " Testing vil_rotate_image\n"
           << "**************************\n";

  const unsigned int ni = 55;
  const unsigned int nj = 35;
  vil_image_view<float> image, dest1, dest2;
  image.set_size(ni,nj);  image.fill(17.f);

  // Rotate 0 degrees:
  {
    vil_rotate_image(image,dest1,0);
    TEST("rotate 0 degrees: width",dest1.ni(),image.ni());
    TEST("rotate 0 degrees: height",dest1.nj(),image.nj());
    bool b = true; for (unsigned int i=0; i<ni; ++i) for (unsigned int j=0; j<nj; ++j) b = b && dest1(i,j)==image(i,j);
    TEST("rotate 0 degrees: content", b, true);
  }

  // Rotate -720 degrees:
  {
    vil_rotate_image(image,dest1,-720.0);
    TEST("rotate -720 degrees: width",dest1.ni(),image.ni());
    TEST("rotate -720 degrees: height",dest1.nj(),image.nj());
    bool b = true; for (unsigned int i=0; i<ni; ++i) for (unsigned int j=0; j<nj; ++j) b = b && dest1(i,j)==image(i,j);
    TEST("rotate 720 degrees: content", b, true);
  }

  unsigned int wd = (unsigned int)((ni+nj)/1.4142135623730950488);

  // Rotate 45 degrees:
  {
    vil_rotate_image(image,dest1,45.0);
    TEST("rotate 45 degrees: width",dest1.ni(),wd);
    TEST("rotate 45 degrees: height",dest1.nj(),wd);
  }

  // Rotate -1035 degrees:
  {
    vil_rotate_image(image,dest2,-1035.0);
    TEST("rotate -1035 degrees: width",dest2.ni(),wd);
    TEST("rotate -1035 degrees: height",dest2.nj(),wd);
    bool b = true; for (unsigned int i=0; i<wd; ++i) for (unsigned int j=0; j<wd; ++j) b = b && dest2(i,j)==dest1(i,j);
    TEST("rotate -1035 degrees: content", b, true);
  }
}


TESTMAIN(test_rotate_image);
