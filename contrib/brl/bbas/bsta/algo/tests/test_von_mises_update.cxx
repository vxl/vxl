#include <string>
#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <bsta/algo/bsta_von_mises_updater.h>
#include <bsta/bsta_attributes.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)

// The 2-d von mises cumulative distribution cannot be expressed in
// closed form. The table below for F(theta) is based on kappa = 10.
template <class T>
T sample_2d()
{
  T th[63]={T(-3.14159),T(-3.04159),T(-2.94159),T(-2.84159),T(-2.74159),T(-2.64159),T(-2.54159),T(-2.44159),T(-2.34159),T(-2.24159),T(-2.14159),T(-2.04159),T(-1.94159),T(-1.84159),T(-1.74159),T(-1.64159),T(-1.54159),T(-1.44159),T(-1.34159),T(-1.24159),T(-1.14159),T(-1.04159),T(-0.941593),T(-0.841593),T(-0.741593),T(-0.641593),T(-0.541593),T(-0.441593),T(-0.341593),T(-0.241593),T(-0.141593),T(-0.0415927),T(0.0584073),T(0.158407),T(0.258407),T(0.358407),T(0.458407),T(0.558407),T(0.658407),T(0.758407),T(0.858407),T(0.958407),T(1.05841),T(1.15841),T(1.25841),T(1.35841),T(1.45841),T(1.55841),T(1.65841),T(1.75841),T(1.85841),T(1.95841),T(2.05841),T(2.15841),T(2.25841),T(2.35841),T(2.45841),T(2.55841),T(2.65841),T(2.75841),T(2.85841),T(2.95841),T(3.05841)};
  T F[63]={0.,T(2.6095722659260293e-10),T(5.495264987441105e-10),T(9.020304479490863e-10),T(1.3767321561343852e-9),T(2.0793149411956304e-9),T(3.2176050649407998e-9),T(5.226452812295283e-9),T(9.065857218918642e-9),T(1.6960623731203717e-8),T(3.42983263259157e-8),T(7.46401210328259e-8),T(1.7324941502870866e-7),T(4.2418306188751966e-7),T(1.0827355917064834e-6),T(2.84795706721355e-6),T(7.633043295732147e-6),T(0.0000206206),T(0.0000555655),T(0.00014786),T(0.000384842),T(0.000970898),T(0.00235433),T(0.00544528),T(0.0119301),T(0.0246132),T(0.0475855),T(0.0858954),T(0.144425),T(0.22601),T(0.329426),T(0.448365),T(0.572307),T(0.689283),T(0.78933),T(0.866992),T(0.921841),T(0.957206),T(0.978112),T(0.989501),T(0.995254),T(0.997965),T(0.999167),T(0.999672),T(0.999874),T(0.999953),T(0.999983),T(0.999994),T(0.999998),T(0.999999),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.),T(1.)};

  double x = std::rand()/(1.0+RAND_MAX);
  for (unsigned i = 0; i<63; ++i)
    if (F[i]>=x)
      return th[i];
  return T(0);
};

//generate samples from a 3-d von mises distribution. The mean vector is
//along the z axis.
template <class T>
vgl_point_2d<T> sample_3d(T kappa)
{
  double phi = vnl_math::twopi*(std::rand()/(1.0+RAND_MAX));
  T phi_T = static_cast<T>(phi);
  double p = std::rand()/(1.0+RAND_MAX);
  double exk = std::exp(2.0*kappa);
  double t1 = exk-1.0;
  double t2 =exk/t1;
  double t3 = std::log(t1*(t2-p));
  double theta = std::acos((-kappa + t3)/kappa);
  T theta_T = static_cast<T>(theta);
  return vgl_point_2d<T>(theta_T, phi_T);
}

template <class T>
void test_von_mises_update_type(T epsilon, const std::string& type_name)
{
  //Test 2-d von mises update
  bsta_von_mises_updater<bsta_von_mises<T, 2> > vm_2d_updater;
  bsta_vsum_num_obs<bsta_von_mises<T, 2> > obvm2;
  for (unsigned i = 0; i<500; ++i){
    T theta  = sample_2d<T>();
    typename bsta_von_mises<T, 2>::vector_type s(T(0));//sample
    s[0]=std::cos(theta);
    s[1]=std::sin(theta);
    vm_2d_updater(obvm2, s);
    }
  std::cout << obvm2.kappa() << ' ' << (obvm2.mean())[0] << '\n';
  TEST_NEAR("Estimate Von Mises 2-d Kappa",obvm2.kappa()/T(10), T(1), T(0.2));
  TEST_NEAR("Estimate Von Mises 2-d Mean z", (obvm2.mean())[0], T(1), T(0.1));
  //Test 3-d von mises update
  bsta_von_mises_updater<bsta_von_mises<T, 3> > vm_3d_updater;
  //A set of random samples for theta and phi about the mean
  //without loss of generality it can be assumed that the mean
  //direction is (0, 0, 1)^t.
  unsigned n = 100;
  std::cout << "Test parameter estimation for 3-d von mises distribution\n"
           << "kappa" << ' ' << "est_kappa" << ' ' << "mean z, "<< n << " samples, type =" << type_name << '\n';
  T ksum = T(0), msum = T(0);
  T norm = T(0);
  for (T kappa = T(1); kappa<=T(20); kappa+=2){
    bsta_vsum_num_obs<bsta_von_mises<T, 3> > obvm3;
    for (unsigned i = 0; i<n; ++i){
      vgl_point_2d<T> p = sample_3d(kappa);
      T theta = p.x(), phi = p.y();
      typename bsta_von_mises<T, 3>::vector_type s(T(0));//sample
      s[0]=std::sin(theta)*std::cos(phi);
      s[1]=std::sin(theta)*std::sin(phi);
      s[2]=std::cos(theta);
      vm_3d_updater(obvm3, s);
    }
    std::cout << kappa << ' ' << obvm3.kappa() << ' ' << (obvm3.mean())[2] << '\n';
    ksum += obvm3.kappa()/kappa;
    msum += (obvm3.mean())[2];
    norm += T(1);
  }
  ksum /=norm; msum /=norm;
  std::cout << "normalized kappa "<< ksum << "  normalized mean z "<< msum << '\n';
  TEST_NEAR("Estimate Von Mises 3-d Kappa", ksum, T(1), 10000*epsilon);
  TEST_NEAR("Estimate Von Mises 3-d Mean z", msum, T(1), 10000*epsilon);
}


static void test_von_mises_update()
{
  test_von_mises_update_type(1e-5f,"float");
  test_von_mises_update_type(1e-5 ,"double");
}

TESTMAIN(test_von_mises_update);
