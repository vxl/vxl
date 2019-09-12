// This is contrib/brl/bbas/bvgl/algo/tests/test_register.cxx
//:
// \file
// \brief Tests for bvgl_register_ptsets_3d
// \author J.L. Mundy
// \data Sept. 7, 2019

#include <iostream>
#include <testlib/testlib_test.h>
#include <vnl/vnl_random.h>
#include <bvgl/algo/bvgl_register_ptsets_3d_rigid.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_register()
{
  // generate a random ptset
  vnl_random rand;
  size_t n = 1000; //min number of points
  double min_c = 0, max_c = 10.0;
  vgl_pointset_3d<float> gt_ptset, test_ptset;
  for (size_t i = 0; i<n; ++i) {
    vgl_point_3d<float> p(static_cast<float>(rand.drand32(min_c, max_c)),
                          static_cast<float>(rand.drand32(min_c, max_c)),
                          static_cast<float>(rand.drand32(min_c, max_c)));
    gt_ptset.add_point(p);
  }
  // transform ptset by t
  vgl_vector_3d<float> t(1.0, -1.0, 1.5);
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<float>& p = gt_ptset.p(i);
    vgl_point_3d<float> tp = p + t;
    test_ptset.add_point(tp);
  }
  bvgl_register_ptsets_3d_rigid<float> reg(gt_ptset, test_ptset);
  bool good = reg.minimize();
  TEST("registration success", good, true);
  std::cout << "msq_error " << reg.min_error() << " trans at min "<< reg.t() << std::endl;
  vgl_vector_3d<float> tmin = reg.t();
  vgl_vector_3d<float> dif = t+tmin;
  float er = dif.length();
  TEST_NEAR("solve for translation", er, 0.0f, 0.5f);
}

TESTMAIN( test_register );
