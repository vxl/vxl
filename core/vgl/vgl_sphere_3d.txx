#ifndef vgl_sphere_3d_txx_
#define vgl_sphere_3d_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief a sphere in 3D nonhomogeneous space
// \author Ian Scott

#include "vgl_sphere_3d.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_line_3d_2_points.h>


//: Return true iff the point p is inside (or on) this sphere
template <class T>
bool vgl_sphere_3d<T>::contains(vgl_point_3d<T> const& p) const
{
  return r_ >= 0 && (p-c_).sqr_length() <= r_*r_;
}


//: Calculate the end points of a line clipped by this sphere.
// \return true if any of the line touches the sphere.
template <class T>
bool vgl_sphere_3d<T>::clip(const vgl_line_3d_2_points<T> & line,
                            vgl_point_3d<T> &p1, vgl_point_3d<T> &p2) const
{
  // The empty sphere does not intersect anything:
  if (r_ < 0) return false;

  vgl_point_3d<T> cp = vgl_closest_point(line, c_);

  T cp_sqr_len = (cp - c_).sqr_length();
  if (cp_sqr_len > r_*r_) return false;

  T half_chord_len = vcl_sqrt(r_*r_ - cp_sqr_len);

  vgl_vector_3d<T> linevec = line.direction();
  linevec *= half_chord_len / linevec.length();

  p1 = cp - linevec;
  p2 = cp + linevec;

  return true;
}


//: Writes "<vgl_sphere_3d centre=vgl_point_3d<x,y,z> radius=r)>" to stream
template <class T>
vcl_ostream& vgl_sphere_3d<T>::print(vcl_ostream& os) const
{
  return os << "<vgl_sphere_3d centre=" << c_
            << "radius=" << r_ << '>';
}


//: Read from stream, possibly with formatting.
//  Either just reads 4 blank-separated numbers,
//  or reads 4 comma-separated numbers,
//  or reads 4 numbers in parenthesized form "(123, 321, 567, 890)"
template <class T>
vcl_istream& vgl_sphere_3d<T>::read(vcl_istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  T cx, cy, cz, r;
  is >> vcl_ws; // jump over any leading whitespace
  if (is.eof()) return is; // nothing to be set because of EOF (TODO: should throw an exception)
  if (is.peek() == '(') { is.ignore(); paren=true; }
  is >> vcl_ws >> cx >> vcl_ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> cy >> vcl_ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> cz >> vcl_ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> r >> vcl_ws;
  if (paren) {
    if (is.eof()) return is;
    if (is.peek() == ')') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
  }
  set_centre(vgl_point_3d<T>(cx,cy,cz));
  set_radius(r);
  return is;
}


//: Writes "<vgl_sphere_3d centre=vgl_point_3d<x,y,z> radius=r)>" to stream
template <class T>
vcl_ostream& operator<<(vcl_ostream& os, const vgl_sphere_3d<T>& sph)
{
  return sph.print(os);
}


//: Read from stream, possibly with formatting.
//  Either just reads 4 blank-separated numbers,
//  or reads 4 comma-separated numbers,
//  or reads 4 numbers in parenthesized form "(123, 321, 567, 890)"
template <class T>
vcl_istream& operator>>(vcl_istream& is, vgl_sphere_3d<T>& sph)
{
  return sph.read(is);
}


#undef VGL_SPHERE_3D_INSTANTIATE
#define VGL_SPHERE_3D_INSTANTIATE(T) \
template class vgl_sphere_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_sphere_3d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_sphere_3d<T >&)


#endif // vgl_sphere_3d_txx_
