// This is mul/vil3d/tests/test_tricub_interp.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_tricub_interp.h>
#include <vxl_config.h>


static void test_tricub_interp_float()
{
  std::cout << "*************************************\n"
           << " Testing vil3d_tricub_interp <float>\n"
           << "*************************************\n";

  unsigned ni = 7;
  unsigned nj = 7;
  unsigned nk = 7;
  vil3d_image_view<float> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned y=0;y<image0.nj();++y)
    for (unsigned x=0;x<image0.ni();++x)
      for (unsigned z=0;z<image0.nk();++z)
        image0(x,y,z) = x*0.1f+y+z*10;

  std::ptrdiff_t istep = image0.istep();
  std::ptrdiff_t jstep = image0.jstep();
  std::ptrdiff_t kstep = image0.kstep();

  {
    double v1 = vil3d_tricub_interp_raw(3,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at grid point",v1,33.3f,1e-5);

    double v2 = vil3d_tricub_interp_raw(3.4,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v2,33.34f,1e-5);
    double v3 = vil3d_tricub_interp_raw(3.4,3.5,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v3,33.84f,1e-5);
    double v4 = vil3d_tricub_interp_raw(3.4,3.5,3.5,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v4,38.84f,1e-5);
  }

  {
    double v1 = vil3d_tricub_interp_safe(3,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at grid point",v1,33.3f,1e-5);

    double v2 = vil3d_tricub_interp_safe(3.4,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v2,33.34f,1e-5);
    double v3 = vil3d_tricub_interp_safe(3.4,3.5,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v3,33.84f,1e-5);
    double v4 = vil3d_tricub_interp_safe(3.4,3.5,3.5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v4,38.84f,1e-5);

    double v_outside = vil3d_tricub_interp_safe(-1,-1,-1,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe outside image",v_outside,0,1e-9);
    double v_unsafe = vil3d_tricub_interp_safe(5,5,5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at unsafe point",v_unsafe,0,1e-9);
  }
}


static void test_tricub_interp_int()
{
  std::cout << "*******************************************\n"
           << " Testing vil3d_tricub_interp <vxl_uint_32>\n"
           << "*******************************************\n";

  unsigned ni = 7;
  unsigned nj = 7;
  unsigned nk = 7;
  vil3d_image_view<vxl_uint_32> image0;
  image0.set_size(ni,nj,nk);

  for (unsigned y=0;y<image0.nj();++y)
    for (unsigned x=0;x<image0.ni();++x)
      for (unsigned z=0;z<image0.nk();++z)
        image0(x,y,z) = x + y*10 + z*100;

  std::ptrdiff_t istep = image0.istep();
  std::ptrdiff_t jstep = image0.jstep();
  std::ptrdiff_t kstep = image0.kstep();

  {
    double v1 = vil3d_tricub_interp_raw(3,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at grid point",v1,333.0f,1e-5);

    double v2 = vil3d_tricub_interp_raw(3.4,3,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v2,333.4f,1e-5);

    double v3 = vil3d_tricub_interp_raw(3.4,3.5,3,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v3,338.4f,1e-5);

    double v4 = vil3d_tricub_interp_raw(3.4,3.5,3.5,image0.origin_ptr(),istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_raw at off-grid point",v4,388.4f,1e-5);
  }

  {
    double v1 = vil3d_tricub_interp_safe(3,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at grid point",v1,333.0f,1e-5);

    double v2 = vil3d_tricub_interp_safe(3.4,3,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v2,333.4f,1e-5);

    double v3 = vil3d_tricub_interp_safe(3.4,3.5,3,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v3,338.4f,1e-5);

    double v4 = vil3d_tricub_interp_safe(3.4,3.5,3.5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at off-grid point",v4,388.4f,1e-5);

    double v_outside = vil3d_tricub_interp_safe(-1,-1,-1,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe outside image",v_outside,0,1e-9);

    double v_unsafe = vil3d_tricub_interp_safe(5,5,5,image0.origin_ptr(),ni,nj,nk,istep,jstep,kstep);
    TEST_NEAR("vil3d_tricub_interp_safe at unsafe point",v_unsafe,0,1e-9);
  }
}


static void test_tricub_interp()
{
  test_tricub_interp_float();
  test_tricub_interp_int();
}

TESTMAIN(test_tricub_interp);
