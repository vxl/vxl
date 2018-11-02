#include <string>
#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpdl/vpdl_mixture_of.h>
#include <vpdl/vpdl_gaussian.h>
#include <vpdl/vpdl_gaussian_sphere.h>
#include <vpdl/vpdl_gaussian_indep.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// function to sort by increasing weight
template <class dist_t>
bool weight_less(const dist_t& /*d1*/, const typename vpdt_dist_traits<dist_t>::scalar_type& w1,
                 const dist_t& /*d2*/, const typename vpdt_dist_traits<dist_t>::scalar_type& w2)
{
  return w1 < w2;
}


template <class T>
void test_mixture_of_type(T epsilon, const std::string& type_name)
{
  // test a bunch of instantiations to make sure they compile
  {
    vpdl_mixture_of<vpdl_gaussian_sphere<T,3> > mix_gauss_s3;
    vpdl_mixture_of<vpdl_gaussian_indep<T,3> > mix_gauss_i3;
    vpdl_mixture_of<vpdl_gaussian<T,3> > mix_gauss_f3;
    vpdl_mixture_of<vpdl_gaussian_sphere<T,1> > mix_gauss_s1;
    vpdl_mixture_of<vpdl_gaussian_indep<T,1> > mix_gauss_i1;
    vpdl_mixture_of<vpdl_gaussian<T,1> > mix_gauss_f1;
    vpdl_mixture_of<vpdl_gaussian_sphere<T> > mix_gauss_s;
    vpdl_mixture_of<vpdl_gaussian_indep<T> > mix_gauss_i;
    vpdl_mixture_of<vpdl_gaussian<T> > mix_gauss_f;
  }

  // create a few sample distributions
  vnl_vector_fixed<T,3> mean1(T(1), T(1), T(1));
  vnl_vector_fixed<T,3> var1(T(0.25), T(1), T(1));

  vnl_vector_fixed<T,3> mean2(T(2), T(0), T(-3));
  vnl_vector_fixed<T,3> var2(T(1), T(0.25), T(2));

  vnl_vector_fixed<T,3> mean3(T(1), T(0), T(0));
  vnl_vector_fixed<T,3> var3(T(0.5), T(1.5), T(0.25));

  std::cout << "=================== fixed<3> ======================="<<std::endl;
  {
    vpdl_gaussian_indep<T,3> gauss1(mean1,var1);
    vpdl_gaussian_indep<T,3> gauss2(mean2,var2);
    vpdl_gaussian_indep<T,3> gauss3(mean3,var3);

    typedef vpdl_gaussian_indep<T,3> dist_t;
    vpdl_mixture_of<dist_t> mixture;

    TEST(("initial num_components <"+type_name+">").c_str(),
         mixture.num_components(), 0);

    mixture.insert(gauss1, T(0.2));
    bool valid = mixture.num_components() == 1;
    mixture.insert(gauss2, T(0.5));
    valid = valid && mixture.num_components() == 2;
    TEST(("num_components after insert <"+type_name+">").c_str(),
         valid, true);

    const dist_t& dist1 = mixture.distribution(0);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1.mean() == gauss1.mean() &&
         dist1.covariance() == gauss1.covariance(), true);
    TEST(("weight access <"+type_name+">").c_str(),
         mixture.weight(1), T(0.5));

    valid = mixture.remove_last();
    TEST(("remove last <"+type_name+">").c_str(),
         valid && mixture.num_components() == 1, true);

    mixture.insert(gauss2, T(0.3));
    mixture.insert(gauss3, T(0.15));
    mixture.set_weight(0, T(0.05));
    TEST(("set_weight <"+type_name+">").c_str(),
         mixture.weight(0), T(0.05));

    mixture.sort();
    TEST(("default sort <"+type_name+">").c_str(),
         mixture.weight(0) == T(0.3) && mixture.weight(1) == T(0.15) &&
         mixture.weight(2) == T(0.05) && &mixture.distribution(2) == &dist1,
         true);

    vpdl_mixture_of<dist_t> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15),
         true);

    vnl_vector_fixed<T,3> pt(T(0), T(1.5), T(1));
    T prob = T( 0.1*gauss1.prob_density(pt)
              + 0.6*gauss2.prob_density(pt)
              + 0.3*gauss3.prob_density(pt));
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
              mixture2.prob_density(pt), prob, epsilon);

    // test gradient virtual functions against numerical difference
    vnl_vector_fixed<T,3> g3;
    T dp = std::sqrt(epsilon);
    T den = mixture2.density(pt);
    T den_x = mixture2.density(pt+vnl_vector_fixed<T,3>(dp,0,0));
    T den_y = mixture2.density(pt+vnl_vector_fixed<T,3>(0,dp,0));
    T den_z = mixture2.density(pt+vnl_vector_fixed<T,3>(0,0,dp));
    vnl_vector_fixed<T,3> grad(den_x-den, den_y-den, den_z-den);
    grad /= dp;
    T density =  mixture2.gradient_density(pt,g3);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              (g3-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    prob = T( 0.1*gauss1.cumulative_prob(pt)
            + 0.6*gauss2.cumulative_prob(pt)
            + 0.3*gauss3.cumulative_prob(pt));
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt), prob, epsilon);

    vnl_vector_fixed<T,3> pt2(T(10), T(5), T(8));
    prob = T( 0.1*gauss1.box_prob(pt,pt2)
            + 0.6*gauss2.box_prob(pt,pt2)
            + 0.3*gauss3.box_prob(pt,pt2));
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt,pt2), prob, epsilon);

    mixture2.sort(weight_less<dist_t>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) &&
         mixture2.weight(2) == T(0.3), true);

    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) &&
         mixture2.weight(2) == T(0.6), true);
  }

  std::cout << "=================== scalar ======================="<<std::endl;
  {
    vpdl_gaussian<T,1> gauss1(mean1[0],var1[0]);
    vpdl_gaussian<T,1> gauss2(mean2[0],var2[0]);
    vpdl_gaussian<T,1> gauss3(mean3[0],var3[0]);

    typedef vpdl_gaussian<T,1> dist_t;
    vpdl_mixture_of<dist_t> mixture;

    TEST(("initial num_components <"+type_name+">").c_str(),
         mixture.num_components(), 0);

    mixture.insert(gauss1, T(0.2));
    bool valid = mixture.num_components() == 1;
    mixture.insert(gauss2, T(0.5));
    valid = valid && mixture.num_components() == 2;
    TEST(("num_components after insert <"+type_name+">").c_str(),
         valid, true);

    const dist_t& dist1 = mixture.distribution(0);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1.mean() == gauss1.mean() &&
         dist1.covariance() == gauss1.covariance(), true);
    TEST(("weight access <"+type_name+">").c_str(),
         mixture.weight(1), T(0.5));

    valid = mixture.remove_last();
    TEST(("remove last <"+type_name+">").c_str(),
         valid && mixture.num_components() == 1, true);

    mixture.insert(gauss2, T(0.3));
    mixture.insert(gauss3, T(0.15));
    mixture.set_weight(0, T(0.05));
    TEST(("set_weight <"+type_name+">").c_str(),
         mixture.weight(0), T(0.05));

    mixture.sort();
    TEST(("default sort <"+type_name+">").c_str(),
         mixture.weight(0) == T(0.3) && mixture.weight(1) == T(0.15) &&
         mixture.weight(2) == T(0.05) && &mixture.distribution(2) == &dist1,
         true);

    vpdl_mixture_of<dist_t> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15),
         true);

    T pt = T(1);
    T prob = T( 0.1*gauss1.prob_density(pt)
              + 0.6*gauss2.prob_density(pt)
              + 0.3*gauss3.prob_density(pt));
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
              mixture2.prob_density(pt), prob, epsilon);

    // test gradient virtual functions against numerical difference
    T g;
    T dp = std::sqrt(epsilon);
    T den = mixture2.density(pt);
    T den_x = mixture2.density(pt+dp);
    T grad = (den_x-den)/dp;
    T density =  mixture2.gradient_density(pt,g);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              g, grad, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    prob = T( 0.1*gauss1.cumulative_prob(pt)
            + 0.6*gauss2.cumulative_prob(pt)
            + 0.3*gauss3.cumulative_prob(pt));
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt), prob, epsilon);

    T pt2 = T(10);
    prob = T( 0.1*gauss1.box_prob(pt,pt2)
            + 0.6*gauss2.box_prob(pt,pt2)
            + 0.3*gauss3.box_prob(pt,pt2));
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt,pt2), prob, epsilon);

    mixture2.sort(weight_less<dist_t>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) &&
         mixture2.weight(2) == T(0.3), true);

    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) &&
         mixture2.weight(2) == T(0.6), true);

    T mean = T(0.1*gauss1.mean() + 0.6*gauss2.mean() + 0.3*gauss3.mean());
    T cmp_mean;
    mixture2.compute_mean(cmp_mean);
    TEST_NEAR(("compute_mean <"+type_name+">").c_str(),
              cmp_mean, mean, epsilon);

    T var = T(0.1*gauss1.covariance() + 0.6*gauss2.covariance() + 0.3*gauss3.covariance());
    var += T( 0.1*gauss1.mean()*gauss1.mean()
            + 0.6*gauss2.mean()*gauss2.mean()
            + 0.3*gauss3.mean()*gauss3.mean());
    var -= mean*mean;
    T cmp_var;
    mixture2.compute_covar(cmp_var);
    TEST_NEAR(("compute_covar <"+type_name+">").c_str(),
              cmp_var, var, epsilon);
  }

  std::cout << "=================== variable ======================="<<std::endl;
  {
    vpdl_gaussian_indep<T> gauss1(mean1.as_ref(),var1.as_ref());
    vpdl_gaussian_indep<T> gauss2(mean2.as_ref(),var2.as_ref());
    vpdl_gaussian_indep<T> gauss3(mean3.as_ref(),var3.as_ref());

    typedef vpdl_gaussian_indep<T> dist_t;
    vpdl_mixture_of<dist_t> mixture;

    TEST(("initial num_components <"+type_name+">").c_str(),
         mixture.num_components(), 0);
    TEST(("initial dimension <"+type_name+">").c_str(),
         mixture.dimension(), 0);

    mixture.insert(gauss1, T(0.2));
    bool valid = mixture.num_components() == 1;
    mixture.insert(gauss2, T(0.5));
    valid = valid && mixture.num_components() == 2;
    TEST(("num_components after insert <"+type_name+">").c_str(),
         valid, true);
    TEST(("dimension after insert <"+type_name+">").c_str(),
         mixture.dimension(), 3);

    const dist_t& dist1 = mixture.distribution(0);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1.mean() == gauss1.mean() &&
         dist1.covariance() == gauss1.covariance(), true);
    TEST(("weight access <"+type_name+">").c_str(),
         mixture.weight(1), T(0.5));

    valid = mixture.remove_last();
    TEST(("remove last <"+type_name+">").c_str(),
         valid && mixture.num_components() == 1, true);

    mixture.insert(gauss2, T(0.3));
    mixture.insert(gauss3, T(0.15));
    mixture.set_weight(0, T(0.05));
    TEST(("set_weight <"+type_name+">").c_str(),
         mixture.weight(0), T(0.05));

    mixture.sort();
    TEST(("default sort <"+type_name+">").c_str(),
         mixture.weight(0) == T(0.3) && mixture.weight(1) == T(0.15) &&
         mixture.weight(2) == T(0.05) && &mixture.distribution(2) == &dist1,
         true);

    vpdl_mixture_of<dist_t> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15) &&
         mixture2.dimension() == 3, true);

    vnl_vector_fixed<T,3> pt(T(0), T(1.5), T(1));
    T prob = T( 0.1*gauss1.prob_density(pt.as_ref())
              + 0.6*gauss2.prob_density(pt.as_ref())
              + 0.3*gauss3.prob_density(pt.as_ref()));
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
              mixture2.prob_density(pt.as_ref()), prob, epsilon);

    // test gradient virtual functions against numerical difference
    vnl_vector<T> g;
    T dp = std::sqrt(epsilon);
    T den = mixture2.density(pt.as_ref());
    T den_x = mixture2.density((pt+vnl_vector_fixed<T,3>(dp,0,0)).as_ref());
    T den_y = mixture2.density((pt+vnl_vector_fixed<T,3>(0,dp,0)).as_ref());
    T den_z = mixture2.density((pt+vnl_vector_fixed<T,3>(0,0,dp)).as_ref());
    vnl_vector_fixed<T,3> grad(den_x-den, den_y-den, den_z-den);
    grad /= dp;
    T density =  mixture2.gradient_density(pt.as_ref(),g);
    TEST_NEAR(("gradient density <"+type_name+">").c_str(),
              (g-grad).inf_norm(), 0, dp);
    TEST_NEAR(("density <"+type_name+">").c_str(),
              density, den, epsilon);

    prob = T( 0.1*gauss1.cumulative_prob(pt.as_ref())
            + 0.6*gauss2.cumulative_prob(pt.as_ref())
            + 0.3*gauss3.cumulative_prob(pt.as_ref()));
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt.as_ref()), prob, epsilon);

    vnl_vector_fixed<T,3> pt2(T(10), T(5), T(8));
    prob = T( 0.1*gauss1.box_prob(pt.as_ref(),pt2.as_ref())
            + 0.6*gauss2.box_prob(pt.as_ref(),pt2.as_ref())
            + 0.3*gauss3.box_prob(pt.as_ref(),pt2.as_ref()));
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt.as_ref(),pt2.as_ref()), prob, epsilon);

    mixture2.sort(weight_less<dist_t>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) &&
         mixture2.weight(2) == T(0.3), true);

    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) &&
         mixture2.weight(2) == T(0.6), true);
  }
}


static void test_mixture_of()
{
  test_mixture_of_type(1e-5f,"float");
  test_mixture_of_type(1e-13,"double");
}

TESTMAIN(test_mixture_of);
