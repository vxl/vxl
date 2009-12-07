// This is core/vgl/vgl_vector_2d.txx
#ifndef vgl_vector_2d_txx_
#define vgl_vector_2d_txx_
//:
// \file

#include "vgl_vector_2d.h"

#include <vcl_cmath.h> // sqrt() , acos()
#include <vcl_iostream.h>

template <class T>
double vgl_vector_2d<T>::length() const
{
  return vcl_sqrt( 0.0+sqr_length() );
}

template<class T>
double angle(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b)
{
  return vcl_acos(cos_angle(a,b));
}


template<class T>
double signed_angle(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b)
{
  return vcl_atan2(double(cross_product(a, b)), double(dot_product(a, b)));
}


template <class T>
bool orthogonal(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b, double eps)
{
  T dot = dot_product(a,b); // should be zero
  if (eps <= 0 || dot == T(0)) return dot == T(0);
  eps *= eps * a.sqr_length() * b.sqr_length();
  dot *= dot;
  return dot < eps;
}

template <class T>
bool parallel(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b, double eps)
{
  T cross = cross_product(a,b); // should be zero
  if (eps <= 0 || cross == T(0)) return cross == T(0);
  eps *= eps * a.sqr_length() * b.sqr_length();
  return cross*cross < eps;
}


template <class T>
vgl_vector_2d<T>  rotated(vgl_vector_2d<T> const& a, double angle)
{
  return vgl_vector_2d<T>( T(vcl_cos(angle)*a.x()-vcl_sin(angle)*a.y()),
                           T(vcl_sin(angle)*a.x() + vcl_cos(angle)*a.y()) );
}


//: Write "<vgl_vector_2d x,y> " to stream
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_vector_2d<T> const& p)
{
  return s << "<vgl_vector_2d "<< p.x() << ',' << p.y() <<  "> ";
}

//: Read from stream, possibly with formatting
//  Either just reads two blank-separated numbers,
//  or reads two comma-separated numbers,
//  or reads two numbers in parenthesized form "(123, 321)"
template <class T>
vcl_istream& vgl_vector_2d<T>::read(vcl_istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  T tx, ty;
  is >> vcl_ws; // jump over any leading whitespace
  if (is.eof()) return is; // nothing to be set because of EOF (TODO: should throw an exception)
  if (is.peek() == '(') { is.ignore(); paren=true; }
  is >> vcl_ws >> tx >> vcl_ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> ty >> vcl_ws;
  if (paren) {
    if (is.eof()) return is;
    if (is.peek() == ')') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
  }
  set(tx,ty);
  return is;
}

//: Read x y from stream
template <class T>
vcl_istream&  operator>>(vcl_istream& is, vgl_vector_2d<T>& p)
{
  return p.read(is);
}


#undef VGL_VECTOR_2D_INSTANTIATE
#define VGL_VECTOR_2D_INSTANTIATE(T) \
template class vgl_vector_2d<T >;\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator+    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator-    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >&     operator+=   (vgl_vector_2d<T >&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >&     operator-=   (vgl_vector_2d<T >&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator+    (vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator-    (vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator*    (double, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator*    (vgl_vector_2d<T > const&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      operator/    (vgl_vector_2d<T > const&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >&     operator*=   (vgl_vector_2d<T >&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >&     operator/=   (vgl_vector_2d<T >&, double));\
VCL_INSTANTIATE_INLINE(T      dot_product  (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(T      inner_product(vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(T      cross_product(vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(double cos_angle    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
template               double angle        (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&);\
template               double signed_angle (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&);\
template               bool   orthogonal   (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&, double);\
template               bool   parallel     (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&, double);\
VCL_INSTANTIATE_INLINE(double operator/    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >&     normalize    (vgl_vector_2d<T >&));\
VCL_INSTANTIATE_INLINE(vgl_vector_2d<T >      normalized   (vgl_vector_2d<T > const&));\
template vgl_vector_2d<T >    rotated      (vgl_vector_2d<T > const&, double);\
template        vcl_ostream&  operator<<   (vcl_ostream&, vgl_vector_2d<T >const&);\
template        vcl_istream&  operator>>   (vcl_istream&, vgl_vector_2d<T >&)

#endif // vgl_vector_2d_txx_
