#include <string>
#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpdl/vpdt/vpdt_update_gaussian.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void test_update_gaussian_type(T epsilon, const std::string& type_name)
{
  // an arbitrary collection of data points
  std::vector<vnl_vector_fixed<T,3> > data;
  data.push_back(vnl_vector_fixed<T,3>(1,1,1));
  data.push_back(vnl_vector_fixed<T,3>(2,2,2));
  data.push_back(vnl_vector_fixed<T,3>(3,1,4));
  data.push_back(vnl_vector_fixed<T,3>(-1,2,5));
  data.push_back(vnl_vector_fixed<T,3>(-10,5,0));

  // compute the incremental means
  std::vector<vnl_vector_fixed<T,3> > means(data.size());
  vnl_vector_fixed<T,3> sum(T(0));
  // compute the incremental variances/covariances
  std::vector<T > vars(data.size());
  T vsum = T(0);
  std::vector<vnl_vector_fixed<T,3> > diagvars(data.size());
  vnl_vector_fixed<T,3> dvsum(T(0));
  std::vector<vnl_matrix_fixed<T,3,3> > covars(data.size());
  vnl_matrix_fixed<T,3,3> cvsum(T(0));
  for (unsigned int i=0; i<data.size(); ++i){
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
  const T min_var = T(0.01);

  // test the spherical gaussian updating
  std::cout <<"================= vpdt_gaussian (spherical) =================\n";
  {
    vpdt_gaussian<vnl_vector_fixed<T,3>, T> gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss3.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss3.mean<<std::endl;

      T var_diff = gauss3.covar - vars[i];
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "variance should be "<<vars[i]
                 <<" is "<<gauss3.covar<<" difference is "<<var_diff<<std::endl;
    }
    TEST(("sphere mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("sphere variance update <"+type_name+"> fixed").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("sphere variance bound <"+type_name+"> fixed").c_str(), gauss3.covar, min_var);

    // test dynamic dimension variant
    vpdt_gaussian<vnl_vector_fixed<T,3>,T> gauss(3);
    test_mean = true;
    test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss.mean<<std::endl;

      T var_diff = gauss.covar - vars[i];
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "variance should be "<<vars[i]
                 <<" is "<<gauss.covar<<" difference is "<<var_diff<<std::endl;
    }
    TEST(("sphere mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("sphere variance update <"+type_name+"> variable").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss, T(0.5), data[0], min_var);

    TEST(("sphere variance bound <"+type_name+"> variable").c_str(), gauss.covar, min_var);
  }

  // test the independent gaussian updating
  std::cout <<"================ vpdt_gaussian (independent) ================\n";
  {
    vpdt_gaussian<vnl_vector_fixed<T,3>, vnl_vector_fixed<T,3> > gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss3.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss3.mean<<std::endl;

      T var_diff = (gauss3.covar - diagvars[i]).inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "variance should be "<<diagvars[i]
                 <<" is "<<gauss3.covar<<" difference is "<<var_diff<<std::endl;
    }
    TEST(("independent mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("independent variance update <"+type_name+"> fixed").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("independent variance bound <"+type_name+"> fixed").c_str(),
         gauss3.covar.min_value(), min_var);

    // test dynamic dimension variant
    vpdt_gaussian<vnl_vector_fixed<T,3>,vnl_vector_fixed<T,3> > gauss(3);
    test_mean = true;
    test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i) {
      vpdt_update_gaussian(gauss, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss.mean<<std::endl;

      T var_diff = (gauss.covar - diagvars[i]).inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "variance should be "<<diagvars[i]
                 <<" is "<<gauss.covar<<" difference is "<<var_diff<<std::endl;
    }
    TEST(("independent mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("independent variance update <"+type_name+"> variable").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss, T(0.5), data[0], min_var);
    TEST(("independent variance bound <"+type_name+"> variable").c_str(),
         gauss.covar.min_value(), min_var);
  }

  // test the full generality gaussian updating
  std::cout <<"================= vpdt_gaussian (general) =================\n";
  {
    vpdt_gaussian<vnl_vector_fixed<T,3> > gauss3;
    bool test_mean = true;
    bool test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss3, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss3.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss3.mean<<std::endl;

      vnl_matrix_fixed<T,3,3> covariance;
      gauss3.compute_covar(covariance);
      T var_diff = (covariance - covars[i]).array_inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "covariance should be\n"<<covars[i]
                 <<"covariance is\n"<<covariance
                 <<"difference is "<<var_diff<<std::endl;
    }
    TEST(("general mean update <"+type_name+"> fixed").c_str(), test_mean, true);
    TEST(("general covariance update <"+type_name+"> fixed").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss3, T(0.5), data[0], min_var);
    TEST(("general covariance bound <"+type_name+"> fixed").c_str(),
         gauss3.covar.eigenvalues().min_value(), min_var);

    // test dynamic dimension variant
    vpdt_gaussian<vnl_vector_fixed<T,3> > gauss(3);
    test_mean = true;
    test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss, T(1.0/(i+1)), data[i]);

      T mean_diff = (gauss.mean - means[i]).inf_norm();
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i]<<" is "<<gauss.mean<<std::endl;

      vnl_matrix_fixed<T,3,3> covariance;
      gauss.compute_covar(covariance);
      T var_diff = (covariance - covars[i]).array_inf_norm();
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "covariance should be\n"<<covars[i]
                 <<"covariance is\n"<<covariance
                 <<"difference is "<<var_diff<<std::endl;
    }
    TEST(("general mean update <"+type_name+"> variable").c_str(), test_mean, true);
    TEST(("general covariance update <"+type_name+"> variable").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss, T(0.5), data[0], min_var);
    TEST(("general covariance bound <"+type_name+"> variable").c_str(),
         gauss.covar.eigenvalues().min_value(), min_var);
  }

  // test scalar gaussian updating
  std::cout <<"================= vpdt_gaussian (scalar) =================\n";
  {
    vpdt_gaussian<T> gauss1;
    bool test_mean = true;
    bool test_vars = true;
    for (unsigned int i=0; i<data.size(); ++i){
      vpdt_update_gaussian(gauss1, T(1.0/(i+1)), data[i][0]);

      T mean_diff = gauss1.mean - means[i][0];
      test_mean = test_mean && (mean_diff < epsilon);
      if (mean_diff >= epsilon)
        std::cout << "mean should be "<<means[i][0]<<" is "<<gauss1.mean<<std::endl;

      T var_diff = gauss1.covar - diagvars[i][0];
      test_vars = test_vars && (var_diff < epsilon);
      if (var_diff >= epsilon)
        std::cout << "covariance should be "<<diagvars[i][0]<<" is "<<gauss1.covar<<std::endl;
    }
    TEST(("scalar mean update <"+type_name+">").c_str(), test_mean, true);
    TEST(("scalar covariance update <"+type_name+">").c_str(), test_vars, true);

    // test updating with a lower bound on variance
    for (unsigned int i=0; i<20; ++i)
      vpdt_update_gaussian(gauss1, T(0.5), data[0][0], min_var);
    TEST(("scalar covariance bound <"+type_name+"> fixed").c_str(),
         gauss1.covar, min_var);
  }
}


static void test_update_gaussian()
{
  test_update_gaussian_type(1e-5f,"float");
  test_update_gaussian_type(1e-13,"double");
}

TESTMAIN(test_update_gaussian);
