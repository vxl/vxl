// This is mul/vil2/tests/test_bilin_interp.cxx
#include <vcl_iostream.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_bilin_interp.h>
#include <vil/vil_byte.h>
#include <testlib/testlib_test.h>

void test_bilin_interp_byte()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing vil2_bilin_interp\n";
  vcl_cout << "*****************************\n";

  vil2_image_view<vil_byte> image0;
  image0.resize(5,5);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

   {
     double v1 = vil2_bilin_interp(3,3,image0.top_left_ptr(),image0.xstep(),image0.ystep());
     TEST_NEAR("vil2_bilin_interp at grid point",v1,33,1e-8);

     double v2 = vil2_bilin_interp(3.4,3,image0.top_left_ptr(),image0.xstep(),image0.ystep());
     TEST_NEAR("vil2_bilin_interp at off-grid point",v2,33.4,1e-8);
     double v3 = vil2_bilin_interp(3.4,3.5,image0.top_left_ptr(),image0.xstep(),image0.ystep());
     TEST_NEAR("vil2_bilin_interp at off-grid point",v3,38.4,1e-8);
   }

   {
     int nx = image0.nx();
     int ny = image0.ny();
     int xstep = image0.xstep();
     int ystep = image0.ystep();
     const vil_byte* data = image0.top_left_ptr();

     double v1 = vil2_safe_bilin_interp(3,3,data,nx,ny,xstep,ystep);
     TEST_NEAR("vil2_safe_bilin_interp at grid point",v1,33,1e-8);

     double v2 = vil2_safe_bilin_interp(3.4,3,data,nx,ny,xstep,ystep);
     TEST_NEAR("vil2_safe_bilin_interp at off-grid point",v2,33.4,1e-8);
     double v3 = vil2_safe_bilin_interp(3.4,3.5,data,nx,ny,xstep,ystep);
     TEST_NEAR("vil2_safe_bilin_interp at off-grid point",v3,38.4,1e-8);

     double v_outside = vil2_safe_bilin_interp(-1,-1,data,nx,ny,xstep,ystep);
     TEST_NEAR("vil2_safe_bilin_interp outside image",v_outside,0,1e-8);
   }
}

MAIN( test_bilin_interp )
{
  START( "Bilinear Interpolation" );

  test_bilin_interp_byte();
}

