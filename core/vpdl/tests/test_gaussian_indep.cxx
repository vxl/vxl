#include <string>
#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpdl/vpdl_gaussian_indep.h>
#include <vnl/vnl_math.h> // for twopi
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T>
void test_gaussian_indep_type(T epsilon, const std::string& type_name)
{
  // test dimension, zero variance
  {
    vpdl_gaussian_indep<T,3> gauss3;
    vpdl_gaussian_indep<T,1> gauss1;
    vpdl_gaussian_indep<T> gauss_default, gauss(3),
                           gauss_init(vnl_vector<T>(10,T(1)),
                                      vnl_vector<T>(10,T(3)));

    TEST(("dimension <"+type_name+"> fixed").c_str(),
         gauss3.dimension(), 3);
    TEST(("dimension <"+type_name+"> scalar").c_str(),
         gauss1.dimension(), 1);
    TEST(("dimension <"+type_name+"> variable").c_str(),
         gauss_default.dimension(), 0);
    TEST(("dimension <"+type_name+"> variable").c_str(),
         gauss.dimension(), 3);
    TEST(("dimension <"+type_name+"> variable").c_str(),
         gauss_init.dimension(), 10);

    TEST(("mean size <"+type_name+"> variable").c_str(),
         gauss_default.mean().size(), 0);
    TEST(("mean size <"+type_name+"> variable").c_str(),
         gauss.mean().size(), 3);
    TEST(("mean size <"+type_name+"> variable").c_str(),
         gauss_init.mean().size(), 10);

    TEST(("var size <"+type_name+"> variable").c_str(),
         gauss_default.covariance().size(), 0);
    TEST(("var size <"+type_name+"> variable").c_str(),
         gauss.covariance().size(), 3);
    TEST(("var size <"+type_name+"> variable").c_str(),
         gauss_init.covariance().size(), 10);

    // test initialization to zero mean, zero variance
    vnl_vector_fixed<T,3> zero_vector(T(0));
    TEST(("zero default mean <"+type_name+"> fixed").c_str(),
         gauss3.mean(), zero_vector);
    TEST(("zero default mean <"+type_name+"> scalar").c_str(),
         gauss1.mean(), T(0));
    TEST(("zero default mean <"+type_name+"> variable").c_str(),
         gauss.mean(), zero_vector);

    TEST(("zero default variance <"+type_name+"> fixed").c_str(),
         gauss3.covariance(), zero_vector);
    TEST(("zero default variance <"+type_name+"> scalar").c_str(),
         gauss1.covariance(), T(0));
    TEST(("zero default variance <"+type_name+"> variable").c_str(),
         gauss.covariance(), zero_vector.as_ref());

    // test zero variance evaluations
    vnl_vector_fixed<T,3> test_pt(T(1), T(1), T(1));
    TEST(("zero var mahalanobis dist <"+type_name+"> fixed").c_str(),
         gauss3.sqr_mahal_dist(test_pt),
         std::numeric_limits<T>::infinity());
    TEST(("zero var mahalanobis dist <"+type_name+"> scalar").c_str(),
         gauss1.sqr_mahal_dist(test_pt[0]),
         std::numeric_limits<T>::infinity());
    TEST(("zero var mahalanobis dist <"+type_name+"> variable").c_str(),
         gauss.sqr_mahal_dist(test_pt.as_ref()),
         std::numeric_limits<T>::infinity());

    // test zero variance probability
    TEST(("zero var probability density<"+type_name+"> fixed").c_str(),
         gauss3.prob_density(test_pt), T(0));
    TEST(("zero var probability density<"+type_name+"> scalar").c_str(),
         gauss1.prob_density(test_pt[0]), T(0));
    TEST(("zero var probability density<"+type_name+"> variable").c_str(),
         gauss.prob_density(test_pt.as_ref()), T(0));

    // test zero variance log probability
    TEST(("zero var log probability density<"+type_name+"> fixed").c_str(),
         gauss3.log_prob_density(test_pt),
         -std::numeric_limits<T>::infinity());
    TEST(("zero var log probability density<"+type_name+"> scalar").c_str(),
         gauss1.log_prob_density(test_pt[0]),
         -std::numeric_limits<T>::infinity());
    TEST(("zero var log probability density<"+type_name+"> variable").c_str(),
         gauss.log_prob_density(test_pt.as_ref()),
         -std::numeric_limits<T>::infinity());
  }

  // test mean, covariance, square malanobis distance, probability density,
  // cumulative probability, box probability
  {
    vnl_vector_fixed<T,3> mean(T(1.0), T(2.0), T(4.0));
    vnl_vector_fixed<T,3> var(T(2.0), T(0.5), T(1.5));
    vpdl_gaussian_indep<T,3> gauss3(mean,var);
    vpdl_gaussian_indep<T,1> gauss1(mean[0],var[0]);
    vpdl_gaussian_indep<T> gauss(mean.as_ref(),var.as_ref());

    // test direct access to data member
    TEST(("mean <"+type_name+"> fixed").c_str(), gauss3.mean(), mean);
    TEST(("covar <"+type_name+"> fixed").c_str(), gauss3.covariance(), var);
    TEST(("mean <"+type_name+"> scalar").c_str(), gauss1.mean(), mean[0]);
    TEST(("covar <"+type_name+"> scalar").c_str(), gauss1.covariance(), var[0]);
    TEST(("mean <"+type_name+"> variable").c_str(), gauss.mean(), mean.as_ref());
    TEST(("covar <"+type_name+"> variable").c_str(), gauss.covariance(), var.as_ref());

    // test virtual functions
    const vpdl_distribution<T,3>& dist3 = gauss3;
    const vpdl_distribution<T,1>& dist1 = gauss1;
    const vpdl_distribution<T>& dist = gauss;

    vnl_matrix_fixed<T,3,3> Strue(T(0));
    for (unsigned int i=0; i<3; ++i)
      Strue(i,i) = var[i];

    // test indirect access to data members (compute full covariance)
    vnl_vector_fixed<T,3> m3;
    dist3.compute_mean(m3);
    TEST(("compute_mean <"+type_name+"> fixed").c_str(), m3, mean);

    vnl_matrix_fixed<T,3,3> S3;
    dist3.compute_covar(S3);
    TEST(("compute_covar <"+type_name+"> fixed").c_str(), S3, Strue);

    T m1;
    dist1.compute_mean(m1);
    TEST(("compute_mean <"+type_name+"> scalar").c_str(), m1, mean[0]);

    T v1;
    dist1.compute_covar(v1);
    TEST(("compute_covar <"+type_name+"> scalar").c_str(), v1, var[0]);

    vnl_vector<T> m;
    dist.compute_mean(m);
    TEST(("compute_mean <"+type_name+"> variable").c_str(), m, mean.as_ref());

    vnl_matrix<T> S;
    dist.compute_covar(S);
    TEST(("compute_covar <"+type_name+"> variable").c_str(), S, Strue.as_ref());

    vnl_vector_fixed<T,3> test_pt(T(1.5), T(3.0), T(3.0));
    vnl_vector_fixed<T,3> d = mean - test_pt;
    T sqr_mahal_dist = d[0]*d[0]/var[0]
                     + d[1]*d[1]/var[1]
                     + d[2]*d[2]/var[2];

    // test mahalanobis distance calculations
    TEST(("mahalanobis dist <"+type_name+"> fixed").c_str(),
         gauss3.sqr_mahal_dist(test_pt), sqr_mahal_dist);
    TEST(("mahalanobis dist <"+type_name+"> scalar").c_str(),
         gauss1.sqr_mahal_dist(test_pt[0]), d[0]*d[0]/var[0]);
    TEST(("mahalanobis dist <"+type_name+"> variable").c_str(),
         gauss.sqr_mahal_dist(test_pt.as_ref()), sqr_mahal_dist);

    T two_pi = static_cast<T>(vnl_math::twopi);
    T prob3 = static_cast<T>(1.0/std::sqrt(two_pi*two_pi*two_pi
                                          *var[0]*var[1]*var[2])
                             * std::exp(-sqr_mahal_dist/2) );
    T prob1 = static_cast<T>(1.0/std::sqrt(two_pi*var[0])
                             * std::exp(-d[0]*d[0]/(2*var[0])) );

    // test probability density virtual functions
    TEST_NEAR(("probability density <"+type_name+"> fixed").c_str(),
              dist3.prob_density(test_pt), prob3, epsilon);
    TEST_NEAR(("probability density <"+type_name+"> scalar").c_str(),
              dist1.prob_density(test_pt[0]), prob1, epsilon);
    TEST_NEAR(("probability density <"+type_name+"> variable").c_str(),
              dist.prob_density(test_pt.as_ref()), prob3, epsilon);

    // test log probability density virtual functions
    TEST_NEAR(("probability density <"+type_name+"> fixed").c_str(),
              dist3.log_prob_density(test_pt), std::log(prob3), epsilon);
    TEST_NEAR(("probability density <"+type_name+"> scalar").c_str(),
              dist1.log_prob_density(test_pt[0]), std::log(prob1), epsilon);
    TEST_NEAR(("probability density <"+type_name+"> variable").c_str(),
              dist.log_prob_density(test_pt.as_ref()), std::log(prob3), epsilon);

    // test gradient virtual functions against numerical difference
    vnl_vector_fixed<T,3> g3;
    T dp = std::sqrt(epsilon);
    T den = dist3.density(test_pt);
    T den_x = dist3.density(test_pt+vnl_vector_fixed<T,3>(dp,0,0));
    T den_y = dist3.density(test_pt+vnl_vector_fixed<T,3>(0,dp,0));
    T den_z = dist3.density(test_pt+vnl_vector_fixed<T,3>(0,0,dp));
    vnl_vector_fixed<T,3> grad(den_x-den, den_y-den, den_z-den);
    grad /= dp;
    T density = dist3.gradient_density(test_pt,g3);
    TEST_NEAR(("gradient density <"+type_name+"> fixed").c_str(),
              (g3-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+"> fixed").c_str(),
              density, den, epsilon);
    density = dist1.gradient_density(test_pt[0],g3[0]);
    T den1 = dist1.density(test_pt[0]);
    T den1_x = dist1.density(test_pt[0]+dp);
    TEST_NEAR(("gradient density <"+type_name+"> scalar").c_str(),
              g3[0], (den1_x-den1)/dp, dp);
    TEST_NEAR(("density <"+type_name+"> scalar").c_str(),
              density, den1, epsilon);
    vnl_vector<T> g;
    density = dist.gradient_density(test_pt.as_ref(),g);
    TEST_NEAR(("gradient density <"+type_name+"> variable").c_str(),
              (g-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+"> variable").c_str(),
              density, den, epsilon);

    // test cumulative probability
    vnl_vector_fixed<T,3> test1(T(3), T(3), T(3));
    vnl_vector_fixed<T,3> cum_test1;
    cum_test1[0] = T((1+vnl_erf((test1[0]-mean[0])/std::sqrt(2*var[0])))/2);
    cum_test1[1] = T((1+vnl_erf((test1[1]-mean[1])/std::sqrt(2*var[1])))/2);
    cum_test1[2] = T((1+vnl_erf((test1[2]-mean[2])/std::sqrt(2*var[2])))/2);
    T joint_cum_test1 = cum_test1[0] * cum_test1[1] * cum_test1[2];
    TEST(("cumulative probability 1 <"+type_name+"> fixed").c_str(),
         gauss3.cumulative_prob(mean), T(0.125));
    TEST_NEAR(("cumulative probability 2 <"+type_name+"> fixed").c_str(),
              gauss3.cumulative_prob(test1), joint_cum_test1, epsilon);
    TEST(("cumulative probability 1 <"+type_name+"> scalar").c_str(),
         gauss1.cumulative_prob(mean[0]), T(0.5));
    TEST_NEAR(("cumulative probability 2 <"+type_name+"> scalar").c_str(),
              gauss1.cumulative_prob(test1[0]), cum_test1[0], epsilon);
    TEST(("cumulative probability 1 <"+type_name+"> variable").c_str(),
         gauss.cumulative_prob(mean.as_ref()), T(0.125));
    TEST_NEAR(("cumulative probability 2 <"+type_name+"> variable").c_str(),
              gauss.cumulative_prob(test1.as_ref()), joint_cum_test1, epsilon);

    // test box probability
    vnl_vector_fixed<T,3> test2(T(-1), T(1), T(0));
    vnl_vector_fixed<T,3> cum_test2;
    cum_test2[0] = T((1+vnl_erf((test2[0]-mean[0])/std::sqrt(2*var[0])))/2);
    cum_test2[1] = T((1+vnl_erf((test2[1]-mean[1])/std::sqrt(2*var[1])))/2);
    cum_test2[2] = T((1+vnl_erf((test2[2]-mean[2])/std::sqrt(2*var[2])))/2);
    T box_test = (cum_test1[0]-cum_test2[0])
               * (cum_test1[1]-cum_test2[1])
               * (cum_test1[2]-cum_test2[2]);
    TEST_NEAR(("box probability <"+type_name+"> fixed").c_str(),
              gauss3.box_prob(test2,test1), box_test, epsilon);
    TEST_NEAR(("box probability <"+type_name+"> scalar").c_str(),
              gauss1.box_prob(test2[0],test1[0]),
              (cum_test1[0]-cum_test2[0]), epsilon);
    TEST_NEAR(("box probability <"+type_name+"> variable").c_str(),
              gauss.box_prob(test2.as_ref(),test1.as_ref()), box_test, epsilon);

    vpdl_distribution<T>* base = &gauss; // pointer to the base class
    TEST_NEAR(("box probability (base==derived) <"+type_name+">").c_str(),
              base->box_prob(test2.as_ref(),test1.as_ref()),
              gauss.box_prob(test2.as_ref(),test1.as_ref()), epsilon);
  }
}


static void test_gaussian_indep()
{
  test_gaussian_indep_type(1e-5f,"float");
  test_gaussian_indep_type(1e-14,"double");
}

TESTMAIN(test_gaussian_indep);
