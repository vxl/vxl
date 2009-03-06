#include <testlib/testlib_test.h>
#include <vpdl/vpdl_mixture.h>
#include <vpdl/vpdl_gaussian.h>
#include <vpdl/vpdl_gaussian_sphere.h>
#include <vpdl/vpdl_gaussian_indep.h>
#include <vnl/vnl_random.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>


template <class T>
vpdl_gaussian<T,3> 
fit_gaussian(typename vcl_vector<vnl_vector_fixed<T,3> >::const_iterator begin,
             typename vcl_vector<vnl_vector_fixed<T,3> >::const_iterator end)
{
  vnl_vector_fixed<T,3> mean(T(0));
  vnl_matrix_fixed<T,3,3> covar(T(0));
  unsigned int count = 0;
  typedef typename vcl_vector<vnl_vector_fixed<T,3> >::const_iterator vitr;
  for(vitr i=begin; i!=end; ++i){
    mean += *i;
    covar += outer_product(*i,*i);
    ++count;
  }
  mean /= count;
  covar /= count;
  covar -= outer_product(mean,mean);
  return vpdl_gaussian<T,3>(mean, covar);
}


// function to sort by increasing weight
template <class T, unsigned int n>
bool weight_less(const vpdl_distribution<T,n>& d1, const T& w1,
                 const vpdl_distribution<T,n>& d2, const T& w2)
{
  return w1 < w2;
}

// test the computation of covariance and mean of a mixture
// by using mixtures of gaussians obtained from data points
template <class T>
void test_covar_mean(T epsilon, const vcl_string& type_name)
{
  // an arbitrary collection of data points
  vcl_vector<vnl_vector_fixed<T,3> > data;
  vnl_random rnd;
  for(unsigned int i=0; i<10; ++i)
    data.push_back(vnl_vector_fixed<T,3>(rnd.normal64()+1,
                                         2*rnd.normal64(),
                                         rnd.normal64()));
  for(unsigned int i=0; i<15; ++i)
    data.push_back(vnl_vector_fixed<T,3>(3*rnd.normal64()-1,
                                         rnd.normal64()+3,
                                         1.5*rnd.normal64()));
  for(unsigned int i=0; i<10; ++i)
    data.push_back(vnl_vector_fixed<T,3>(rnd.normal64(),
                                         rnd.normal64()+3,
                                         rnd.normal64()+3));
  
  // make a gaussian for each group of data
  vpdl_gaussian<T,3> gauss1(fit_gaussian<T>(data.begin(),data.begin()+10));
  vpdl_gaussian<T,3> gauss2(fit_gaussian<T>(data.begin()+10,data.begin()+25));
  vpdl_gaussian<T,3> gauss3(fit_gaussian<T>(data.begin()+25,data.begin()+35));
  // make a gaussian for all of the data together
  vpdl_gaussian<T,3> gaussT(fit_gaussian<T>(data.begin(),data.begin()+35));
  
  vpdl_mixture<T,3> mixture;
  mixture.insert(gauss1, 10.0/35);
  mixture.insert(gauss2, 15.0/35);
  mixture.insert(gauss3, 10.0/35);
  
  vnl_vector_fixed<T,3> mean;
  mixture.compute_mean(mean);
  TEST_NEAR(("compute_mean <"+type_name+">").c_str(), 
            (gaussT.mean()-mean).inf_norm(), T(0), epsilon);
  
  vnl_matrix_fixed<T,3,3> covar;
  mixture.compute_covar(covar);
  TEST_NEAR(("compute_covar <"+type_name+">").c_str(), 
            (gaussT.covariance()-covar).array_inf_norm(), T(0), epsilon);
}


template <class T>
void test_mixture_type(T epsilon, const vcl_string& type_name)
{
  // create a few sample distributions
  vnl_vector_fixed<T,3> mean1(T(1), T(1), T(1));
  T var1 = T(0.5);
  
  vnl_vector_fixed<T,3> mean2(T(2), T(0), T(-3));
  vnl_vector_fixed<T,3> var2(T(1), T(0.25), T(2));

  vnl_vector_fixed<T,3> mean3(T(1), T(0), T(0));
  vnl_vector_fixed<T,3> var3(T(0.5), T(1.5), T(0.25));
  
  vcl_cout << "=================== fixed<3> ======================="<<vcl_endl;
  {
    vpdl_gaussian_sphere<T,3> gauss1(mean1,var1);
    vpdl_gaussian_indep<T,3> gauss2(mean2,var2);
    vpdl_gaussian_indep<T,3> gauss3(mean3,var3);
    
    vpdl_mixture<T,3> mixture;
    TEST(("initial num_components <"+type_name+">").c_str(),
         mixture.num_components(), 0);
    
    mixture.insert(gauss1, T(0.2));
    bool valid = mixture.num_components() == 1;
    mixture.insert(gauss2, T(0.5));
    valid = valid && mixture.num_components() == 2;
    TEST(("num_components after insert <"+type_name+">").c_str(),
         valid, true);
    
    const vpdl_distribution<T,3>& dist1 = mixture.distribution(0);
    const vpdl_gaussian_sphere<T,3>* dist1_ptr = 
        dynamic_cast<const vpdl_gaussian_sphere<T,3>*>(&dist1);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1_ptr && dist1_ptr->mean() == gauss1.mean() && 
         dist1_ptr->covariance() == gauss1.covariance(), true);
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
    
    vpdl_mixture<T,3> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    valid = dynamic_cast<const vpdl_gaussian_indep<T,3>*>(&mixture2.distribution(0));
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15) &&
         valid, true);
    
    vnl_vector_fixed<T,3> pt(T(0), T(1.5), T(1));
    T prob = 0.1*gauss1.prob_density(pt) 
            + 0.6*gauss2.prob_density(pt) 
            + 0.3*gauss3.prob_density(pt);
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
         mixture2.prob_density(pt), prob, epsilon);
    
    prob = 0.1*gauss1.cumulative_prob(pt) 
         + 0.6*gauss2.cumulative_prob(pt) 
         + 0.3*gauss3.cumulative_prob(pt);
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt), prob, epsilon);
    
     vnl_vector_fixed<T,3> pt2(T(10), T(5), T(8));
    prob = 0.1*gauss1.box_prob(pt,pt2) 
         + 0.6*gauss2.box_prob(pt,pt2) 
         + 0.3*gauss3.box_prob(pt,pt2);
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt,pt2), prob, epsilon);
    
    mixture2.sort(weight_less<T,3>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) && 
         mixture2.weight(2) == T(0.3), true);
    
    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) && 
         mixture2.weight(2) == T(0.6), true);
  }
  
  // test the covariance and mean computations
  test_covar_mean(epsilon, type_name);
  
  vcl_cout << "=================== scalar ======================="<<vcl_endl;
  
  {
    vpdl_gaussian<T,1> gauss1(mean1[0],var1);
    vpdl_gaussian<T,1> gauss2(mean2[0],var2[0]);
    vpdl_gaussian<T,1> gauss3(mean3[0],var3[0]);
    
    vpdl_mixture<T,1> mixture;
    TEST(("initial num_components <"+type_name+">").c_str(),
         mixture.num_components(), 0);
    
    mixture.insert(gauss1, T(0.2));
    bool valid = mixture.num_components() == 1;
    mixture.insert(gauss2, T(0.5));
    valid = valid && mixture.num_components() == 2;
    TEST(("num_components after insert <"+type_name+">").c_str(),
         valid, true);
    
    const vpdl_distribution<T,1>& dist1 = mixture.distribution(0);
    const vpdl_gaussian<T,1>* dist1_ptr = 
    dynamic_cast<const vpdl_gaussian<T,1>*>(&dist1);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1_ptr && dist1_ptr->mean() == gauss1.mean() && 
         dist1_ptr->covariance() == gauss1.covariance(), true);
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
    
    vpdl_mixture<T,1> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    valid = dynamic_cast<const vpdl_gaussian<T,1>*>(&mixture2.distribution(0));
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15) &&
         valid, true);
    
    T pt = T(1);
    T prob = 0.1*gauss1.prob_density(pt) 
           + 0.6*gauss2.prob_density(pt) 
           + 0.3*gauss3.prob_density(pt);
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
              mixture2.prob_density(pt), prob, epsilon);
    
    prob = 0.1*gauss1.cumulative_prob(pt) 
         + 0.6*gauss2.cumulative_prob(pt) 
         + 0.3*gauss3.cumulative_prob(pt);
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt), prob, epsilon);
    
    T pt2 = T(10);
    prob = 0.1*gauss1.box_prob(pt,pt2) 
         + 0.6*gauss2.box_prob(pt,pt2) 
         + 0.3*gauss3.box_prob(pt,pt2);
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt,pt2), prob, epsilon);
    
    mixture2.sort(weight_less<T,1>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) && 
         mixture2.weight(2) == T(0.3), true);
    
    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) && 
         mixture2.weight(2) == T(0.6), true);
    
    T mean = 0.1*gauss1.mean() + 0.6*gauss2.mean() + 0.3*gauss3.mean();
    T cmp_mean;
    mixture2.compute_mean(cmp_mean);
    TEST_NEAR(("compute_mean <"+type_name+">").c_str(), 
              cmp_mean, mean, epsilon);
    
    T var = 0.1*gauss1.covariance() + 0.6*gauss2.covariance() + 0.3*gauss3.covariance();
    var += 0.1*gauss1.mean()*gauss1.mean() + 0.6*gauss2.mean()*gauss2.mean() 
         + 0.3*gauss3.mean()*gauss3.mean();
    var -= mean*mean;
    T cmp_var;
    mixture2.compute_covar(cmp_var);
    TEST_NEAR(("compute_covar <"+type_name+">").c_str(), 
              cmp_var, var, epsilon);
  }
  
  vcl_cout << "=================== variable ======================="<<vcl_endl;
  {
    vpdl_gaussian_sphere<T> gauss1(mean1,var1);
    vpdl_gaussian_indep<T> gauss2(mean2,var2);
    vpdl_gaussian_indep<T> gauss3(mean3,var3);
    
    vpdl_mixture<T> mixture;
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
    
    const vpdl_distribution<T>& dist1 = mixture.distribution(0);
    const vpdl_gaussian_sphere<T>* dist1_ptr = 
    dynamic_cast<const vpdl_gaussian_sphere<T>*>(&dist1);
    TEST(("distribution access <"+type_name+">").c_str(),
         dist1_ptr && dist1_ptr->mean() == gauss1.mean() && 
         dist1_ptr->covariance() == gauss1.covariance(), true);
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
    
    vpdl_mixture<T> mixture2(mixture);
    mixture.remove_last();
    mixture.remove_last();
    mixture.remove_last();
    valid = dynamic_cast<const vpdl_gaussian_indep<T>*>(&mixture2.distribution(0));
    TEST(("copy constructor <"+type_name+">").c_str(),
         mixture2.num_components() == 3 && mixture2.weight(1) == T(0.15) &&
         mixture2.dimension() == 3 && valid, true);
    
    vnl_vector_fixed<T,3> pt(T(0), T(1.5), T(1));
    T prob = 0.1*gauss1.prob_density(pt) 
           + 0.6*gauss2.prob_density(pt) 
           + 0.3*gauss3.prob_density(pt);
    TEST_NEAR(("probability density <"+type_name+">").c_str(),
              mixture2.prob_density(pt), prob, epsilon);
    
    prob = 0.1*gauss1.cumulative_prob(pt) 
         + 0.6*gauss2.cumulative_prob(pt) 
         + 0.3*gauss3.cumulative_prob(pt);
    TEST_NEAR(("cumulative probability <"+type_name+">").c_str(),
              mixture2.cumulative_prob(pt), prob, epsilon);
    
    vnl_vector_fixed<T,3> pt2(T(10), T(5), T(8));
    prob = 0.1*gauss1.box_prob(pt,pt2) 
         + 0.6*gauss2.box_prob(pt,pt2) 
         + 0.3*gauss3.box_prob(pt,pt2);
    TEST_NEAR(("box probability <"+type_name+">").c_str(),
              mixture2.box_prob(pt,pt2), prob, epsilon);
    
    mixture2.sort(weight_less<T,0>);
    TEST(("sort by increasing weight <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.05) && mixture2.weight(1) == T(0.15) && 
         mixture2.weight(2) == T(0.3), true);
    
    mixture2.normalize_weights();
    TEST(("normalize <"+type_name+">").c_str(),
         mixture2.weight(0) == T(0.1) && mixture2.weight(1) == T(0.3) && 
         mixture2.weight(2) == T(0.6), true);
  }
}


MAIN( test_mixture )
{
  START ("mixture");
  test_mixture_type(float(1e-5),"float");
  test_mixture_type(double(1e-13),"double");
  SUMMARY();
}


