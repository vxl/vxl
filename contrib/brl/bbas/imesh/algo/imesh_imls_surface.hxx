// This is brl/bbas/imesh/algo/imesh_imls_surface.hxx
#ifndef imesh_imls_surface_hxx_
#define imesh_imls_surface_hxx_
#include "imesh_imls_surface.h"
//:
// \file

#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_intersect.h>
#include <imesh/algo/imesh_kd_tree.hxx>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: area integral of the squared weight function times a linearly interpolated value
//  \a eps2 is epsilon^2
template <class T, class F>
T
imesh_imls_surface::triangle_quadrature(F quad_func,
                                        const vgl_point_3d<double>& x,
                                        const vgl_point_3d<double>& p0,
                                        const vgl_point_3d<double>& p1,
                                        const vgl_point_3d<double>& p2,
                                        const vgl_vector_3d<double>& n,
                                        double v0, double v1, double v2,
                                        double eps)
{
  double dist,u,v;
  unsigned char flag = imesh_triangle_closest_point(x,p0,p1,p2,n,dist,u,v);
  switch (flag)
  {
    case 1:
      return quad_func(x,p0,p1,p2,v0,v1,v2,eps);
    case 2:
      return quad_func(x,p1,p2,p0,v1,v2,v0,eps);
    case 4:
      return quad_func(x,p2,p0,p1,v2,v0,v1,eps);

    case 3:
    {
      double t = 1.0-u;
      vgl_point_3d<double> pi(t*p0.x()+u*p1.x(), t*p0.y()+u*p1.y(), t*p0.z()+u*p1.z());
      double vi = t*v0 + u*v1;
      return quad_func(x,pi,p2,p0,vi,v2,v0,eps)
           + quad_func(x,pi,p1,p2,vi,v1,v2,eps);
    }

    case 6:
    {
      vgl_point_3d<double> pi(u*p1.x()+v*p2.x(), u*p1.y()+v*p2.y(), u*p1.z()+v*p2.z());
      double vi = u*v1 + v*v2;
      return quad_func(x,pi,p0,p1,vi,v0,v1,eps)
           + quad_func(x,pi,p2,p0,vi,v2,v0,eps);
    }

    case 5:
    {
      double t = 1.0-v;
      vgl_point_3d<double> pi(t*p0.x()+v*p2.x(), t*p0.y()+v*p2.y(), t*p0.z()+v*p2.z());
      double vi = t*v0 + v*v2;
      return quad_func(x,pi,p0,p1,vi,v0,v1,eps)
           + quad_func(x,pi,p1,p2,vi,v1,v2,eps);
    }

    case 7:
    {
      double t = 1.0-u-v;
      vgl_point_3d<double> pi(t*p0.x()+u*p1.x()+v*p2.x(),
                              t*p0.y()+u*p1.y()+v*p2.y(),
                              t*p0.z()+u*p1.z()+v*p2.z());
      double vi = t*v0 + u*v1 + v*v2;
      return quad_func(x,pi,p0,p1,vi,v0,v1,eps)
           + quad_func(x,pi,p1,p2,vi,v1,v2,eps)
           + quad_func(x,pi,p2,p0,vi,v2,v0,eps);
    }
    default: // should never be reached
      assert(!"invalid flag");
  }
  return T();
}

#define IMESH_IMLS_SURFACE_INSTANTATE(T1,T2) \
template T1 imesh_imls_surface::triangle_quadrature(T1(*)(T2 const&, T2 const&, T2 const&, T2 const&, double, double, double, double), \
                                                    const vgl_point_3d<double>& x, \
                                                    const vgl_point_3d<double>& p0, \
                                                    const vgl_point_3d<double>& p1, \
                                                    const vgl_point_3d<double>& p2, \
                                                    const vgl_vector_3d<double>& n, \
                                                    double v0, double v1, double v2, \
                                                    double eps)

#endif // imesh_imls_surface_hxx_
