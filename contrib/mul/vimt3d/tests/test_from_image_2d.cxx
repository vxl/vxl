// This is mul/vimt3d/tests/test_from_image_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vimt3d/vimt3d_from_image_2d.h>


//========================================================================
//
//========================================================================
static void test_from_image_2d_1()
{
  unsigned ni=5;
  unsigned nj=6;
  unsigned nk=1;
  unsigned nplanes=1;
  vil_image_view<vxl_int_32> imview2d(ni, nj);
  for (int j=0; j<nj; ++j)
    for (int i=0; i<ni; ++i)
      imview2d(i,j) = 10*i + j;
  vimt_transform_2d transf2d;
  transf2d.set_zoom_only(2.0, 1.0, 0.0, 0.0);
  vimt_image_2d_of<vxl_int_32> img2d(imview2d, transf2d);

  
    
  vimt3d_image_3d_of<vxl_int_32> img3d = vimt3d_from_image_2d(img2d);
  TEST("ni correct", img3d.image().ni(), ni);
  TEST("nj correct", img3d.image().nj(), nj);
  TEST("nk correct", img3d.image().nk(), nk);
  TEST("np correct", img3d.image().nplanes(), nplanes);
 
//  TEST("Pixel (3,2,0) correct", img3d(3,2,0), 32);
//  TEST("Pixel (1,5,0) correct", img3d(1,5,0), 15);  
}


//========================================================================
//
//========================================================================
static void test_from_image_2d()
{
  vcl_cout << "***************************\n"
           << " Testing vimt3d_from_image_2d\n"
           << "***************************\n";
  
  test_from_image_2d_1();
}


//========================================================================
//
//========================================================================
TESTMAIN(test_from_image_2d);

