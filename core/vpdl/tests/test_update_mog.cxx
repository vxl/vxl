#include <string>
#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpdl/vpdt/vpdt_update_mog.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void test_update_mog_type(T epsilon, const std::string& type_name, T inf)
{
  // an arbitrary collection of data points
  std::cout << "*************************\n"
           << " testing for type " << type_name << '\n'
           << "*************************\n";
  std::vector<vnl_vector_fixed<T,3> > data;
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
  TEST("dimension", mixture.dimension(), 3);
  TEST("# components", mixture.num_components(), 1);
  TEST("weight", mixture.weight(0), 1);
  TEST_NEAR("cumulative prob at minus inf", mixture.cumulative_prob(vnl_vector_fixed<T,3>(-inf,-inf,-inf)), 0, epsilon);
  TEST_NEAR("cumulative prob at plus inf", mixture.cumulative_prob(vnl_vector_fixed<T,3>(inf,inf,inf)), 1, epsilon);
  TEST("norm_const", mixture.norm_const(), 1);
  vnl_vector_fixed<T,3> mean; mixture.compute_mean(mean);
  vnl_vector_fixed<T,3> exact_mean(1,1,1);
  TEST("mean", mean, exact_mean);
}


static void test_update_mog()
{
  test_update_mog_type(1e-5f,"float", 1e29f);
  test_update_mog_type(1e-13,"double",1e29);
}

TESTMAIN(test_update_mog);
