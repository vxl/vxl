// This is core/vgl/vgl_point_3d.hxx
#ifndef vgl_point_3d_hxx_
#define vgl_point_3d_hxx_
//:
// \file

#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include "vgl_point_3d.h"
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_tolerance.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Construct from homogeneous point
template <class Type>
vgl_point_3d<Type>::vgl_point_3d(vgl_homg_point_3d<Type> const& p)
  : x_(p.x()/p.w()), y_(p.y()/p.w()), z_(p.z()/p.w()) // could be infinite!
{
}

//: Construct from 3 planes (intersection).
template <class Type>
vgl_point_3d<Type>::vgl_point_3d(vgl_plane_3d<Type> const& pl1,
                                 vgl_plane_3d<Type> const& pl2,
                                 vgl_plane_3d<Type> const& pl3)
{
  vgl_homg_plane_3d<Type> h1(pl1.nx(), pl1.ny(), pl1.nz(), pl1.d());
  vgl_homg_plane_3d<Type> h2(pl2.nx(), pl2.ny(), pl2.nz(), pl2.d());
  vgl_homg_plane_3d<Type> h3(pl3.nx(), pl3.ny(), pl3.nz(), pl3.d());
  vgl_homg_point_3d<Type> p(h1, h2, h3); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()); // could be infinite!
}

template <class Type>
bool vgl_point_3d<Type>::operator==(const vgl_point_3d<Type> &p) const
{
  return this==&p || (x_>=p.x()-vgl_tolerance<Type>::position && x_<=p.x()+vgl_tolerance<Type>::position &&
                      y_>=p.y()-vgl_tolerance<Type>::position && y_<=p.y()+vgl_tolerance<Type>::position &&
                      z_>=p.z()-vgl_tolerance<Type>::position && z_<=p.z()+vgl_tolerance<Type>::position );
}

template <class Type>
bool coplanar(vgl_point_3d<Type> const& p1,
              vgl_point_3d<Type> const& p2,
              vgl_point_3d<Type> const& p3,
              vgl_point_3d<Type> const& p4)
{
  Type r = ( (p1.x()*p2.y()-p1.y()*p2.x())*p3.z()
            +(p3.x()*p1.y()-p3.y()*p1.x())*p2.z()
            +(p2.x()*p3.y()-p2.y()*p3.x())*p1.z()
            +(p1.x()*p4.y()-p1.y()*p4.x())*p2.z()
            +(p4.x()*p2.y()-p4.y()*p2.x())*p1.z()
            +(p2.x()*p1.y()-p2.y()*p1.x())*p4.z()
            +(p3.x()*p4.y()-p3.y()*p4.x())*p1.z()
            +(p1.x()*p3.y()-p1.y()*p3.x())*p4.z()
            +(p4.x()*p1.y()-p4.y()*p1.x())*p3.z()
            +(p3.x()*p2.y()-p3.y()*p2.x())*p4.z()
            +(p2.x()*p4.y()-p2.y()*p4.x())*p3.z()
            +(p4.x()*p3.y()-p4.y()*p3.x())*p2.z() );
  return r <= vgl_tolerance<Type>::point_3d_coplanarity && r >= -vgl_tolerance<Type>::point_3d_coplanarity;
}

template <class T>
double cross_ratio(vgl_point_3d<T>const& p1, vgl_point_3d<T>const& p2,
                   vgl_point_3d<T>const& p3, vgl_point_3d<T>const& p4)
{
  // least squares solution: (Num_x-CR*Den_x)^2 + (Num_y-CR*Den_y)^2 + (Num_z-CR*Den_z)^2 minimal.
  double Num_x = (p1.x()-p3.x())*(p2.x()-p4.x());
  double Num_y = (p1.y()-p3.y())*(p2.y()-p4.y());
  double Num_z = (p1.z()-p3.z())*(p2.z()-p4.z());
  double Den_x = (p1.x()-p4.x())*(p2.x()-p3.x());
  double Den_y = (p1.y()-p4.y())*(p2.y()-p3.y());
  double Den_z = (p1.z()-p4.z())*(p2.z()-p3.z());
  if (Den_x == Den_y && Den_y == Den_z) return (Num_x+Num_y+Num_z)/3/Den_x;
  else return (Den_x*Num_x+Den_y*Num_y+Den_z*Num_z)/(Den_x*Den_x+Den_y*Den_y+Den_z*Den_z);
}

//: Write "<vgl_point_3d x,y,z> " to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, vgl_point_3d<Type> const& p)
{
  return s << "<vgl_point_3d "<< p.x() << ',' << p.y() << ',' << p.z() << "> ";
}

//: Read from stream, possibly with formatting
//  Either just reads three blank-separated numbers,
//  or reads three comma-separated numbers,
//  or reads three numbers in parenthesized form "(123, 321, 567)"
// \relatesalso vgl_point_3d
template <class Type>
std::istream& vgl_point_3d<Type>::read(std::istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  Type tx, ty, tz;
  is >> std::ws; // jump over any leading whitespace
  char c;
  c=is.peek();
  if(c == '<'){
          std::string temp;
          is >> temp;
  }
  if (is.eof()) return is; // nothing to be set because of EOF (TODO: should throw an exception)
  if (is.peek() == '(') { is.ignore(); paren=true; }
  is >> std::ws >> tx >> std::ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> std::ws >> ty >> std::ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  is >> std::ws >> tz >> std::ws;
  if (paren) {
    if (is.eof()) return is;
    if (is.peek() == ')') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
  }
  is >> std::ws;
  if (is.peek() == '>') is.ignore();
  set(tx,ty,tz);
  return is;
}

//: Read from stream, possibly with formatting
//  Either just reads three blank-separated numbers,
//  or reads three comma-separated numbers,
//  or reads three numbers in parenthesized form "(123, 321, 567)"
// \relatesalso vgl_point_3d
template <class Type>
std::istream&  operator>>(std::istream& is, vgl_point_3d<Type>& p)
{
  return p.read(is);
}

//: Return the "average deviation" of a set of given points from its centre of gravity.
//  "Average" in the sense of the standard deviation (2-norm, i.e., square root
//  of sum of squares) of the distances from that centre of gravity.
// \relatesalso vgl_point_3d
template <class Type>
double stddev(std::vector<vgl_point_3d<Type> > const& v)
{
  int n = (int)(v.size());
  double d = 0.0;
  if (n<=1) return d;
  vgl_point_3d<Type> c = centre(v);
  Type cx = c.x(), cy = c.y(), cz = c.z();
#define vgl_sqr(x) double((x)*(x))
  for (int i=0; i<n; ++i)
    d += vgl_sqr(v[i].x()-cx) + vgl_sqr(v[i].y()-cy) + vgl_sqr(v[i].z()-cz);
#undef vgl_sqr
  return std::sqrt(d);
}


#undef VGL_POINT_3D_INSTANTIATE
#define VGL_POINT_3D_INSTANTIATE(T) \
template class vgl_point_3d<T >; \
template double cross_ratio(vgl_point_3d<T >const&, vgl_point_3d<T >const&, \
                            vgl_point_3d<T >const&, vgl_point_3d<T >const&); \
template bool coplanar(vgl_point_3d<T > const&, vgl_point_3d<T > const&, \
                       vgl_point_3d<T > const&, vgl_point_3d<T > const&); \
template std::ostream& operator<<(std::ostream&, const vgl_point_3d<T >&); \
template std::istream& operator>>(std::istream&, vgl_point_3d<T >&); \
template double stddev(std::vector<vgl_point_3d<T > > const&)

#endif // vgl_point_3d_hxx_
