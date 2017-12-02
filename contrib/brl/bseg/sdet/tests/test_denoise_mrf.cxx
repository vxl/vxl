// This is brl/bseg/sdet/tests/test_denoise_mrf.cxx
#include <testlib/testlib_test.h>
#include <sdet/sdet_denoise_mrf.h>
#include <sdet/sdet_denoise_mrf_params.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

void test_denoise_mrf()
{
  unsigned int ni = 5, nj = 5;
  vil_image_view<float> input(ni,nj);
  vil_image_view<float> var(ni,nj);
  input.fill(5.0f);
  input(2,2) = 20.0f;
  var.fill(0.001f);
  var(2,2) = 0.02f;  var(1,2) = 0.02f;  var(3,2) = 0.02f;
  var(1,1) = 0.02f;  var(2,1) = 0.02f;  var(3,1) = 0.02f;
  var(1,3) = 0.02f;  var(2,3) = 0.02f;  var(3,3) = 0.02f;
  sdet_denoise_mrf_params dmp;
  dmp.radius_ = 1.0;
  dmp.kappa_ = 100.0;
  dmp.beta_ = 0.0;
  sdet_denoise_mrf dm(dmp);
  vil_image_resource_sptr in_resc = vil_new_image_resource_of_view(input);
  vil_image_resource_sptr var_resc = vil_new_image_resource_of_view(var);
  dm.set_image(in_resc);    dm.set_variance(var_resc);
  bool success = dm.denoise();
  TEST("bool success = dm.denoise();",success,true);
  vil_image_resource_sptr outr = dm.output();
  vil_image_view<float> out_v = outr->get_view();
  for (unsigned int i = 0; i<ni; ++i)
    std::cout << "Fi[" << i << "] = " << out_v(i,2) << '\n';
  TEST_NEAR("center output pixel", out_v(2,2), 5.72894, 0.01);
}

TESTMAIN(test_denoise_mrf);
