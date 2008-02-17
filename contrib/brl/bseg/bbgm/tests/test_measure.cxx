#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <bbgm/bbgm_update.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_basic_functors.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_vector_fixed.h>
#include <bbgm/bbgm_measure.h>


void init_random_image(vil_image_view<float>& img)
{
  vnl_random rand;
  for (unsigned int p=0; p<img.nplanes(); ++p)
    for (unsigned int j=0; j<img.nj(); ++j)
      for (unsigned int i=0; i<img.ni(); ++i)
        img(i,j,p) = static_cast<float>(rand.drand32());
}


MAIN( test_measure )
{
  const unsigned window_size = 50;
  const unsigned int max_components = 3;
  const float init_var = 0.1f;
  const float g_thresh = 3.0f;
  const float min_stdev = 0.02f;
  const unsigned int ni = 2, nj = 3;

  vnl_vector_fixed<float,3> init_mean(0.0f);
  vnl_vector_fixed<float,3> init_covar(init_var);

  vil_image_view<float> img(ni,nj,3);
  init_random_image(img);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture<gauss_type> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  bsta_gauss_if3 init_gauss( init_mean, init_covar );
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components,
                                                  g_thresh,
                                                  min_stdev,
                                                  window_size);

  bbgm_image_of<obs_mix_gauss_type> model(ni,nj,obs_mix_gauss_type());

  vul_timer time;
  update(model,img,updater);
  double up_time = time.real() / 1000.0;
  vcl_cout << " updated in " << up_time << " sec" <<vcl_endl;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      obs_mix_gauss_type& om = model(i, j);
      gauss_type g =  om.distribution(0);
      vcl_cout << "weight:"<< om.weight(0) << " mean(" << g.mean() << ") dcvar(" << g.diag_covar() <<")\n";
    }
  bsta_probability_functor<mix_gauss_type> functor_;
  vil_image_view<float> result;
  float delta = 0.01f;
  measure(model, img, result, functor_, delta);
  for (unsigned j = 0; j<nj; ++j)
  {
    for (unsigned i = 0; i<ni; ++i)
      vcl_cout << result(i,j) << ' ';
    vcl_cout << vcl_endl;
  }
  TEST_NEAR("measure probability", 1.60547e-005, result(0,0), 1.60547e-008);
  SUMMARY();
}


