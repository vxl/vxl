// This is mul/vil3d/tests/test_from_image_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vil3d/vil3d_from_image_2d.h>


//========================================================================
//
//========================================================================
static void test_from_image_2d_1()
{
  unsigned ni=5;
  unsigned nj=6;
  unsigned nk=1;
  unsigned nplanes=1;
  vil_image_view<vxl_int_32> img2d(ni, nj);
  for (unsigned int j=0; j<nj; ++j)
    for (unsigned int i=0; i<ni; ++i)
      img2d(i,j) = 10*i + j;

  vil3d_image_view<vxl_int_32> img3d = vil3d_from_image_2d(img2d);
  TEST("ni correct", img3d.ni(), ni);
  TEST("nj correct", img3d.nj(), nj);
  TEST("nk correct", img3d.nk(), nk);
  TEST("np correct", img3d.nplanes(), nplanes);

  TEST("Pixel (3,2,0) correct", img3d(3,2,0), 32);
  TEST("Pixel (1,5,0) correct", img3d(1,5,0), 15);
}


//========================================================================
//
//========================================================================
static void test_from_image_2d()
{
  vcl_cout << "*****************************\n"
           << " Testing vil3d_from_image_2d\n"
           << "*****************************\n";

  test_from_image_2d_1();
}


//========================================================================
//
//========================================================================
TESTMAIN(test_from_image_2d);

