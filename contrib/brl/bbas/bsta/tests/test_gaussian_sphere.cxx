#include <testlib/testlib_test.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>


template <class T>
void test_gaussian_sphere_type(T epsilon, const vcl_string& type_name)
{
  bsta_gaussian_sphere<T,3> df_gauss;

  TEST(("dimension <"+type_name+">").c_str(),
        (bsta_gaussian_sphere<T,3>::dimension), 3);
  TEST(("det(null covar) <"+type_name+">").c_str(),
        df_gauss.det_covar(), T(0));

  vnl_vector_fixed<T,3> mean(T(1.0), T(2.0), T(4.0));
  T var = T(0.5);

  bsta_gaussian_sphere<T,3> gauss(mean, var);

  TEST(("mean <"+type_name+">").c_str(), gauss.mean(), mean);
  TEST(("var <"+type_name+">").c_str(), gauss.var(), var);
  TEST(("det(covar) <"+type_name+">").c_str(),
        gauss.det_covar(), T(0.125));

  vnl_vector_fixed<T,3> test_pt(T(1.5), T(3.0), T(3.0));
  vnl_vector_fixed<T,3> d = mean - test_pt;
  T sqr_mah_dist = d[0]*d[0]/var
      + d[1]*d[1]/var
      + d[2]*d[2]/var;

  TEST(("mahalanobis dist <"+type_name+">").c_str(),
        gauss.sqr_mahalanobis_dist(test_pt), sqr_mah_dist);

  T two_pi = static_cast<T>(2.0*vnl_math::pi);
  T prob = static_cast<T>(1.0/vcl_sqrt(two_pi*two_pi*two_pi*gauss.det_covar()) * vcl_exp(-sqr_mah_dist/2));

  TEST_NEAR(("probability density <"+type_name+">").c_str(),
             gauss.prob_density(test_pt), prob, epsilon);

  bsta_gaussian_sphere<T,3> zero_var_gauss;
  TEST(("zero var mahalanobis dist <"+type_name+">").c_str(),
        zero_var_gauss.sqr_mahalanobis_dist(test_pt),
        vcl_numeric_limits<T>::infinity());

  TEST(("zero var probability density<"+type_name+">").c_str(),
        zero_var_gauss.prob_density(test_pt), T(0));

  vnl_vector_fixed<T,3> delta(T(0.1), T(0.1), T(0.1));
  T prob_box = gauss.probability(mean-delta, mean+delta);
  TEST_NEAR(("box probability <"+type_name+">").c_str(),
            prob_box,0.00142242,1e-07);
  // test the 1-d sphere case
  typedef T vector_;
  vector_ mean_1d = vector_(1.0);
  bsta_gaussian_sphere<T,1> gauss_1d(mean_1d, var);
  vector_ box_1d_low = vector_(0.9), box_1d_high = vector_(1.1);
  T prob_box_1d = gauss_1d.probability(box_1d_low, box_1d_high);
  TEST_NEAR(("box probability (1-d) <"+type_name+">").c_str(),
            prob_box_1d,0.11246291,1e-07);
}



MAIN( test_gaussian_sphere )
{
  START ("gaussian_sphere");
  test_gaussian_sphere_type(float(1e-5),"float");
  test_gaussian_sphere_type(double(1e-14),"double");
  SUMMARY();
}


