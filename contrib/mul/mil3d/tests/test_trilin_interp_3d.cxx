// This is mul/mil3d/tests/test_trilin_interp_3d.cxx
#include <testlib/testlib_test.h>
#include <mil3d/mil3d_image_3d_of.h>
#include <mil3d/mil3d_trilin_interp_3d.h>
#include <vcl_iostream.h>

void test_trilin_interp_3d_float()
{
  vcl_cout << "********************************\n"
           << " Testing mil3d_trilin_interp_3d\n"
           << "********************************\n";

  int nx = 5;
  int ny = 5;
  int nz = 5;
  mil3d_image_3d_of<float> image0;
  image0.resize(nx,ny,nz);

  for (int y=0;y<image0.ny();++y)
    for (int x=0;x<image0.nx();++x)
      for (int z=0;z<image0.nz();++z)
        image0(x,y,z) = x*0.1f+y+z*10;

  int xstep = image0.xstep();
  int ystep = image0.ystep();
  int zstep = image0.zstep();

   {
     double v1 = mil3d_trilin_interp_3d(3,3,3,image0.plane(0),xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at grid point",v1,33.3f,1e-8);

     double v2 = mil3d_trilin_interp_3d(3.4,3,3,image0.plane(0),xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v2,33.34f,1e-8);
     double v3 = mil3d_trilin_interp_3d(3.4,3.5,3,image0.plane(0),xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v3,33.84f,1e-8);
     double v4 = mil3d_trilin_interp_3d(3.4,3.5,3.5,image0.plane(0),xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v4,38.84f,1e-8);
   }

   {
     double v1 = mil3d_safe_trilin_interp_3d(3,3,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at grid point",v1,33.3f,1e-8);

     double v2 = mil3d_safe_trilin_interp_3d(3.4,3,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v2,33.34f,1e-8);
     double v3 = mil3d_safe_trilin_interp_3d(3.4,3.5,3,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v3,33.84f,1e-8);
     double v4 = mil3d_safe_trilin_interp_3d(3.4,3.5,3.5,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST_NEAR("mil3d_trilin_interp_3d at off-grid point",v4,38.84f,1e-8);

     double v_outside = mil3d_safe_trilin_interp_3d(-1,-1,-1,image0.plane(0),nx,ny,nz,xstep,ystep,zstep);
     TEST_NEAR("mil3d_safe_trilin_interp_3d outside image",v_outside,0,1e-8);
   }
}

void test_trilin_interp_3d()
{
  test_trilin_interp_3d_float();
}


TESTLIB_DEFINE_MAIN(test_trilin_interp_3d);
