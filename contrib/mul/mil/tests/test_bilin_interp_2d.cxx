// This is mul/mil/tests/test_bilin_interp_2d.cxx
#include <vcl_iostream.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_bilin_interp_2d.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

void test_bilin_interp_2d_byte()
{
  vcl_cout << "*****************************\n"
           << " Testing mil_bilin_interp_2d\n"
           << "*****************************\n";

  mil_image_2d_of<vxl_byte> image0;
  image0.resize(5,5);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

   {
     double v1 = mil_bilin_interp_2d(3,3,image0.plane(0),image0.ystep());
     TEST_NEAR("mil_bilin_interp_2d at grid point",v1,33,1e-8);
     v1 = mil_bilin_interp_2d(3,3,image0.plane(0),image0.xstep(),image0.ystep());
     TEST_NEAR("mil_bilin_interp_2d at grid point",v1,33,1e-8);

     double v2 = mil_bilin_interp_2d(3.4,3,image0.plane(0),image0.ystep());
     TEST_NEAR("mil_bilin_interp_2d at off-grid point",v2,33.4,1e-8);
     double v3 = mil_bilin_interp_2d(3.4,3.5,image0.plane(0),image0.ystep());
     TEST_NEAR("mil_bilin_interp_2d at off-grid point",v3,38.4,1e-8);
   }

   {
     int nx = image0.nx();
     int ny = image0.ny();
     int xstep = image0.xstep();
     int ystep = image0.ystep();
     const vxl_byte* data = image0.plane(0);

     double v1 = mil_safe_bilin_interp_2d(3,3,data,nx,ny,ystep);
     TEST_NEAR("mil_safe_bilin_interp_2d at grid point",v1,33,1e-8);
     v1 = mil_safe_bilin_interp_2d(3,3,data,nx,ny,xstep,ystep);
     TEST_NEAR("mil_safe_bilin_interp_2d at grid point",v1,33,1e-8);

     double v2 = mil_safe_bilin_interp_2d(3.4,3,data,nx,ny,ystep);
     TEST_NEAR("mil_safe_bilin_interp_2d at off-grid point",v2,33.4,1e-8);
     double v3 = mil_safe_bilin_interp_2d(3.4,3.5,data,nx,ny,ystep);
     TEST_NEAR("mil_safe_bilin_interp_2d at off-grid point",v3,38.4,1e-8);

     double v_outside = mil_safe_bilin_interp_2d(-1,-1,data,nx,ny,ystep);
     TEST_NEAR("mil_safe_bilin_interp_2d outside image",v_outside,0,1e-8);
   }
}

void test_bilin_interp_2d()
{
  test_bilin_interp_2d_byte();
}


TESTLIB_DEFINE_MAIN(test_bilin_interp_2d);
