#include <string>
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>

#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_beta.h>
#include <bsta/algo/bsta_beta_updater.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void load_samples(const std::string& file, std::vector<float>& samples)
{
  std::ifstream is(file.data());
  while (is.is_open() && !is.eof()) {
    float sample;
    is >> sample;
    samples.push_back(sample);
  }
}

void random_sampling(std::vector<float> &samples, std::vector<float> in_samples,
                     int num, bsta_beta<float>& beta)
{
  std::vector<float> s;
  vnl_random rand;
  for (int i=0; i<num; ++i) {
    int index=rand.lrand32(0,in_samples.size()-1);
    samples.push_back(in_samples[index]);
    s.push_back(in_samples[index]);
  }
  bsta_beta<float> b(s);
  beta.set_alpha_beta(b.alpha(),b.beta());
  std::cout << beta;
}

float compare_betas(bsta_beta<float>& beta1, bsta_beta<float>& beta2)
{
  float val=0, inc=0.01f, total_diff=0;
  while (val < 1.0f) {
    float diff = beta1.prob_density(val)-beta2.prob_density(val);
    diff*=diff;
    total_diff += diff;
    val+=inc;
  }
  return total_diff;
}

static void test_beta_updater()
{
  std::cout << "-------------------------------\n"
           << " float, 1-dimensional, 3-modal\n"
           << "-------------------------------" <<std::endl;

  //A tri-mixture of 1 dimensional beta distribution
  typedef bsta_num_obs<bsta_beta<float> > beta_type;
  typedef bsta_mixture_fixed<beta_type, 3> mix_beta;
  typedef bsta_num_obs<mix_beta> mix_beta_type;

  bsta_beta<float> beta(100.0f, 100.0f);
  bsta_num_obs<bsta_beta<float> > init_beta(beta,1);

  // single distribution update
  bsta_beta_updater<bsta_beta<float> > updater;

  std::ifstream is("beta_distr_100_100.txt");
  double alpha_=100, beta_=100;
  while (is.is_open() && !is.eof()) {
    float sample;
    is >> sample;
    beta_type::vector_type obs(sample);
    updater(init_beta, obs);
  }

  std::cout << init_beta;
  TEST_NEAR("extracting alpha and beta from samples (100,100) - alpha", init_beta.alpha(), alpha_ ,10.0f);
  TEST_NEAR("extracting alpha and beta from samples (100,100) - beta ", init_beta.beta(), beta_ ,10.0f);

  mix_beta mix;
  bsta_beta<float> beta1;
  bsta_num_obs<mix_beta_type> model;

  bsta_mix_beta_updater<mix_beta> mix_updater(beta1,0.5f,3);
  std::vector<float> samples100_100, samples10_100, samples100_10, samples;
  load_samples("beta_distr_100_100.txt", samples100_100);
  load_samples("beta_distr_10_100.txt", samples10_100);
  load_samples("beta_distr_100_10.txt", samples100_10);

#if 0 // this test does not work
  // get random samples to feed the mixture
  bsta_beta<float> beta100_100, beta10_100, beta100_10;
  random_sampling(samples, samples100_100, 1000, beta100_100);
  random_sampling(samples, samples10_100, 1000, beta10_100);
  random_sampling(samples, samples100_10, 1000, beta100_10);

  // shuffle the samples so we get a random mix of 3 distributions
  vnl_random rand;
  for (unsigned i=0; i<samples.size(); i++) {
    int i1=rand.lrand32(0,samples.size()-1);
    int i2=rand.lrand32(0,samples.size()-1);
    float temp = samples[i1];
    samples[i1] = samples[i2];
    samples[i2] = temp;
  }

  for (unsigned i=0; i<samples.size(); i++) {
    beta_type::vector_type obs(samples[i]);
    mix_updater(model, obs ); // FIXME
  }

  std::cout << "W1=" << model.weight(0) << '\n'
           << "W2=" << model.weight(1) << '\n'
           << "W3=" << model.weight(2) << '\n' << std::endl;

  beta_type d1 = model.distribution(0);
  float diff1 = compare_betas(d1, beta100_100);
  std::cout << d1;
  TEST_NEAR("diff1", diff1, 2000, 1000);
  beta_type d2 = model.distribution(1);
  float diff2 = compare_betas(d2, beta10_100);
  std::cout << d2;
  TEST_NEAR("diff2", diff2, 1000, 1000);
  beta_type d3 = model.distribution(2);
  float diff3 = compare_betas(d3, beta100_10);
  std::cout << d3;
  TEST("diff3", vnl_math::isnan(diff3), true);
#endif
}

TESTMAIN(test_beta_updater);
