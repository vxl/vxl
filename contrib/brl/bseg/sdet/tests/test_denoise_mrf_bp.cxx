// This is brl/bseg/sdet/tests/test_denoise_mrf_bp.cxx
#include <sdet/sdet_denoise_mrf_bp.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <testlib/testlib_test.h>

static void test_denoise_mrf_bp()
{
  vil_image_resource_sptr depth =
    vil_load_image_resource("C:/images/MeshStudies/Quantico/depth_med.tif");
  vil_image_resource_sptr var =
    vil_load_image_resource("C:/images/MeshStudies/Quantico/var_med.tif");
  sdet_denoise_mrf_bp_params dmbp;
  dmbp.n_iter_ = 3;
  dmbp.pyramid_levels_ = 5;
  dmbp.discontinuity_cost_ = 5.0;
  dmbp.truncation_cost_=400;
  dmbp.n_labels_ = 64;
  //dmbp.lambda_=0.000001f;
  dmbp.lambda_=0.0000003f;
  sdet_denoise_mrf_bp mbp(dmbp);
  mbp.set_image(depth);   mbp.set_variance(var);
  bool success = mbp.denoise();
  if (success) {
    vil_image_resource_sptr outr = mbp.output();
    vil_save_image_resource(outr,
                            "C:/images/MeshStudies/Quantico/belief_med.tif",
                            "tiff");
#if 0
    unsigned pi = 214, pj = 88;
    sdet_mrf_bp_sptr mrf = mbp.mrf();
    sdet_mrf_site_bp_sptr s = mrf->site(pi, pj);
    for (unsigned lab = 0; lab < dmbp.n_labels_; ++ lab)
      vcl_cout << s->prior_message(0,lab) << ' '
               << s->prior_message(1,lab) << ' '
               << s->prior_message(2,lab) << ' '
               << s->prior_message(3,lab) << '\n';
#endif
  }
}

TESTMAIN(test_denoise_mrf_bp);
