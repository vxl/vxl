// This is core/vnl/vnl_polynomial.cxx
#ifndef vnl_polynomial_txx_
#define vnl_polynomial_txx_

#include "vnl_polynomial.h"
//:
// \file
// \brief Evaluation of polynomials - the implementation
// Templated class (on the data type of the coefficients),
// further very similar to the vnl_real_polynomial class.
// \author Peter Vanroose, ABIS Leuven.
// \date  August 2011

#include <vcl_iostream.h>

//: Evaluate polynomial at value x
template <class T>
T vnl_polynomial<T>::evaluate(T const& x) const
{
  unsigned n = coeffs_.size();
  if (n==0) return T(0);
  T acc = coeffs_[--n];
  T xn = x; // x^n
  while (n--) {
    acc += coeffs_[n] * xn;
    xn *= x;
  }
  return acc;
}

//: Returns negative of this polynomial
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::operator-() const
{
  int n=coeffs_.size();
  vcl_vector<T> neg; neg.reserve(n);
  for (int i=0;i<n;++i) neg.push_back(-coeffs_[i]);
  return vnl_polynomial<T>(neg);
}

//: Returns polynomial which is sum of this with polynomial f
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::operator+(vnl_polynomial<T> const& f) const
{
  // Degree of result is at most the maximum of the two input degrees
  int d=degree(), d2=f.degree(); // might be -1 !
  vcl_vector<T> sum = coeffs_;
  // Coefficient of x^(d-i) is f(i)
  if (d < d2) { sum.insert(sum.begin(), d2-d, T(0)); d=d2; }
  for (int i=d,j=d2;j>=0;--i,--j) sum[i]+=f[j];
  // The higher order coefficients could now be zero, which effectively reduces the degree:
  while (sum.size() > 0 && sum[0] == T(0)) sum.erase(sum.begin());
  return vnl_polynomial<T>(sum);
}

//: Returns polynomial which is product of this with polynomial f
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::operator*(vnl_polynomial<T> const& f) const
{
  int d1=degree(), d2=f.degree(), d = d1+d2;
  if (d1<0 || d2<0) return vnl_polynomial<T>(); // one of the factors is zero
  vcl_vector<T> prod(d+1, T(0));
  for (int i=0;i<=d1;++i)
    for (int j=0;j<=d2;++j)
      prod[d-(i+j)] += coeffs_[d1-i]*f[d2-j];
  return vnl_polynomial<T>(prod);
}

//: Returns polynomial which is the result of a long division by f
// Beware that this operation might not make sense for integral types T
// if the highest order coefficient of f is not 1 or -1!
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::operator/(vnl_polynomial<T> const& f) const
{
  int d1=degree(), d2=f.degree(), d = d1-d2;
  assert (d2 >= 0 && f[0] != T(0)); // denominator should not be zero
  if (d<0) return vnl_polynomial<T>(); // nominator is zero, or degree of denominator is larger
  vcl_vector<T> quot(d+1, T(0));
  for (int i=0;i<=d;++i) {
    quot[i] = coeffs_[i];
    int j=i-d2; if (j<0) j=0;
    for (;j<i;++j) quot[i] -= f[i-j] * quot[j];
    quot[i] /= f[0];
  }
  return vnl_polynomial<T>(quot);
}

//: Returns polynomial which is the remainder after long division by f
// Beware that this operation might not make sense for integral types T
// if the highest order coefficient of f is not 1 or -1!
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::operator%(vnl_polynomial<T> const& f) const
{
  int n=f.degree(), m=coeffs_.size()-n;
  vnl_polynomial<T> quot = operator/(f);
  if (quot.degree() < 0) return *this;
  vcl_vector<T> prod = (f * quot).coefficients();
  prod.erase(prod.begin(), prod.begin()+m);
  vcl_vector<T> diff = coeffs_;
  diff.erase(diff.begin(), diff.begin()+m);
  for (int i=0; i<=n; ++i) diff[i] -= prod[i];
  while (diff.size() > 0 && diff[0] == T(0)) diff.erase(diff.begin());
  return vnl_polynomial<T>(diff);
}

//: Return derivative of this polynomial
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::derivative() const
{
  int d = coeffs_.size()-1; // degree of source polynomial
  vcl_vector<T> cd(d);
  for (int i=d-1,di=1; i>=0; --i,++di)
    cd[i] = coeffs_[i] * di;
  return vnl_polynomial<T>(cd);
}

//: Return primitive function (inverse derivative) of this polynomial
// Since a primitive function is not unique, the one with constant = 0 is returned
// Beware that this operation might not make sense for integral types T!
template <class T>
vnl_polynomial<T> vnl_polynomial<T>::primitive() const
{
  int d = coeffs_.size(); // degree+1
  vcl_vector<T> cd(d+1);
  cd[d] = T(0);
  for (int i=d-1,di=1; i>=0; --i,++di)
    cd[i] = coeffs_[i] / di;
  return vnl_polynomial<T>(cd);
}

template <class T>
void vnl_polynomial<T>::print(vcl_ostream& os) const
{
  int d = degree();
  bool first_coeff = true;

  for (int i=0; i <= d; ++i) {
    if (coeffs_[i] == T(0)) continue;
    os << ' ';
    if (coeffs_[i] > T(0) && !first_coeff) os << '+';
    if (i==d)                     os << coeffs_[i];
    else if (coeffs_[i] == -T(1)) os << '-';
    else if (coeffs_[i] != T(1))  os << coeffs_[i] << ' ';
    if (i < d-1)                  os << "X^" << d-i;
    else if (i == d-1)            os << 'X';
    first_coeff = false;
  }
  if (first_coeff) os << " 0";
}

#undef VNL_POLYNOMIAL_INSTANTIATE
#define VNL_POLYNOMIAL_INSTANTIATE(T) \
template class vnl_polynomial<T >; \
template vcl_ostream& operator<<(vcl_ostream& os, vnl_polynomial<T > const&)

#endif // vnl_polynomial_txx_
