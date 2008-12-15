#include <testlib/testlib_test.h>

#include <brec/brec_fg_pair_density.h>
#include <brec/brec_fg_bg_pair_density.h>

#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

static void test_brec_densities()
{
  brec_fg_pair_density fd;
  fd.set_sigma(0.1);
  TEST_NEAR("testing fg pair density ", fd(0,0), 4.33533, 0.001);
  TEST_NEAR("testing fg pair density ", fd(0.5,0.8), 0.0481612, 0.001);
  TEST_NEAR("testing fg pair density gradient of log ", fd.gradient_of_log(0,0), -9.13293, 0.001);
  TEST_NEAR("testing fg pair density gradient of log ", fd.gradient_of_log(0.5,0.8), 80.8671, 0.001);

  brec_fg_bg_pair_density fbd;
  fbd.set_sigma(0.1);
  TEST_NEAR("testing fg bg pair density ", fbd(0,0), 0.0, 0.1);
  TEST_NEAR("testing fg bg pair density ", fbd(0.5,0.8), 1.28538, 0.001);

  vil_image_view<float> obs(100, 100);
  obs.fill(0.0f);
  for (unsigned i = 20; i < 40; i++)
    for (unsigned j = 20; j < 40; j++)
      obs(i,j) = 1.0f;

  vil_image_view<float> map = fd.prob_density(obs);
  float minv, maxv;
  vil_math_value_range(map, minv, maxv);
  vil_image_view<vxl_byte> map_b(map.ni(), map.nj());
  vil_convert_stretch_range_limited(map, map_b, 0.0f, maxv);
  vil_save(map_b, "./map_out.png");
}

TESTMAIN( test_brec_densities );
