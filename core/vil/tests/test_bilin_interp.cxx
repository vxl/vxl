// This is core/vil/tests/test_bilin_interp.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_bilin_interp.h>

static void test_bilin_interp_byte()
{
  vcl_cout << "**************************\n"
           << " Testing vil_bilin_interp\n"
           << "**************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(5,5);

  for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j) = i+j*10;

  {
    double v1 = vil_bilin_interp(image0,3,3);
    TEST_NEAR("vil_bilin_interp at grid point",v1,33,1e-8);
    double v2 = vil_bilin_interp(image0,3.4,3);
    TEST_NEAR("vil_bilin_interp at off-grid point",v2,33.4,1e-8);
    double v3 = vil_bilin_interp(image0,3.4,3.5);
    TEST_NEAR("vil_bilin_interp at off-grid point",v3,38.4,1e-8);
  }

  {
    double v1 = vil_bilin_interp_safe(image0,4,4);
    TEST_NEAR("vil_bilin_interp_safe at grid point",v1,44,1e-8);
    double v2 = vil_bilin_interp_safe(image0,3.4,3);
    TEST_NEAR("vil_bilin_interp_safe at off-grid point",v2,33.4,1e-8);
    double v3 = vil_bilin_interp_safe(image0,3.4,3.5);
    TEST_NEAR("vil_bilin_interp_safe at off-grid point",v3,38.4,1e-8);

    double v4 = vil_bilin_interp_safe(image0,5,2);
    TEST_NEAR("vil_bilin_interp_safe outside image",v4,0,1e-8);
    double v5 = vil_bilin_interp_safe(image0,4.001,1);
    TEST_NEAR("vil_bilin_interp_safe outside image",v5,0,1e-8);
    double v6 = vil_bilin_interp_safe(image0,1,4.001);
    TEST_NEAR("vil_bilin_interp_safe outside image",v6,0,1e-8);
  }
}

static void test_bilin_interp()
{
  test_bilin_interp_byte();
}

TESTMAIN(test_bilin_interp);
