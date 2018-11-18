#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_gauss_sf1.h>
#include <vil/vil_image_view.h>
#include <bbgm/pro/bbgm_neighborhood_functors.h>

static void test_neighborhood_ops()
{
  unsigned ni = 7, nj = 7;
  vil_image_view<float> p(ni,nj);
  p.fill(0.0f);
  p(2,2) = 0.1f;   p(2,4) = 0.1f;
  p(4,2) = 0.1f;   p(4,4) = 0.1f;
  p(2,3) = 0.25f;  p(3,2) = 0.25f;
  p(3,4) = 0.25f;  p(4,3) = 0.25f;
  p(3,3) = 1.0f;
  bbgm_image_of<bsta_gauss_sf1> prop_dist;
  bbgm_neighborhood_area<bsta_gauss_sf1> neighbrhd_op;
  property_dist(p, prop_dist, neighbrhd_op, 2);
  std::cout << " mean " << prop_dist(3,3).mean() << '\n'
           << " variance " << prop_dist(3,3).var() << '\n';
  float er = std::fabs(prop_dist(3,3).mean()-2.4f);
  er += std::fabs(prop_dist(3,3).var()-1.11f);
  TEST_NEAR("area_distribution", er, 0, 1e-03);
}

TESTMAIN(test_neighborhood_ops);
