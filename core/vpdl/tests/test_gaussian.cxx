#include <testlib/testlib_test.h>
#include <vpdl/vpdl_gaussian.h>
#include <vcl_string.h>
#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vcl_iostream.h>

template <class T>
void test_gaussian_type(T epsilon, const vcl_string& type_name)
{
  
  vpdl_gaussian<T,3> df_gauss3;
  vpdl_gaussian<T,1> df_gauss1;
  vpdl_gaussian<T> df_gauss;
  
  vnl_vector_fixed<T,3> mean(T(1.0), T(2.0), T(4.0));
  vnl_matrix_fixed<T,3,3> covar(T(0));
  
  covar(0,0) = (T)0.5; covar(0,1) = (T)0.2; covar(0,2) = (T)0.1;
  covar(1,0) = (T)0.2; covar(1,1) = (T)0.3; covar(1,2) = (T)0.1;
  covar(2,0) = (T)0.1; covar(2,1) = (T)0.1; covar(2,2) = (T)0.4;

  //:FIXME add tests here

}

MAIN( test_gaussian )
{
  test_gaussian_type(float(1e-5),"float");
  test_gaussian_type(double(1e-14),"double");
  SUMMARY();
}

