// This is mul/vil3d/tests/test_switch_axes.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_switch_axes.h>

void test_switch_axes_int()
{
  vcl_cout << "***************************\n"
           << " Testing vil3d_switch_axes\n"
           << "***************************\n";

  vil3d_image_view<int> image(5,5,5);
  for (int k=0;k<5;++k)
    for (int j=0;j<5;++j)
      for (int i=0;i<5;++i)
        image(i,j,k)=100*i+10*j+k;

  TEST("vil3d_switch_axes_ikj",vil3d_switch_axes_ikj(image)(1,3,2),123);
  TEST("vil3d_switch_axes_jik",vil3d_switch_axes_jik(image)(2,1,3),123);
  TEST("vil3d_switch_axes_jki",vil3d_switch_axes_jki(image)(2,3,1),123);
  TEST("vil3d_switch_axes_kij",vil3d_switch_axes_kij(image)(3,1,2),123);
  TEST("vil3d_switch_axes_kji",vil3d_switch_axes_kji(image)(3,2,1),123);
}


MAIN( test_switch_axes )
{
  START( "vil3d_switch_axes_*" );

  test_switch_axes_int();

  SUMMARY();
}
