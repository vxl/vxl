#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_bilin_interp_2d.h>
#include <vil/vil_byte.h>
#include <vcl_cmath.h> // for fabs()

void test_bilin_interp_2d_byte()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing mil_bilin_interp_2d\n";
  vcl_cout << "*****************************\n";

  mil_image_2d_of<vil_byte> image0;
  image0.resize(5,5);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
     {
       image0(x,y) = x+y*10;
     }

   {
     double v1 = mil_bilin_interp_2d(3,3,image0.plane(0),image0.ystep());
     TEST("mil_bilin_interp_2d at grid point",vcl_fabs(v1-33)<1e-8,true);
     v1 = mil_bilin_interp_2d(3,3,image0.plane(0),image0.xstep(),image0.ystep());
     TEST("mil_bilin_interp_2d at grid point",vcl_fabs(v1-33)<1e-8,true);

     double v2 = mil_bilin_interp_2d(3.4,3,image0.plane(0),image0.ystep());
     TEST("mil_bilin_interp_2d at off-grid point",vcl_fabs(v2-33.4)<1e-8,true);
     double v3 = mil_bilin_interp_2d(3.4,3.5,image0.plane(0),image0.ystep());
     TEST("mil_bilin_interp_2d at off-grid point",vcl_fabs(v3-38.4)<1e-8,true);
   }

   {
     int nx = image0.nx();
     int ny = image0.ny();
     int xstep = image0.xstep();
     int ystep = image0.ystep();
     const vil_byte* data = image0.plane(0);

     double v1 = mil_safe_bilin_interp_2d(3,3,data,nx,ny,ystep);
     TEST("mil_safe_bilin_interp_2d at grid point",vcl_fabs(v1-33)<1e-8,true);
     v1 = mil_safe_bilin_interp_2d(3,3,data,nx,ny,xstep,ystep);
     TEST("mil_safe_bilin_interp_2d at grid point",vcl_fabs(v1-33)<1e-8,true);

     double v2 = mil_safe_bilin_interp_2d(3.4,3,data,nx,ny,ystep);
     TEST("mil_safe_bilin_interp_2d at off-grid point",vcl_fabs(v2-33.4)<1e-8,true);
     double v3 = mil_safe_bilin_interp_2d(3.4,3.5,data,nx,ny,ystep);
     TEST("mil_safe_bilin_interp_2d at off-grid point",vcl_fabs(v3-38.4)<1e-8,true);

     double v_outside = mil_safe_bilin_interp_2d(-1,-1,data,nx,ny,ystep);
     TEST("mil_safe_bilin_interp_2d outside image",vcl_fabs(v_outside)<1e-8,true);
   }
}

void test_bilin_interp_2d()
{
  test_bilin_interp_2d_byte();
}


TESTMAIN(test_bilin_interp_2d);
