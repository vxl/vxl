#include <string>
#include <limits>
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#include <vpdl/vpdl_kernel_gaussian_sfbw.h>
#include <vpdl/vpdl_gaussian_sphere.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void test_kernel_gaussian_sfbw_type(T epsilon, const std::string& type_name)
{
  // an arbitrary collection of sample points
  std::vector<vnl_vector_fixed<T,3> > samples;
  samples.push_back(vnl_vector_fixed<T,3>(1,1,1));
  samples.push_back(vnl_vector_fixed<T,3>(2,2,2));
  samples.push_back(vnl_vector_fixed<T,3>(3,1,4));
  samples.push_back(vnl_vector_fixed<T,3>(-1,2,5));
  samples.push_back(vnl_vector_fixed<T,3>(-10,5,0));

  vnl_vector_fixed<T,3> mean(T(0));
  vnl_matrix_fixed<T,3,3> covar(T(0));
  for (unsigned int i=0; i<samples.size(); ++i){
    mean += samples[i];
    covar += outer_product(samples[i],samples[i]);
  }
  mean /= T(samples.size());
  covar /= T(samples.size());
  covar -= outer_product(mean,mean);

  T bandwidth = 1.5;
  covar(0,0) += bandwidth*bandwidth;
  covar(1,1) += bandwidth*bandwidth;
  covar(2,2) += bandwidth*bandwidth;

  // values for probability tests
  vpdl_gaussian_sphere<T,3> gauss3(vnl_vector_fixed<T,3>(T(0)), bandwidth*bandwidth);
  vnl_vector_fixed<T,3> pt1(1,2,0), pt2(10,10,10);
  T density = T(0);
  T cum_prob = T(0);
  T box_prob = T(0);
  for (unsigned int i=0; i<samples.size(); ++i){
    density += gauss3.prob_density(pt1-samples[i]);
    cum_prob += gauss3.cumulative_prob(pt1-samples[i]);
    box_prob += gauss3.box_prob(pt1-samples[i], pt2-samples[i]);
  }
  density /= samples.size();
  cum_prob /= samples.size();
  box_prob /= samples.size();

  std::cout << "=================== fixed<3> ======================="<<std::endl;
  {
    vpdl_kernel_gaussian_sfbw<T,3> kernel_g3(samples, bandwidth);

    TEST(("num_components <"+type_name+">").c_str(),
         kernel_g3.num_components(), samples.size());

    TEST(("bandwidth <"+type_name+">").c_str(),
         kernel_g3.bandwidth(), bandwidth);

    // test probability funnctions
    TEST_NEAR(("prob_density <"+type_name+">").c_str(),
              kernel_g3.prob_density(pt1), density, epsilon);
    TEST_NEAR(("cumulative_prob <"+type_name+">").c_str(),
              kernel_g3.cumulative_prob(pt1), cum_prob, epsilon);
    TEST_NEAR(("box_prob <"+type_name+">").c_str(),
              kernel_g3.box_prob(pt1,pt2), box_prob, epsilon);

    // test gradient virtual functions against numerical difference
    vnl_vector_fixed<T,3> g3;
    T dp = std::sqrt(epsilon);
    T den = kernel_g3.density(pt1);
    T den_x = kernel_g3.density(pt1+vnl_vector_fixed<T,3>(dp,0,0));
    T den_y = kernel_g3.density(pt1+vnl_vector_fixed<T,3>(0,dp,0));
    T den_z = kernel_g3.density(pt1+vnl_vector_fixed<T,3>(0,0,dp));
    vnl_vector_fixed<T,3> grad(den_x-den, den_y-den, den_z-den);
    grad /= dp;
    T density =  kernel_g3.gradient_density(pt1,g3);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              (g3-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    // test mean and covariance computation
    vnl_vector_fixed<T,3> m;
    kernel_g3.compute_mean(m);
    TEST_NEAR(("compute_mean <"+type_name+">").c_str(),
              (m-mean).inf_norm(), T(0), epsilon);
    vnl_matrix_fixed<T,3,3> c;
    kernel_g3.compute_covar(c);
    TEST_NEAR(("compute_covar <"+type_name+">").c_str(),
              (c-covar).array_inf_norm(), T(0), epsilon);

    kernel_g3.set_bandwidth(T(2.1));
    TEST(("set_bandwidth <"+type_name+">").c_str(),
         kernel_g3.bandwidth(), T(2.1));
  }

  std::cout << "=================== variable ======================="<<std::endl;
  {
    std::vector<vnl_vector<T> > vsamples;
    vsamples.push_back(samples[0].as_ref());
    vpdl_kernel_gaussian_sfbw<T> kernel_g, kernel_g1(vsamples,bandwidth);

    TEST(("init num_components <"+type_name+">").c_str(),
         kernel_g.num_components(), 0);
    TEST(("init dimension <"+type_name+">").c_str(),
         kernel_g.dimension() == 0 &&
         kernel_g1.dimension() == 3, true);

    TEST(("init bandwidth <"+type_name+">").c_str(),
         kernel_g.bandwidth(), T(1));

    kernel_g.set_bandwidth(bandwidth);
    TEST(("set_bandwidth <"+type_name+">").c_str(),
         kernel_g.bandwidth(), bandwidth);

    for (unsigned int i=0; i<samples.size(); ++i)
      kernel_g.add_sample(samples[i].as_ref());
    TEST(("add_sample <"+type_name+">").c_str(),
         kernel_g.num_components(), samples.size());

    TEST(("dimension <"+type_name+">").c_str(),
         kernel_g.dimension(), 3);

    // test probability funnctions
    TEST_NEAR(("prob_density <"+type_name+">").c_str(),
              kernel_g.prob_density(pt1.as_ref()), density, epsilon);
    TEST_NEAR(("cumulative_prob <"+type_name+">").c_str(),
              kernel_g.cumulative_prob(pt1.as_ref()), cum_prob, epsilon);
    TEST_NEAR(("box_prob <"+type_name+">").c_str(),
              kernel_g.box_prob(pt1.as_ref(),pt2.as_ref()), box_prob, epsilon);

    // test gradient virtual functions against numerical difference
    vnl_vector<T> g;
    T dp = std::sqrt(epsilon);
    T den = kernel_g.density(pt1.as_ref());
    T den_x = kernel_g.density((pt1+vnl_vector_fixed<T,3>(dp,0,0)).as_ref());
    T den_y = kernel_g.density((pt1+vnl_vector_fixed<T,3>(0,dp,0)).as_ref());
    T den_z = kernel_g.density((pt1+vnl_vector_fixed<T,3>(0,0,dp)).as_ref());
    vnl_vector_fixed<T,3> grad(den_x-den, den_y-den, den_z-den);
    grad /= dp;
    T density =  kernel_g.gradient_density(pt1.as_ref(),g);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              (g-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    // test mean and covariance computation
    vnl_vector<T> m;
    kernel_g.compute_mean(m);
    TEST_NEAR(("compute_mean <"+type_name+">").c_str(),
              (m-mean).inf_norm(), T(0), epsilon);
    vnl_matrix<T> c;
    kernel_g.compute_covar(c);
    TEST_NEAR(("compute_covar <"+type_name+">").c_str(),
              (c-covar).array_inf_norm(), T(0), epsilon);
  }

  std::cout << "=================== scalar ======================="<<std::endl;
  {
    vpdl_kernel_gaussian_sfbw<T,1> kernel_g1;
    kernel_g1.set_bandwidth(bandwidth);
    for (unsigned int i=0; i<samples.size(); ++i)
      kernel_g1.add_sample(samples[i][0]);

    TEST(("num_components <"+type_name+">").c_str(),
         kernel_g1.num_components(), samples.size());

    TEST(("bandwidth <"+type_name+">").c_str(),
         kernel_g1.bandwidth(), bandwidth);

    // values for probability tests
    vpdl_gaussian_sphere<T,1> gauss(T(0), bandwidth*bandwidth);
    T pt1 = T(1), pt2 = T(10);
    density = T(0);
    cum_prob = T(0);
    box_prob = T(0);
    for (unsigned int i=0; i<samples.size(); ++i){
      density += gauss.prob_density(pt1-samples[i][0]);
      cum_prob += gauss.cumulative_prob(pt1-samples[i][0]);
      box_prob += gauss.box_prob(pt1-samples[i][0], pt2-samples[i][0]);
    }
    density /= samples.size();
    cum_prob /= samples.size();
    box_prob /= samples.size();

    // test probability funnctions
    TEST_NEAR(("prob_density <"+type_name+">").c_str(),
              kernel_g1.prob_density(pt1), density, epsilon);
    TEST_NEAR(("cumulative_prob <"+type_name+">").c_str(),
              kernel_g1.cumulative_prob(pt1), cum_prob, epsilon);
    TEST_NEAR(("box_prob <"+type_name+">").c_str(),
              kernel_g1.box_prob(pt1,pt2), box_prob, epsilon);

    // test gradient virtual functions against numerical difference
    T g;
    T dp = std::sqrt(epsilon);
    T den = kernel_g1.density(pt1);
    T den_x = kernel_g1.density(pt1+dp);
    T grad = (den_x-den)/dp;
    T density =  kernel_g1.gradient_density(pt1,g);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              g, grad, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    // test mean and covariance computation
    T m;
    kernel_g1.compute_mean(m);
    TEST_NEAR(("compute_mean <"+type_name+">").c_str(),
              m, mean[0], epsilon);
    T c;
    kernel_g1.compute_covar(c);
    TEST_NEAR(("compute_covar <"+type_name+">").c_str(),
              c, covar(0,0), epsilon);
  }
}


static void test_kernel_gaussian_sfbw()
{
  test_kernel_gaussian_sfbw_type(1e-5f,"float");
  test_kernel_gaussian_sfbw_type(1e-14,"double");
}

TESTMAIN(test_kernel_gaussian_sfbw);
