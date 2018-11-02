#ifndef bsta_kent_hxx_
#define bsta_kent_hxx_

#include "bsta_kent.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_bessel.h>
#include <vnl/vnl_gamma.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#if 0
#include <vnl/vnl_inverse.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#endif
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bsta_kent<T>::bsta_kent(vnl_matrix_fixed<T,3,3> const& m)
{
  vnl_symmetric_eigensystem<T> E(m.as_ref());
  T t3 = E.get_eigenvalue(0); // smallest ??
  T t2 = E.get_eigenvalue(1);
  T t1 = E.get_eigenvalue(2); // largest??

  assert(t1 > t3);

  gamma3_ = E.get_eigenvector(0);  // check if this is the smallest values's vector
  gamma2_ = E.get_eigenvector(1);
  gamma1_ = E.get_eigenvector(2);

  std::cout << "gamma1->" << gamma1_ << std::endl
           << "gamma2->" << gamma2_ << std::endl
           << "gamma3->" << gamma3_ << std::endl;

  // compute the kent distr. parameters
  vnl_matrix<T> g3(3,1);
  g3.put(0,0,gamma3_.get(0));
  g3.put(1,0,gamma3_.get(1));
  g3.put(2,0,gamma3_.get(2));
  vnl_matrix<T> g3t = g3.transpose();
  vnl_matrix<T> res=(g3t*m*g3);
  // make sure that res is 1x1
  T R = 1 - res.get(0,0);
  T Q = std::abs(t3-t2);

  kappa_= (1./(2.-2.*R-Q))+(1./(2.-2.*R+Q)); // always >= 0
  beta_= 0.5 * kappa_;
  std::cout << "kappa=" << kappa_ << "beta=" << beta_ << std::endl;
}

#if 0
template <class T>
bsta_kent<T>::bsta_kent(std::vector<vgl_plane_3d<T> > const& planes)
{
  vnl_matrix<T> X(3,3,0);
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
  std::cout << X << std::endl;

  vnl_matrix<T> X_inv = vnl_inverse(X);
  vnl_symmetric_eigensystem<T> E(X_inv);
  T t3 = E.get_eigenvalue(0); // smallest ??
  T t2 = E.get_eigenvalue(1);
  T t1 = E.get_eigenvalue(2); // largest??

  assert(t1 > t3);

  gamma3_ = E.get_eigenvector(0);  // check if this is the smallest values's vector
  gamma2_ = E.get_eigenvector(1);
  gamma1_ = E.get_eigenvector(2);

  std::cout << "gamma1->" << gamma1_ << std::endl
           << "gamma2->" << gamma2_ << std::endl
           << "gamma3->" << gamma3_ << std::endl;

  // compute the kent distr. parameters
  vnl_matrix<T> g3(3,1);
  g3.put(0,0,gamma3_.get(0));
  g3.put(1,0,gamma3_.get(1));
  g3.put(2,0,gamma3_.get(2));
  vnl_matrix<T> g3t = g3.transpose();
  vnl_matrix<T> res=(g3t*X_inv*g3);
  // make sure that res is 1x1
  T R = 1 - res.get(0,0);
  T Q = std::abs(t3-t2);

  kappa_= (1./(2.-2.*R-Q))+(1./(2.-2.*R+Q)); // always >= 0
  beta_= 0.5 * kappa_;
  std::cout << "kappa=" << kappa_ << "beta=" << beta_ << std::endl;
}
#endif // 0

template <class T>
T bsta_kent<T>::prob_density(vnl_vector_fixed<T,3> const& x)
{
  T a=dot_product(x,gamma3_);
  T b=dot_product(x,gamma2_);
  T c=dot_product(x,gamma1_);
  T e=std::exp((kappa_*a)+(beta_*(b*b - c*c)));
  return e * normalizing_const();
}

template <class T>
T bsta_kent<T>::normalizing_const(T kappa, T beta)
{
  if (kappa<=0) kappa=kappa_; // the default
  if (beta <=0) beta= beta_;  // the default
  T denom = vnl_math::twopi * vnl_math::sqrt2pi;
  denom*=std::pow(kappa, T(-0.5)); // so kappa should not be zero!
  T sum=0;
  for (int r=0; r<20; r++) {
    T f = vnl_gamma(r+1);
    T two_f = vnl_gamma(2*r+1);
    T x = std::pow(beta/kappa,2*r)*vnl_bessel(2*r/*+T(0.5)*/,kappa);
    sum += static_cast<T>(T(two_f)/T(f*f))* x;
  }
  denom*=sum; // which is 0 when beta==0, so also beta should not be zero!
  return T(1.0)/denom;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, bsta_kent<T> const& sample)
{
  vsl_b_write(os, sample.version());
  vsl_b_write(os, sample.kappa());
  vsl_b_write(os, sample.beta());
  vsl_b_write<T,3>(os, sample.minor_axis());
  vsl_b_write<T,3>(os, sample.major_axis());
  vsl_b_write<T,3>(os, sample.mean_direction());
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, bsta_kent<T> const* &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, bsta_kent<T> &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      {T kappa, beta;
      vnl_vector_fixed<T,3> v1, v2, v3;
      vsl_b_read(is, kappa);
      vsl_b_read(is, beta);
      vsl_b_read<T,3>(is, v1);
      vsl_b_read<T,3>(is, v2);
      vsl_b_read<T,3>(is, v3);
      sample=bsta_kent<T>(kappa,beta,v1,v2,v3);
      break;}
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, bsta_kent<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

template <class T>
void vsl_b_read(vsl_b_istream & is, bsta_kent<T> *&sample)
{
  vsl_b_read(is, *sample);
}

template <class T>
std::ostream& operator<< (std::ostream& os, bsta_kent<T>& b)
{
  return os << "kent: (kappa,beta) = (" << b.kappa() << "  " << b.beta() << ")\n";
}

#undef BSTA_KENT_INSTANTIATE
#define BSTA_KENT_INSTANTIATE(T) \
template class bsta_kent<T >; \
template void vsl_b_write(vsl_b_ostream &, bsta_kent<T > const&); \
template void vsl_b_write(vsl_b_ostream &, bsta_kent<T > const*&); \
template void vsl_b_read(vsl_b_istream &, bsta_kent<T > &); \
template void vsl_b_read(vsl_b_istream &, bsta_kent<T > *&); \
template std::ostream& operator << (std::ostream &, bsta_kent<T > &)

#endif
