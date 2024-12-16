// This is core/vgl/vgl_homg_point_2d.hxx
#ifndef vgl_homg_point_2d_hxx_
#define vgl_homg_point_2d_hxx_

#include <iostream>
#include "vgl_homg_point_2d.h"
#include "vgl_homg_line_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Note that the given lines must be distinct!
template <class Type>
vgl_homg_point_2d<Type>::vgl_homg_point_2d(const vgl_homg_line_2d<Type> & l1, const vgl_homg_line_2d<Type> & l2)
{
  set(l1.b() * l2.c() - l1.c() * l2.b(), l1.c() * l2.a() - l1.a() * l2.c(), l1.a() * l2.b() - l1.b() * l2.a());
}

template <class T>
double
cross_ratio(const vgl_homg_point_2d<T> & p1,
            const vgl_homg_point_2d<T> & p2,
            const vgl_homg_point_2d<T> & p3,
            const vgl_homg_point_2d<T> & p4)
{
  // least squares solution: (Num_x-CR*Den_x)^2 + (Num_y-CR*Den_y)^2 minimal.
  double Num_x = (p1.x() * p3.w() - p3.x() * p1.w()) * (p2.x() * p4.w() - p4.x() * p2.w());
  double Num_y = (p1.y() * p3.w() - p3.y() * p1.w()) * (p2.y() * p4.w() - p4.y() * p2.w());
  double Den_x = (p1.x() * p4.w() - p4.x() * p1.w()) * (p2.x() * p3.w() - p3.x() * p2.w());
  double Den_y = (p1.y() * p4.w() - p4.y() * p1.w()) * (p2.y() * p3.w() - p3.y() * p2.w());
  if (Den_x == Den_y)
    return 0.5 * (Num_x + Num_y) / Den_x;
  else
    return (Den_x * Num_x + Den_y * Num_y) / (Den_x * Den_x + Den_y * Den_y);
}

template <class Type>
std::ostream &
operator<<(std::ostream & s, const vgl_homg_point_2d<Type> & p)
{
  return s << " <vgl_homg_point_2d (" << p.x() << ',' << p.y() << ',' << p.w() << ") >";
}

template <class Type>
std::istream &
operator>>(std::istream & s, vgl_homg_point_2d<Type> & p)
{
  Type x, y, w;
  s >> x >> y >> w;
  p.set(x, y, w);
  return s;
}

#undef VGL_HOMG_POINT_2D_INSTANTIATE
#define VGL_HOMG_POINT_2D_INSTANTIATE(T)                                            \
  template class vgl_homg_point_2d<T>;                                              \
  template double cross_ratio(vgl_homg_point_2d<T> const &,                         \
                              vgl_homg_point_2d<T> const &,                         \
                              vgl_homg_point_2d<T> const &,                         \
                              vgl_homg_point_2d<T> const &);                        \
  template std::ostream & operator<<(std::ostream &, vgl_homg_point_2d<T> const &); \
  template std::istream & operator>>(std::istream &, vgl_homg_point_2d<T> &)

#endif // vgl_homg_point_2d_hxx_
