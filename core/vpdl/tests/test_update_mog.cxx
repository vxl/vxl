#include <testlib/testlib_test.h>
#include <vpdl/vpdt/vpdt_update_mog.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

template <class T>
void test_update_mog_type(T epsilon, const vcl_string& type_name)
{
  // an arbitrary collection of data points
  vcl_vector<vnl_vector_fixed<T,3> > data;
  data.push_back(vnl_vector_fixed<T,3>(1,1,1));
  data.push_back(vnl_vector_fixed<T,3>(2,2,2));
  data.push_back(vnl_vector_fixed<T,3>(3,1,4));
  data.push_back(vnl_vector_fixed<T,3>(-1,2,5));
  data.push_back(vnl_vector_fixed<T,3>(-10,5,0));
  
  typedef vpdt_gaussian<vnl_vector_fixed<T,3>,T> gauss3_t;
  typedef vpdt_mixture_of<gauss3_t> mog_t;
  
  vpdt_mog_sg_updater<mog_t> mog_updater(gauss3_t(vnl_vector_fixed<T,3>(0.0),1));
  
  mog_t mixture;
  
  mog_updater(mixture, data[0]);
  
}


MAIN( test_update_mog )
{
  START ("update mixture of gaussians");
  test_update_mog_type(float(1e-5),"float");
  test_update_mog_type(double(1e-13),"double");
  SUMMARY();
}


