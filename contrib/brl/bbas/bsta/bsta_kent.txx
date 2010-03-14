#ifndef bsta_kent_txx_
#define bsta_kent_txx_

#include "bsta_kent.h"
#include <vnl/vnl_bignum.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_bessel.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vcl_cassert.h>

// Factorial
static inline vnl_bignum factorial(int n)
{
  if (n <= 1)
    return vnl_bignum(1);
  else
    return n * factorial(n-1);
}

template <class T>
bsta_kent<T>::bsta_kent(vcl_vector<vgl_plane_3d<T> > planes)
{
  vnl_matrix<T> X(3,3);
  for (unsigned i=0; i<planes.size(); i++) {
    vgl_plane_3d<T> plane = planes[i];
    vgl_vector_3d<T> normal = plane.normal();
    vnl_matrix<T> n(1,3);
    n.put(0,0,normal.x());
    n.put(0,1,normal.y());
    n.put(0,2,normal.z());
    vnl_matrix<T> nt = n.transpose();
    X += nt*n;
  }
  X/=planes.size();

  vnl_matrix<T> X_inv = vnl_inverse(X);
  vnl_symmetric_eigensystem<T> E(X_inv);
  T t3 = E.get_eigenvalue(0); // smallest ??
  T t2 = E.get_eigenvalue(1);
  T t1 = E.get_eigenvalue(2); // largest??

  assert(t1 > t3);

  gamma3_ = E.get_eigenvector(0);  // check if this is the smallest values's vector
  gamma2_ = E.get_eigenvector(1);
  gamma1_ = E.get_eigenvector(2);

  // compute the kent distr. parameters
  vnl_matrix<T> g3(3,1);
  g3.put(0,0,gamma3_.get(0));
  g3.put(1,0,gamma3_.get(1));
  g3.put(2,0,gamma3_.get(2));
  vnl_matrix<T> g3t = g3.transpose();
  vnl_matrix<T> res=(g3t*X_inv*g3);
  // make sure that res is 1x1
  T R = 1 - res.get(0,0);
  T Q = vcl_abs(t1-t2);

  kappa_= (1./(2.-2.*R-Q))+(1./(2.-2.*R+Q));
  beta_= ((1./(2.-2.*R-Q))-(1./(2.-2.*R+Q)))/2.0;
}

template <class T>
T bsta_kent<T>::prob_density(vnl_vector_fixed<T,3> x)
{
  //vnl_vector_fixed<T,3> nc = normalizing_const(kappa_,beta); what is expected here a vector or a value?
  T nc = normalizing_const(kappa_,beta_);
  T a=dot_product(x,gamma3_);
  T b=dot_product(x,gamma2_);
  T c=dot_product(x,gamma1_);
  T e=vcl_exp((kappa_*a)+(beta_*(b*b - c*c)));
  T p=nc*e;
  return p;
}

template <class T>
T bsta_kent<T>::normalizing_const(T kappa, T beta)
{
  T denom = vcl_pow(2*vnl_math::pi, 1.5);
  denom*=vcl_pow(kappa_, -0.5);
  T sum=0;
  for (int r=0; r<15; r++) {
    T f = factorial(r);
    T x = vcl_pow(beta_/kappa_,2*r)*vnl_bessel(2*r/*+T(0.5)*/,kappa_);
    sum += static_cast<T>(factorial(2*r)/(f*f))* x;
  }
  denom*=sum;
  return T(1.0)/denom;
}

#undef BSTA_KENT_INSTANTIATE
#define BSTA_KENT_INSTANTIATE(T) \
template class bsta_kent<T >

#endif
