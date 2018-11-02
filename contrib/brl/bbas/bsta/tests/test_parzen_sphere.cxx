#include <string>
#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bsta/bsta_parzen_sphere.h>
#include <vnl/vnl_matrix_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//
template <class T>
void test_parzen_sphere_type(T epsilon, const std::string& type_name)
{
  bool good = bsta_parzen_sphere<T,3>::dimension == 3
           && bsta_parzen_sphere<T,1>::dimension == 1;
  TEST(("dimension <"+type_name+">").c_str(),good, true);

  vnl_vector_fixed<T,3> s0(T(1.0), T(2.0), T(3.0));
  vnl_vector_fixed<T,3> s1(T(2.0), T(3.0), T(4.0));
  std::vector<vnl_vector_fixed<T,3> > samples;
  samples.push_back(s0); samples.push_back(s1);
  T bandwidth = T(0.5);
  bsta_parzen_sphere<T,3> parzen;
  parzen.set_bandwidth(bandwidth);
  parzen.insert_samples(samples);
  TEST(("n_samples <"+type_name+">").c_str(), parzen.size(), 2);

  vnl_vector_fixed<T,3> mean(T(1.5), T(2.5), T(3.5));
  TEST(("mean <"+type_name+">").c_str(), parzen.mean(), mean);

  //test covariance
  vnl_matrix_fixed<T, 3, 3> covar = parzen.covar();
  vnl_matrix_fixed<T, 3, 3> cact(T(0.25));
  cact[0][0]=T(0.5); cact[1][1]=T(0.5); cact[2][2]=T(0.5);
  T er = (covar - cact).frobenius_norm();
  TEST_NEAR(("covariance matrix <"+type_name+">").c_str(),
            er, T(0), epsilon);

  T prob_den = T(0.11333876);
  TEST_NEAR(("probability density <"+type_name+">").c_str(),
            parzen.prob_density(mean), prob_den, epsilon);

  T prob = T(0.10870197904);
  TEST_NEAR(("probability  <"+type_name+">").c_str(),
            parzen.probability(s0, s1), prob, epsilon);

  std::vector<T> ssamps;
  ssamps.push_back(1.0);   ssamps.push_back(2.0);   ssamps.push_back(3.0);
  bsta_parzen_sphere<T,1> sparzen;
  sparzen.set_bandwidth(bandwidth);
  sparzen.insert_samples(ssamps);
  TEST(("n_samples_scalar <"+type_name+">").c_str(), sparzen.size(), 3);
  T smean = 2.0;
  TEST(("mean_scalar <"+type_name+">").c_str(), sparzen.mean(), smean);

  T sprob_den = T(0.3379494756185392);
  TEST_NEAR(("sprob_den <"+type_name+">").c_str(), sparzen.prob_density(smean),
            sprob_den, epsilon);

  T sprobability = T(0.33243340131224675);
  TEST_NEAR(("sprobability <"+type_name+">").c_str(),
            sparzen.probability(smean-bandwidth, smean +bandwidth),
            sprobability, epsilon);
  T var = sparzen.covar();
  TEST_NEAR(("variance <"+type_name+">").c_str(),
            var, T(0.9166666666666607), epsilon);
}


static void test_parzen_sphere()
{
  test_parzen_sphere_type(1e-5f,"float");
  //some values not specified to higher double accuracy
  test_parzen_sphere_type(1e-8,"double");
}

TESTMAIN(test_parzen_sphere);
