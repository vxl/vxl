#include <testlib/testlib_test.h>
#include <vpdl/vpdl_gaussian_sphere.h>
#include <vpdl/vpdl_gaussian_indep.h>
#include <vpdl/vpdl_gaussian.h>
#include <vpdl/vpdl_update_gaussian.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

template <class T>
void test_update_gaussian_type(T epsilon, const vcl_string& type_name)
{
  // an arbitrary collection of data points
  vcl_vector<vnl_vector_fixed<T,3> > data;
  data.push_back(vnl_vector_fixed<T,3>(1,1,1));
  data.push_back(vnl_vector_fixed<T,3>(2,2,2));
  data.push_back(vnl_vector_fixed<T,3>(3,1,4));
  data.push_back(vnl_vector_fixed<T,3>(-1,2,5));
  data.push_back(vnl_vector_fixed<T,3>(-10,5,0));
  
  // compute the incremental means
  vcl_vector<vnl_vector_fixed<T,3> > means(data.size());
  vnl_vector_fixed<T,3> sum(T(0));
  // compute the incremental variances/covariances
  vcl_vector<T > vars(data.size());
  T vsum = T(0);
  vcl_vector<vnl_vector_fixed<T,3> > diagvars(data.size());
  vnl_vector_fixed<T,3> dvsum(T(0));
  vcl_vector<vnl_matrix_fixed<T,3,3> > covars(data.size());
  vnl_matrix_fixed<T,3,3> cvsum(T(0));
  for(unsigned int i=0; i<data.size(); ++i){
    sum += data[i];
    vsum += dot_product(data[i],data[i]);
    dvsum += element_product(data[i],data[i]);
    cvsum += outer_product(data[i],data[i]);
    
    T divisor = T(1.0)/(i+1);
    means[i] = sum * divisor;
    vars[i] = vsum*divisor - dot_product(means[i],means[i]);
    diagvars[i] = dvsum*divisor - element_product(means[i],means[i]);
    covars[i] = cvsum*divisor - outer_product(means[i],means[i]);
  }
  
  // minimum variance for lower bounding updating tests
  const T min_var = 0.01;
  

  // test the spherical gaussian updating
  {
    vpdl_gaussian_sphere<T,3> gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss3.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss3.mean()<<vcl_endl;
      
      T var_diff = gauss3.covariance() - vars[i];
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "variance should be "<<vars[i]<<" is "<<gauss3.covariance()<<vcl_endl;
    }
    TEST(("sphere mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("sphere variance update <"+type_name+"> fixed").c_str(), test_vars, true);
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("sphere variance bound <"+type_name+"> fixed").c_str(), gauss3.covariance(), min_var);
    
    // test dynamic dimension variant
    vpdl_gaussian_sphere<T> gauss(3);
    test_mean = true;
    test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss.mean()<<vcl_endl;
      
      T var_diff = gauss.covariance() - vars[i];
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "variance should be "<<vars[i]<<" is "<<gauss.covariance()<<vcl_endl;
    }
    TEST(("sphere mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("sphere variance update <"+type_name+"> variable").c_str(), test_vars, true);
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss, T(0.5), data[0], min_var);
    TEST(("sphere variance bound <"+type_name+"> variable").c_str(), gauss.covariance(), min_var);
  }
  
  vcl_cout <<"======================================================\n";
  
  // test the independent gaussian updating
  {
    vpdl_gaussian_indep<T,3> gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss3.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss3.mean()<<vcl_endl;
      
      T var_diff = (gauss3.covariance() - diagvars[i]).inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "variance should be "<<diagvars[i]<<" is "<<gauss3.covariance()<<vcl_endl;
    }
    TEST(("independent mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("independent variance update <"+type_name+"> fixed").c_str(), test_vars, true);   
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("independent variance bound <"+type_name+"> fixed").c_str(), 
         gauss3.covariance().min_value(), min_var);
    
    // test dynamic dimension variant
    vpdl_gaussian_indep<T> gauss(3);
    test_mean = true;
    test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss.mean()<<vcl_endl;
      
      T var_diff = (gauss.covariance() - diagvars[i]).inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "variance should be "<<diagvars[i]<<" is "<<gauss.covariance()<<vcl_endl;
    }
    TEST(("independent mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("independent variance update <"+type_name+"> variable").c_str(), test_vars, true);   
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss, T(0.5), data[0], min_var);
    TEST(("independent variance bound <"+type_name+"> variable").c_str(), 
         gauss.covariance().min_value(), min_var);
  }
  
  vcl_cout <<"======================================================\n";
  
  // test the full generality gaussian updating
  {
    vpdl_gaussian<T,3> gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss3.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss3.mean()<<vcl_endl;
      
      T var_diff = (gauss3.covariance() - covars[i]).array_inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "covariance should be\n"<<covars[i]<<"covariance is\n"<<gauss3.covariance()<<vcl_endl;
    }
    TEST(("general mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("general covariance update <"+type_name+"> fixed").c_str(), test_vars, true);  
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("general covariance bound <"+type_name+"> fixed").c_str(), 
         gauss3.covar_eigenvals().min_value(), min_var);
    
    // test dynamic dimension variant
    vpdl_gaussian<T> gauss(3);
    test_mean = true;
    test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss, T(1.0/(i+1)), data[i]);
      
      T mean_diff = (gauss.mean() - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i]<<" is "<<gauss.mean()<<vcl_endl;
      
      T var_diff = (gauss.covariance() - covars[i]).array_inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "covariance should be\n"<<covars[i]<<"covariance is\n"<<gauss.covariance()<<vcl_endl;
    }
    TEST(("general mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("general covariance update <"+type_name+"> variable").c_str(), test_vars, true);
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss, T(0.5), data[0], min_var);
    TEST(("general covariance bound <"+type_name+"> variable").c_str(), 
         gauss.covar_eigenvals().min_value(), min_var);
  }
  
  vcl_cout <<"======================================================\n";
  
  // test scalar gaussian updating
  {
    vpdl_gaussian<T,1> gauss1;
    bool test_mean = true;
    bool test_vars = true;
    for(unsigned int i=0; i<data.size(); ++i){
      vpdl_update_gaussian(gauss1, T(1.0/(i+1)), data[i][0]);
      
      T mean_diff = gauss1.mean() - means[i][0];
      test_mean = test_mean && (mean_diff < epsilon);
      if(mean_diff >= epsilon)
        vcl_cout << "mean should be "<<means[i][0]<<" is "<<gauss1.mean()<<vcl_endl;
      
      T var_diff = gauss1.covariance() - diagvars[i][0];
      test_vars = test_vars && (var_diff < epsilon);
      if(var_diff >= epsilon)
        vcl_cout << "covariance should be "<<diagvars[i][0]<<" is "<<gauss1.covariance()<<vcl_endl;
    }
    TEST(("scalar mean update <"+type_name+">").c_str(), test_mean, true);
    TEST(("scalar covariance update <"+type_name+">").c_str(), test_vars, true);
    
    // test updating with a lower bound on variance
    for(unsigned int i=0; i<20; ++i)
      vpdl_update_gaussian(gauss1, T(0.5), data[0][0], min_var);
    TEST(("scalar covariance bound <"+type_name+"> fixed").c_str(), 
         gauss1.covariance(), min_var);
    
  }
  
  vcl_cout <<"======================================================\n";
  
}


MAIN( test_update_gaussian )
{
  START ("update gaussian");
  test_update_gaussian_type(float(1e-5),"float");
  test_update_gaussian_type(double(1e-14),"double");
  SUMMARY();
}


