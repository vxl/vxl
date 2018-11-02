// This is core/vil/tests/test_nearest_interp.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_nearest_interp.h>

static void test_nearest_interp_byte()
{
  std::cout << "****************************\n"
           << " Testing vil_nearest_interp\n"
           << "****************************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(5,5);

  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      image0(i,j) = vxl_byte(i+j*10);

  double v;

  v = vil_nearest_interp(image0,3,3);
  TEST("vil_nearest_interp at grid point",v,33);
  v = vil_nearest_interp(image0,2.4,3);
  TEST("vil_nearest_interp at off-grid point",v,32); // was 32.4
  v = vil_nearest_interp(image0,2,1.6);
  TEST("vil_nearest_interp at off-grid point",v,22); // was 18.0
  v = vil_nearest_interp(image0,2.4,2.5);
  TEST("vil_nearest_interp at off-grid point",v,32); // was 27.4

  v = vil_nearest_interp_safe(image0,3,3);
  TEST("vil_nearest_interp_safe at grid point",v,33);
  v = vil_nearest_interp_safe(image0,2.4,3);
  TEST("vil_nearest_interp_safe at off-grid point",v,32); // was 32.4
  v = vil_nearest_interp_safe(image0,2,1.6);
  TEST("vil_nearest_interp_safe at off-grid point",v,22); // was 18.0
  v = vil_nearest_interp_safe(image0,2.4,2.5);
  TEST("vil_nearest_interp_safe at off-grid point",v,32); // was 27.4

  v = vil_nearest_interp_safe(image0,5,2);
  TEST("vil_nearest_interp_safe outside image",v,0);
  v = vil_nearest_interp_safe(image0,4.500,1);
  TEST("vil_nearest_interp_safe outside image",v,0);
  v = vil_nearest_interp_safe(image0,1,4.500);
  TEST("vil_nearest_interp_safe outside image",v,0);
}

static void test_nearest_interp()
{
  test_nearest_interp_byte();
}

TESTMAIN(test_nearest_interp);
