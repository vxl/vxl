#include <testlib/testlib_test.h>

#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_beta.h>
#include <bsta/algo/bsta_beta_updater.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <vcl_string.h>
#include <vcl_fstream.h>

void load_samples(vcl_string file, vcl_vector<float>& samples)
{
  vcl_ifstream is(file.data());
  while (!is.eof()) {
    float sample;
    is >> sample;
    samples.push_back(sample);
  }
}

MAIN( test_beta_updater)
{
  START ("test_beta_model");
  vcl_cout << "----------------------------------\n"
           << " float, 1-dimensional, 3-modal\n"
           << "-----------------------------------" <<vcl_endl;

  //A tri-mixture of 1 dimensional beta distribution
  typedef bsta_num_obs<bsta_beta<float> > beta_type;
  typedef bsta_mixture_fixed<beta_type, 3> mix_beta;
  typedef bsta_num_obs<mix_beta> mix_beta_type;

  bsta_beta<float> beta(2.0, 2.0);
  bsta_num_obs<bsta_beta<float> > init_beta(beta,1);

  // single distribution update
  bsta_beta_updater<bsta_beta<float> > updater;

  vcl_ifstream is("beta_distr_2_2.txt");
  double alpha_=2, beta_=2;
  while (!is.eof()) {
    float sample;
    is >> sample;
    beta_type::vector_type obs(sample);
    updater(init_beta, obs);
  }

  vcl_cout << init_beta;
  TEST_NEAR("extracting alpha and beta from samples (2,2) - alpha", init_beta.alpha(), alpha_ ,1e-01);
  TEST_NEAR("extracting alpha and beta from samples (2,2) - beta ", init_beta.beta(), beta_ ,1e-01);

  vcl_cout << "VAR=" << init_beta.var() <<vcl_endl;

  mix_beta mix;
  bsta_beta<float> beta1;
  bsta_num_obs<mix_beta_type> model;

  bsta_mix_beta_updater<mix_beta> mix_updater(beta1,0.6f,3);
  vcl_vector<float> samples2_2, samples2_7, samples7_2;
  load_samples("beta_distr_2_2.txt", samples2_2);
  load_samples("beta_distr_2_7.txt", samples2_7);
  load_samples("beta_distr_7_2.txt", samples7_2);

  int size=samples2_2.size();

  // get random samples tt feed the mixture
  vnl_random rand;
  for (unsigned i=0; i<600; i++) {
    int index=rand.lrand32(0,size-1);
    beta_type::vector_type obs(samples2_2[index]);
    mix_updater(model, obs);
  }

  for (unsigned i=0; i<200; i++) {
    int index=rand.lrand32(0,size-1);
    beta_type::vector_type obs(samples2_2[index]);
    mix_updater(model, obs);
  }

  for (unsigned i=0; i<200; i++) {
    int index=rand.lrand32(0,size-1);
    beta_type::vector_type obs(samples7_2[index]);
    mix_updater(model, obs);
  }

  vcl_cout << "W1=" << model.weight(0) << vcl_endl;
  vcl_cout << "W2=" << model.weight(1) << vcl_endl;
  vcl_cout << "W3=" << model.weight(2) << vcl_endl << vcl_endl;

  beta_type d1 = model.distribution(0);
  vcl_cout << d1;
  beta_type d2 = model.distribution(1);
  vcl_cout << d2;
  beta_type d3 = model.distribution(2);
  vcl_cout << d3;
  
  SUMMARY();
}
