#include <testlib/testlib_test.h>
#include <bsta/bsta_parzen_sphere.h>
#include <vcl_string.h>
#include <vcl_limits.h>
#include <vcl_iostream.h>
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//
template <class T>
void test_parzen_sphere_type(T epsilon, const vcl_string& type_name)
{
  bsta_parzen_sphere<T,3> df_parzen;

  TEST(("dimension <"+type_name+">").c_str(),
        (bsta_parzen_sphere<T,3>::dimension), 3);

  vnl_vector_fixed<T,3> s0(T(1.0), T(2.0), T(3.0));
  vnl_vector_fixed<T,3> s1(T(2.0), T(3.0), T(4.0));
  vcl_vector<vnl_vector_fixed<T,3> > samples;
  samples.push_back(s0); samples.push_back(s1);
  T bandwidth = T(0.5);
  bsta_parzen_sphere<T,3> parzen;
  parzen.set_bandwidth(bandwidth);
  parzen.insert_samples(samples);
  TEST(("n_samples <"+type_name+">").c_str(),
	  parzen.size(), 2);

  vnl_vector_fixed<T,3> mean(T(1.5), T(2.5), T(3.5));
  TEST(("mean <"+type_name+">").c_str(), parzen.mean(), mean);

  T prob_den = T(0.11333876);
  TEST_NEAR(("probability density <"+type_name+">").c_str(),
             parzen.prob_density(mean), prob_den, epsilon);

  T prob = T(0.10870197904);
  TEST_NEAR(("probability  <"+type_name+">").c_str(),
             parzen.probability(s0, s1), prob, epsilon);
}


MAIN( test_parzen_sphere )
{
  START ("parzen_sphere");
  test_parzen_sphere_type(float(1e-5),"float");
  //  test_parzen_sphere_type(double(1e-14),"double");
  SUMMARY();
}


