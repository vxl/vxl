// This is core/vgl/vgl_vector_3d.txx
#ifndef vgl_vector_3d_txx_
#define vgl_vector_3d_txx_
//:
// \file

#include "vgl_vector_3d.h"

#include <vcl_cmath.h> // sqrt() , acos()
#include <vcl_iostream.h>

template <class T>
double vgl_vector_3d<T>::length() const
{
  return vcl_sqrt( 0.0+sqr_length() );
}

template<class T>
double angle(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b)
{
  return vcl_acos(cos_angle(a,b));
}

template <class T>
bool orthogonal(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b, double eps)
{
  T dot = dot_product(a,b); // should be zero
  if (eps <= 0 || dot == T(0)) return dot == T(0);
  eps *= eps * a.sqr_length() * b.sqr_length();
  dot *= dot;
  return dot < eps;
}

template <class T>
bool parallel(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b, double eps)
{
  double cross = cross_product(a,b).sqr_length(); // should be zero
  if (eps <= 0 || cross == 0.0) return cross == 0.0;
  eps *= eps * a.sqr_length() * b.sqr_length();
  return cross < eps;
}

//: Write "<vgl_vector_3d x,y,z> " to stream
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_vector_3d<T> const& p)
{
  return s << "<vgl_vector_3d "<< p.x() << ',' << p.y() << ',' << p.z() << "> ";
}

//: Read x y z from stream
template <class T>
vcl_istream&  operator>>(vcl_istream& s, vgl_vector_3d<T>& p)
{
  T x, y, z; s >> x >> y >> z;
  p.set(x,y,z); return s;
}


#undef VGL_VECTOR_3D_INSTANTIATE
#define VGL_VECTOR_3D_INSTANTIATE(T) \
template class vgl_vector_3d<T >;\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator+    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator-    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator+=   (vgl_vector_3d<T >&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator-=   (vgl_vector_3d<T >&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator+    (vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator-    (vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator*    (double, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator*    (vgl_vector_3d<T > const&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator/    (vgl_vector_3d<T > const&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator*=   (vgl_vector_3d<T >&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator/=   (vgl_vector_3d<T >&, double));\
VCL_INSTANTIATE_INLINE(T      dot_product  (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(T      inner_product(vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      cross_product(vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(double cos_angle    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
template               double angle        (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&);\
template               bool   orthogonal   (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&, double);\
template               bool   parallel     (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&, double);\
VCL_INSTANTIATE_INLINE(double operator/    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     normalize    (vgl_vector_3d<T >&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      normalized   (vgl_vector_3d<T > const&));\
template        vcl_ostream&  operator<<   (vcl_ostream&, vgl_vector_3d<T >const&);\
template        vcl_istream&  operator>>   (vcl_istream&, vgl_vector_3d<T >&)

#endif // vgl_vector_3d_txx_
