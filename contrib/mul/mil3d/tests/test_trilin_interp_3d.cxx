#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <mil3d/mil3d_image_3d_of.h>
#include <mil3d/mil3d_trilin_interp_3d.h>
#include <vcl_cmath.h> // for fabs()

void test_trilin_interp_3d_float()
{
  vcl_cout << "*****************************\n";
  vcl_cout << " Testing mil3d_trilin_interp_3d\n";
  vcl_cout << "*****************************\n";

  int nx = 5;
  int ny = 5;
  int nz = 5;
  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      for (int z=0;z<image0.nz();++z)
      {
        image0(x,y,z) = x+y*10+z*100;
      }
  int xstep = image0.xstep();
  int ystep = image0.ystep();
  int zstep = image0.zstep();

   {
     double v1 = mil3d_trilin_interp_3d(3,3,3,image0.plane(0),xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at grid point",vcl_fabs(v1-333)<1e-8,true);

     double v2 = mil3d_trilin_interp_3d(3.4,3,3,image0.plane(0),xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v2-333.4)<1e-8,true);
     double v3 = mil3d_trilin_interp_3d(3.4,3.5,3,image0.plane(0),xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v3-338.4)<1e-8,true);
     double v4 = mil3d_trilin_interp_3d(3.4,3.5,3.5,image0.plane(0),xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v4-388.4)<1e-8,true);
   }

   {
     double v1 = mil3d_safe_trilin_interp_3d(3,3,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at grid point",vcl_fabs(v1-333)<1e-8,true);

     double v2 = mil3d_safe_trilin_interp_3d(3.4,3,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v2-333.4)<1e-8,true);
     double v3 = mil3d_safe_trilin_interp_3d(3.4,3.5,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v3-338.4)<1e-8,true);
     double v4 = mil3d_safe_trilin_interp_3d(3.4,3.5,3.5,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST("mil3d_trilin_interp_3d at off-grid point",vcl_fabs(v4-388.4)<1e-8,true);

     double v_outside = mil3d_safe_trilin_interp_3d(-1,-1,-1,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST("mil3d_safe_trilin_interp_3d outside image",vcl_fabs(v_outside)<1e-8,true);
   }
}

void test_trilin_interp_3d()
{
  test_trilin_interp_3d_float();
}


TESTMAIN(test_trilin_interp_3d);
