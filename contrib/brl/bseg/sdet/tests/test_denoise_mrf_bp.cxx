// This is brl/bseg/sdet/tests/test_denoise_mrf_bp.cxx
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sdet/sdet_denoise_mrf_bp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <testlib/testlib_test.h>

static void test_denoise_mrf_bp()
{
#if 0
  vil_image_resource_sptr depth =

    vil_load_image_resource("e:/images/MeshStudies/FixedModelBug/z_image.tiff");

  vil_image_resource_sptr var =

    vil_load_image_resource("e:/images/MeshStudies/FixedModelBug/var_image.tiff");
  sdet_denoise_mrf_bp_params dmbp;
  dmbp.n_iter_ = 3;
  dmbp.pyramid_levels_ = 5;
  dmbp.discontinuity_cost_ = 5.0;
  dmbp.truncation_cost_=400;
  dmbp.n_labels_ = 64;

  //dmbp.lambda_=0.000001f;
  dmbp.lambda_=0.001f;

  sdet_denoise_mrf_bp mbp(dmbp);
  mbp.set_image(depth);   mbp.set_variance(var);
  bool success = mbp.denoise();
  if (success) {
    vil_image_resource_sptr outr = mbp.output();
    vil_save_image_resource(outr,
                            "e:/images/MeshStudies/FixedModelBug/belief_.001.tiff", "tiff");
  }
#endif
}
TESTMAIN(test_denoise_mrf_bp);
