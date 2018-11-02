#include <string>
#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>

#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/algo/bsta_fit_gaussian.h>

#include <bsta/bsta_histogram.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

MAIN( test_rand_sampling)
{
  START ("test_fit_gaussian");
  vnl_random rand_gen;
  unsigned nbins = 4;
  float delta = 1.0f/nbins;
  bsta_histogram<float> h(0.0f,1.0f, nbins);
  bsta_histogram<float> ho(nbins-1, 0.5f*delta, delta);
  float vis = 0.75f;
  float f = 0.2f;
  const unsigned int n_samples = 24, n_trials = 40000;
  auto ns = static_cast<unsigned>(f*n_samples + 0.5);
  bsta_histogram<float> hc(0.0f,static_cast<float>(n_samples),2*n_samples);
  for (unsigned j = 0; j<n_trials; ++j) {
  // generate samples
  std::vector<float> samples;
  for (unsigned int n=0; n<ns; ++n) {
    auto sample = (float)rand_gen.drand32(0.35, 0.6);
    samples.push_back(sample);
   h.upcount(sample, vis);
   ho.upcount(sample, vis);
  }
  for ( unsigned n = 0; n<(n_samples-ns); ++n) {
    auto sample = (float)rand_gen.drand32(0.0, 1.0);
    samples.push_back(sample);
    h.upcount(sample, 1.0f-vis);
    ho.upcount(sample,1.0f-vis);
  }

  float nmax = 0, nmaxo = 0;
  for (unsigned b = 0; b<nbins; ++b)
    if (h.counts(b)>nmax) {
      nmax = h.counts(b);
    }
  for (unsigned b = 0; b<nbins-1; ++b)
    if (ho.counts(b)>nmaxo) {
      nmaxo = ho.counts(b);
    }
  if (nmax>nmaxo)
    hc.upcount(nmax, 1.0f);
  else
    hc.upcount(nmaxo, 1.0f);

   h.clear(); ho.clear();
   samples.clear();
  }
  std::cout << "\n histogram of bin population\n";
  for (unsigned b = 0; b<hc.nbins(); ++b)
    std::cout << hc.avg_bin_value(b) << ' ' << hc.counts(b) << '\n';

  return 0;
}
