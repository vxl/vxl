// This is mul/vil2/tests/test_bilin_interp.cxx
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_bilin_interp.h>

void test_bilin_interp_byte()
{
  vcl_cout << "***************************\n";
  vcl_cout << " Testing vil2_bilin_interp\n";
  vcl_cout << "***************************\n";

  vil2_image_view<vxl_byte> image0;
  image0.resize(5,5);

  for (unsigned int y=0;y<image0.nj();++y)
     for (unsigned int x=0;x<image0.ni();++x)
       image0(x,y) = x+y*10;

   {
     double v1 = vil2_bilin_interp(image0,3,3);
     TEST_NEAR("vil2_bilin_interp at grid point",v1,33,1e-8);

     double v2 = vil2_bilin_interp(image0,3.4,3);
     TEST_NEAR("vil2_bilin_interp at off-grid point",v2,33.4,1e-8);
     double v3 = vil2_bilin_interp(image0,3.4,3.5);
     TEST_NEAR("vil2_bilin_interp at off-grid point",v3,38.4,1e-8);
   }

   {

     double v1 = vil2_bilin_interp_safe(image0,3,3);
     TEST_NEAR("vil2_bilin_interp_safe at grid point",v1,33,1e-8);

     double v2 = vil2_bilin_interp_safe(image0,3.4,3);
     TEST_NEAR("vil2_bilin_interp_safe at off-grid point",v2,33.4,1e-8);
     double v3 = vil2_bilin_interp_safe(image0,3.4,3.5);
     TEST_NEAR("vil2_bilin_interp_safe at off-grid point",v3,38.4,1e-8);

     double v_outside = vil2_bilin_interp_safe(image0,-1,-1);
     TEST_NEAR("vil2_bilin_interp_safe outside image",v_outside,0,1e-8);
   }
}

MAIN( test_bilin_interp )
{
  START( "Bilinear Interpolation" );

  test_bilin_interp_byte();

  SUMMARY();
}

