// This is mul/vil3d/tests/test_trilin_interp.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_trilin_interp.h>


static void test_trilin_interp_float()
{
  vcl_cout << "************************************\n"
           << " Testing vil3d_trilin_interp <float>\n"
           << "************************************\n";

  unsigned ni = 5;
  unsigned nj = 5;
  unsigned nk = 5;
  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned y=0;y<image0.nj();++y)
    for (unsigned x=0;x<image0.ni();++x)
      for (unsigned z=0;z<image0.nk();++z)
        image0(x,y,z) = x*0.1f+y+z*10;

  vcl_ptrdiff_t istep = image0.istep();
  vcl_ptrdiff_t jstep = image0.jstep();
  vcl_ptrdiff_t kstep = image0.kstep();

  {
    double v1 = vil3d_trilin_interp_raw(3,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at grid point",v1,33.3f,1e-5);

    double v2 = vil3d_trilin_interp_raw(3.4,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v2,33.34f,1e-5);
    double v3 = vil3d_trilin_interp_raw(3.4,3.5,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v3,33.84f,1e-5);
    double v4 = vil3d_trilin_interp_raw(3.4,3.5,3.5,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v4,38.84f,1e-5);
  }

  {
    double v1 = vil3d_trilin_interp_safe(3,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at grid point",v1,33.3f,1e-5);

    double v2 = vil3d_trilin_interp_safe(3.4,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v2,33.34f,1e-5);
    double v3 = vil3d_trilin_interp_safe(3.4,3.5,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v3,33.84f,1e-5);
    double v4 = vil3d_trilin_interp_safe(3.4,3.5,3.5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_trilin_interp at off-grid point",v4,38.84f,1e-5);

    double v_outside = vil3d_trilin_interp_safe(-1,-1,-1,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_safe_trilin_interp outside image",v_outside,0,1e-5);
  }
}


static void test_trilin_interp_int()
{
  vcl_cout << "**********************************\n"
           << " Testing vil3d_trilin_interp <int>\n"
           << "**********************************\n";
  
  unsigned ni = 5;
  unsigned nj = 5;
  unsigned nk = 5;
  vil3d_image_view<int> image0;
  image0.set_size(ni,nj,nk);
  
  for (unsigned y=0;y<image0.nj();++y)
    for (unsigned x=0;x<image0.ni();++x)
      for (unsigned z=0;z<image0.nk();++z)
        image0(x,y,z) = x + y*10 + z*100;
      
      vcl_ptrdiff_t istep = image0.istep();
      vcl_ptrdiff_t jstep = image0.jstep();
      vcl_ptrdiff_t kstep = image0.kstep();
      
      {
        double v1 = vil3d_trilin_interp_raw(3,3,3,image0.origin_ptr(),istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at grid point",v1,333.0f,1e-5);
        
        double v2 = vil3d_trilin_interp_raw(3.4,3,3,image0.origin_ptr(),istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v2,333.4f,1e-5);

        double v3 = vil3d_trilin_interp_raw(3.4,3.5,3,image0.origin_ptr(),istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v3,338.4f,1e-5);
        
        double v4 = vil3d_trilin_interp_raw(3.4,3.5,3.5,image0.origin_ptr(),istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v4,388.4f,1e-5);
      }
      
      {
        double v1 = vil3d_trilin_interp_safe(3,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at grid point",v1,333.0f,1e-5);
        
        double v2 = vil3d_trilin_interp_safe(3.4,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v2,333.4f,1e-5);

        double v3 = vil3d_trilin_interp_safe(3.4,3.5,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v3,338.4f,1e-5);

        double v4 = vil3d_trilin_interp_safe(3.4,3.5,3.5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
        TEST_NEAR("vil3d_trilin_interp at off-grid point",v4,388.4f,1e-5);
        
        double v_outside = vil3d_trilin_interp_safe(-1,-1,-1,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
        TEST_NEAR("vil3d_safe_trilin_interp outside image",v_outside,0,1e-5);
      }
}


static void test_trilin_interp()
{
  test_trilin_interp_float();
  test_trilin_interp_int();
}

TESTMAIN(test_trilin_interp);
