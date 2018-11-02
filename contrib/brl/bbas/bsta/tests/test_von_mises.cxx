#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include <bsta/bsta_von_mises.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> // for pi

// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)

template <class T>
void test_von_mises_type(T epsilon, const std::string& type_name)
{
  //test default constructor
  bsta_von_mises<T,2> vm2;
  const typename bsta_von_mises<T,2>::vector_type& mean2 = vm2.mean();
  typename bsta_von_mises<T,2>::vector_type v02(T(0));
  v02[1]=T(1);
  T kappa2 = vm2.kappa();

  bsta_von_mises<T,3> vm3;
  const typename bsta_von_mises<T,3>::vector_type& mean3 = vm3.mean();
  typename bsta_von_mises<T,3>::vector_type v03(T(0)),v03a(T(0));
  v03[2]=T(1);
  T kappa3 = vm3.kappa();
  bool good = mean2 == v02 && mean3 == v03 && kappa2 == 1 && kappa3 == 1;
  TEST(("default constructors <"+type_name+">").c_str(),good, true);

  //test probability density
  //  2-d case
  typename bsta_von_mises<T,2>::vector_type v12(T(0)), v22(T(0)), v32(T(0));
  v12[0]=T(1); v22[1]=T(1); v32[1]=T(1);
  T r1 = vm2.prob_density(v12)/T(0.12570826359722015);
  T r2 = vm2.prob_density(v22)/T(0.3417104886234632);
  vm2.set_kappa(T(10));
  T r3 = vm2.prob_density(v32)/T(1.2450190581700005);
  std::string test = "Probability density 2-d <"+type_name+">";
  TEST_NEAR(test.c_str(), r1+r2+r3, 3, epsilon);
  //  3-d case
  typename bsta_von_mises<T,3>::vector_type v13(T(0));
  double s2d = 1.0/std::sqrt(2.0);
  T s2 = static_cast<T>(s2d);
  v13[0]=s2; v13[2]=s2;
  T r4 = vm3.prob_density(v03);
  r4 /= T(0.184065499616596);
  T r5 = vm3.prob_density(v13);
  r5 /= T(0.13733160170094183);
  vm3.set_kappa(T(10));
  T r6 = vm3.prob_density(v13);
  r6 /= T(0.08507482126667297);
  std::string testb = "Probability density 3-d <"+type_name+">";
  TEST_NEAR(testb.c_str(), r4+r5+r6, 3, epsilon);
  //test probability
  v03a[0]=T(0.707);v03a[2]=T(0.707);
  vm3.set_kappa(T(1));
  T prob = vm3.probability(v03a,T(vnl_math::pi));
  std::string testa = "Probability 3-d <"+type_name+">";
  TEST_NEAR(testa.c_str(), prob, 0.998141, 1.0e-06);
}


static void test_von_mises()
{
  test_von_mises_type(float(1e-5),"float");
  test_von_mises_type(double(1e-7),"double");
}

TESTMAIN(test_von_mises);
