#include "boxm2_vecf_spline_field.h"

vgl_vector_3d<double> boxm2_vecf_spline_field::operator ()(double t) const{
  vgl_vector_3d<double> ret;
  if(generating_spline_.n_knots() < 2)
    return ret;
  assert(t>=0.0 && t<=max_t());
  unsigned im1, i0, i1, i2;
  double u;
  generating_spline_.knot_indices(t, im1, i0, i1, i2, u);
  vgl_vector_3d<double> pm1 = field_knots_[im1], p0 = field_knots_[i0];
  vgl_vector_3d<double> p1 =  field_knots_[i1],  p2 = field_knots_[i2];
  // monomials
  double u2 = u*u, u3 = u2*u;
  // coefficients
  double a0x, a1x, a2x, a3x;
  double a0y, a1y, a2y, a3y;
  double a0z, a1z, a2z, a3z;
  generating_spline_.coefficients(pm1.x(), p0.x(), p1.x(), p2.x(), a0x, a1x, a2x, a3x);
  generating_spline_.coefficients(pm1.y(), p0.y(), p1.y(), p2.y(), a0y, a1y, a2y, a3y);
  generating_spline_.coefficients(pm1.z(), p0.z(), p1.z(), p2.z(), a0z, a1z, a2z, a3z);
  // interpolate vector field on spline
  double vx = a3x*u3 + a2x*u2 + a1x*u + a0x;
  double vy = a3y*u3 + a2y*u2 + a1y*u + a0y;
  double vz = a3z*u3 + a2z*u2 + a1z*u + a0z;
  ret.set(vx, vy, vz);
  return ret;
}
vgl_cubic_spline_3d<double> boxm2_vecf_spline_field::apply_field() const{
  std::vector<vgl_point_3d<double > > knots = generating_spline_.knots();
  bool closed = generating_spline_.closed();
  double s = generating_spline_.s();
  for(unsigned i = 0; i<generating_spline_.n_knots(); ++i){
    const vgl_vector_3d<double>& v = field_knots_[i];
    knots[i].set(knots[i].x() + v.x(), knots[i].y() + v.y(), knots[i].z() + v.z());
  }
  return vgl_cubic_spline_3d<double>(knots, s, closed);
}
