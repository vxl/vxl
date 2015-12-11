// This is core/vgl/vgl_infinite_line_3d.txx
#ifndef vgl_infinite_line_3d_txx_
#define vgl_infinite_line_3d_txx_

#include "vgl_infinite_line_3d.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for fabs
#include "vgl_tolerance.h"
#include "vgl_vector_3d.h"

template <class Type>
vgl_infinite_line_3d<Type>::vgl_infinite_line_3d(vgl_point_3d<Type> const& p1,
                                                 vgl_point_3d<Type> const& p2)
{
  vgl_vector_3d<Type> dir = p2-p1;
  vgl_infinite_line_3d<Type> l(p1, dir);
  x0_ = l.x0();
  t_ = dir;
}

template <class Type>
void vgl_infinite_line_3d<Type>::
compute_uv_vectors(vgl_vector_3d<Type>& u, vgl_vector_3d<Type>& v) const
{
  // start with u0 and v0 as the x and y axes, assuming direction z
  vgl_vector_3d<Type> u0(Type(1),Type(0),Type(0));
  vgl_vector_3d<Type> v0(Type(0),Type(1),Type(0));
  vgl_vector_3d<Type> t0(Type(0),Type(0),Type(1));

  // rotate u0 and v0 such that t0 aligns with t_
  vgl_vector_3d<Type> tu = normalized(t_);
  vgl_vector_3d<Type> rot_axis = cross_product(t0, tu);
  double rot_mag = static_cast<double>(rot_axis.length());
  rot_axis /= rot_mag;
  const double pi = 3.14159265359;
  double aa = 0.0; if (dot_product(t0, tu) < 0) { aa = pi; rot_axis=-rot_axis; }
  // cross product of unit vectors is at most a unit vector:
  if (rot_mag>1.0) rot_mag=1.0;
  // if the vectors have the same direction, then set to identity rotation:
  if (rot_mag < vgl_tolerance<double>::position)
  {
    // rotation magnitude is nearly 0
    if (aa!=pi) {
      // identity: no rotation of u0 and v0 needed.
      u = u0;
      v = v0;
      return;
    }
    else { 
      // 180 degree rotation: arbitrarily choose to rotate around the x axis.
      u = u0;
      v = -v0;
      return;
    }
  }
  // perform the rotations of u0 and v0 based on the quaternion formulation.
  // There is some code duplication here from vnl_quaternion in order to avoid a vnl dependency.
  double half_angle = 0.5*(vcl_asin(rot_mag)+aa);
  Type s = Type(vcl_sin(half_angle));
  vgl_vector_3d<Type> qim = s * rot_axis; // the imaginary component of the quaternion
  Type qreal = vcl_cos(half_angle);  // the real component of the quaternion
  // rotate u0 using quaternion rotation formula
  vgl_vector_3d<Type> u0_i_x_v = cross_product(qim, u0);
  u = u0 + u0_i_x_v * Type(2*qreal) - cross_product(u0_i_x_v, qim) * Type(2);
  // rotate v0 using quaternion rotation formula
  vgl_vector_3d<Type> v0_i_x_v = cross_product(qim, v0);
  v = v0 + v0_i_x_v * Type(2*qreal) - cross_product(v0_i_x_v, qim) * Type(2);
}

template <class Type>
vgl_infinite_line_3d<Type>::
vgl_infinite_line_3d(vgl_point_3d<Type> const& p,
                     vgl_vector_3d<Type> const& dir)
{
  // reconcile direction so that tangent is in the positive hemisphere
  double ttx = vcl_fabs(static_cast<double>(dir.x()));
  double tty = vcl_fabs(static_cast<double>(dir.y()));
  double ttz = vcl_fabs(static_cast<double>(dir.z()));
  double max_comp = ttx;
  double sign = static_cast<double>(dir.x());
  if (max_comp < tty) {
    max_comp = tty;
    sign = static_cast<double>(dir.y());
  }
  if (max_comp < ttz) {
    max_comp = ttz;
    sign = static_cast<double>(dir.z());
  }
  // switch sense if max component is negative
  Type sense = static_cast<Type>(sign/max_comp);
  t_ = normalized(dir*sense);
  // Define the plane perpendicular to the line passing through the origin
  // the plane normal is t_ the distance of the plane from the origin is 0
  // it follows that the intersection of the line with the perpendicular plane
  // is as follows:
  Type mag = static_cast<Type>(t_.length());
  assert(mag>Type(0));
  vgl_vector_3d<Type> pv(p.x(), p.y(), p.z());
  Type dp = dot_product(pv, t_);
  Type k = -dp/(mag*mag);
  // The intersection point
  vgl_vector_3d<Type> p0 = pv + k*t_, u, v;
  this->compute_uv_vectors(u, v);
  // The location of the intersection point in plane coordinates can now be computed
  Type u0 = dot_product(u, p0), v0 = dot_product(v, p0);
  x0_.set(u0, v0);
}

// the point on the line closest to the origin
template <class Type>
vgl_point_3d<Type> vgl_infinite_line_3d<Type>::point() const
{
  // u,v plane coordinate vectors
  vgl_vector_3d<Type> u, v, pv;
  this->compute_uv_vectors(u, v);
  pv = x0_.x()*u + x0_.y()*v;
  return vgl_point_3d<Type>(pv.x(), pv.y(), pv.z());
}

template <class Type>
bool vgl_infinite_line_3d<Type>::contains(const vgl_point_3d<Type>& p ) const
{
  vgl_point_3d<Type> point1 = this->point();
  vgl_point_3d<Type> point2 = this->point_t(Type(1));
  double seg = 1.0;
  double len1 = static_cast<double>((point1 - p).length());
  double len2 = static_cast<double>((point2 - p).length());
  // two cases: point inside (point1, point2) segment;
  //            point outside (point1, point2) segment
  double r = seg -(len1 + len2);
  if (len1>seg||len2>seg)
    r = seg - vcl_fabs(len1-len2);
  return r < 1e-8 && r > -1e-8;
}

// stream operators
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_infinite_line_3d<Type> const & p)
{
  return s << "<vgl_infinite_line_3d: origin" << p.x0() << " dir " << p.direction() << " >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_infinite_line_3d<Type>& p)
{
  vgl_vector_2d<Type> x_0;
  vgl_vector_3d<Type> dir;
  s >> x_0 >> dir;
  p.set(x_0, dir);
  return s;
}

#undef VGL_INFINITE_LINE_3D_INSTANTIATE
#define VGL_INFINITE_LINE_3D_INSTANTIATE(Type) \
template class vgl_infinite_line_3d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_infinite_line_3d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_infinite_line_3d<Type > const&)

#endif // vgl_infinite_line_3d_txx_
