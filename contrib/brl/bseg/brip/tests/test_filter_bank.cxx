// This is brl/bseg/brip/tests/test_filter_bank.cxx

#include <vil/vil_image_view.h>
#include <brip/brip_filter_bank.h>
#include <brip/brip_vil_float_ops.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
static void test_filter_bank()
{
  unsigned ni = 500, nj = 500;
  vil_image_view<float> view(ni, nj);
  view.fill(0.0f);
  for(unsigned j = 0; j<nj; ++j)
    for(unsigned i = 0; i<ni; ++i)
      if(i>=(10+j)&&i<=(20+j))
        view(i,j) = 1.0f;
  unsigned n_levels = 3;
  double scale_range = 32.0;
  float lambda0 = 3.0f, lambda1 = 1.2f, theta_interval = 30.0f;
  float cutoff_ratio = 0.01f;
  brip_filter_bank bnk(n_levels, scale_range, lambda0, lambda1, theta_interval,
                       cutoff_ratio, view);
  vcl_cout << bnk;
  vil_image_view<float>& resp = bnk.response(n_levels-1);
  float v = resp(ni/2, nj/2);
  TEST_NEAR("filter_bank response", v, -0.016099010f, 0.001f);
  unsigned band = bnk.invalid_border();
  vcl_cout << " Interval:" << band << '\n';
  TEST("invalid_border", band, 259);
}
TESTMAIN(test_filter_bank);
