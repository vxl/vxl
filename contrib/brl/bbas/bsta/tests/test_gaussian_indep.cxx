#include <string>
#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T>
void test_gaussian_indep_type(T epsilon, const std::string& type_name)
{
  bsta_gaussian_indep<T,3> df_gauss;

  TEST(("dimension <"+type_name+">").c_str(),
       (bsta_gaussian_indep<T,3>::dimension), 3);
  TEST(("det(null covar) <"+type_name+">").c_str(),
       df_gauss.det_covar(), T(0));

  vnl_vector_fixed<T,3> mean(T(1.0), T(2.0), T(4.0));
  vnl_vector_fixed<T,3> diag_covar(T(0.5), T(1.0), T(0.5));

  //vnl_matrix_fixed<T,3,3> covar(T(0));
  //covar(0,0) = diag_covar[0];
  //covar(1,1) = diag_covar[1];
  //covar(2,2) = diag_covar[2];
  bsta_gaussian_indep<T,3> gauss(mean, diag_covar);

  TEST(("mean <"+type_name+">").c_str(), gauss.mean(), mean);
  TEST(("covar <"+type_name+">").c_str(), gauss.diag_covar(), diag_covar);
  TEST(("det(covar) <"+type_name+">").c_str(),
       gauss.det_covar(), T(0.25));

  vnl_vector_fixed<T,3> test_pt(T(1.5), T(3.0), T(3.0));
  vnl_vector_fixed<T,3> d = mean - test_pt;
  T sqr_mah_dist = d[0]*d[0]/diag_covar[0]
                 + d[1]*d[1]/diag_covar[1]
                 + d[2]*d[2]/diag_covar[2];

  TEST(("mahalanobis dist <"+type_name+">").c_str(),
       gauss.sqr_mahalanobis_dist(test_pt), sqr_mah_dist);

  T two_pi = static_cast<T>(vnl_math::twopi);
  T prob = static_cast<T>(1.0/std::sqrt(two_pi*two_pi*two_pi*gauss.det_covar()) * std::exp(-sqr_mah_dist/2));
  TEST_NEAR(("probability density <"+type_name+">").c_str(),
            gauss.prob_density(test_pt), prob, epsilon);

  T gradient_x = static_cast<T>(-prob);
  TEST_NEAR(("gradient x <"+type_name+">").c_str(),
            gauss.gradient(test_pt)[0], gradient_x, epsilon);
  T gradient_y = static_cast<T>(-prob);
  TEST_NEAR(("gradient y <"+type_name+">").c_str(),
            gauss.gradient(test_pt)[1], gradient_y, epsilon);
  T gradient_z = static_cast<T>(2.0*prob);
  TEST_NEAR(("gradient z <"+type_name+">").c_str(),
            gauss.gradient(test_pt)[2], gradient_z, epsilon);

  bsta_gaussian_indep<T,3> zero_var_gauss;
  TEST(("zero var mahalanobis dist <"+type_name+">").c_str(),
       zero_var_gauss.sqr_mahalanobis_dist(test_pt),
       std::numeric_limits<T>::infinity());

  TEST(("zero var probability density <"+type_name+">").c_str(),
       zero_var_gauss.prob_density(test_pt), T(0));

  vnl_vector_fixed<T,3> delta(T(0.1), T(0.1), T(0.1));
  T prob_box = gauss.probability(mean-delta, mean+delta);
  TEST_NEAR(("box probability density <"+type_name+">").c_str(),prob_box,0.00100748,1e-07);

  // test stream
  std::cout << "testing stream operator\n"
           << gauss << '\n';
}


static void test_gaussian_indep()
{
  test_gaussian_indep_type(1e-5f,"float");
  test_gaussian_indep_type(1e-14,"double");
}

TESTMAIN(test_gaussian_indep);
