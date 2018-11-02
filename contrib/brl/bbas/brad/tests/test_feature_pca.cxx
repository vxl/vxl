// This is brl/bbas/brad/tests/test_feature_pca.cxx

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_eigenspace.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_feature_pca()
{
  //Test feature vector functor
  vil_image_view<float> view(10, 10);
  view.fill(300.0f);
  brad_hist_prob_feature_vector fv(0.0f, 1000.0f, 5);
  vnl_vector<double> vec = fv(view);
  std::cout << vec << '\n';
  TEST("feature vector", vec[1], 1.0f);
  // test binary io
  vsl_b_ofstream bos("./temp.bin");
  if (!bos)
    return;
  vsl_b_write(bos , fv);
  bos.close();
  vsl_b_ifstream bis("./temp.bin");
  if (!bis)
    return;
  brad_hist_prob_feature_vector fv_in;
  vsl_b_read(bis, fv_in);
  unsigned nbins_in = fv_in.nbins();

  TEST("test feature_vector io", nbins_in, 5);
  vpl_unlink("./temp.bin");

  //Test eigenspace binary io
  brad_eigenspace<brad_hist_prob_feature_vector> tproc(3, 3, fv);
  vsl_b_ofstream eos("./etemp.bin");
  if (!eos)
    return;
  vsl_b_write(eos , tproc);
  eos.close();
  brad_eigenspace<brad_hist_prob_feature_vector> epin;
  vsl_b_ifstream eis("./etemp.bin");
  if (!eis)
    return;
  vsl_b_read(eis, epin);
  eis.close();
  brad_hist_prob_feature_vector eifv_in = epin.functor();
  double ne = epin.nib(), maxf = eifv_in.max(), nbf = eifv_in.nbins();
  double eper = std::fabs(ne-3.0)+ std::fabs(maxf-1000.0)+ std::fabs(nbf-5.0);
  TEST_NEAR("eigenspace", eper, 0, 0.0001);
  vpl_unlink("./etemp.bin");
}

TESTMAIN(test_feature_pca);
