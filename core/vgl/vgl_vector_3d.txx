// This is vxl/vgl/vgl_vector_3d.txx
#ifndef vgl_vector_3d_txx_
#define vgl_vector_3d_txx_

#include "vgl_vector_3d.h"

#include <vcl_cstdlib.h> // abort()
#include <vcl_cmath.h> // sqrt() , acos()
#include <vcl_ostream.h>

template <class T>
double vgl_vector_3d<T>::length() const {
  return vcl_sqrt( 0.0+x()*x()+y()*y()+z()*z() );
}

template<class T>
double angle(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b) {
  return vcl_acos(cos_angle(a,b));
}

template <class T>
bool parallel(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b, double eps)
{
  double cross = cross_product(a,b).length(); // should be zero
  if (eps <= 0 || cross == 0.0) return cross == 0.0;
  // Since cross != 0, a and b cannot have zero length:
  double dev = cross / a.length() / a.length();
  return (dev < eps && -dev < eps);
}


//: Write "<vgl_vector_3d x,y,z> " to stream
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_vector_3d<T>& p) {
  return s << "<"<< p.x() << "," << p.y() << "," << p.z() << "> ";
}


#undef VGL_VECTOR_3D_INSTANTIATE
#define v vgl_vector_3d
#define VGL_VECTOR_3D_INSTANTIATE(T) \
template class vgl_vector_3d<T >;\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator+    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator-    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator+=   (vgl_vector_3d<T >&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator-=   (vgl_vector_3d<T >&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator+    (vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator-    (vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator*    (T, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator*    (vgl_vector_3d<T > const&, T));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      operator/    (vgl_vector_3d<T > const&, double));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator*=   (vgl_vector_3d<T >&, T));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     operator/=   (vgl_vector_3d<T >&, double));\
VCL_INSTANTIATE_INLINE(T      dot_product  (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(T      inner_product(vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      cross_product(vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(double cos_angle    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
template               double angle        (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&);\
template               bool   parallel     (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&, double);\
template vcl_ostream& operator<<(vcl_ostream&, const vgl_vector_3d<T >&); \
VCL_INSTANTIATE_INLINE(double operator/    (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >&     normalize    (vgl_vector_3d<T >&));\
VCL_INSTANTIATE_INLINE(vgl_vector_3d<T >      normalized   (vgl_vector_3d<T > const&))
#undef v

#endif // vgl_vector_3d_txx_
