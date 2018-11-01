// This is core/vgl/vgl_vector_3d.hxx
#ifndef vgl_vector_3d_hxx_
#define vgl_vector_3d_hxx_
//:
// \file

#include <cmath>
#include <iostream>
#include <string>
#include "vgl_vector_3d.h"
#include "vgl_tolerance.h"

#include <vcl_compiler_detection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vcl_deprecated.h>

template <class T>
double vgl_vector_3d<T>::length() const
{
  return std::sqrt( 0.0+sqr_length() );
}

//: The one-parameter family of unit vectors that are orthogonal to *this, v(s).
// The parameterization is such that 0<=s<1, v(0)==v(1)
template <class T>
vgl_vector_3d<T> vgl_vector_3d<T>::orthogonal_vectors(double s) const
{
  VXL_DEPRECATED_MACRO("vgl_vector_3d<T>::orthogonal_vectors(double s)");
  return ::orthogonal_vectors(*this, s);
}

template<class T>
double angle(vgl_vector_3d<T> const& a, vgl_vector_3d<T> const& b)
{
  double ca = cos_angle(a,b);
  // Deal with numerical errors giving values slightly outside range.
  if (ca>=-1.0)
  {
    if (ca<=1.0)
      return std::acos(ca);
    else
      return 0;
  }
  else
    return std::acos(-1.0); // pi
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

template <class T>
vgl_vector_3d<T> orthogonal_vectors(vgl_vector_3d<T> const& a, double s)
{
  assert(a.length()>0.0);

  // enforce parameter range
  if (s<0.0) s=0.0;
  if (s>1.0) s = 1.0;
  double tol = static_cast<double>(vgl_tolerance<T>::position);
  double nx = static_cast<double>(a.x_);
  double ny = static_cast<double>(a.y_);
  double nz = static_cast<double>(a.z_);
  double two_pi = 2*std::acos(-1.0);
  double co = std::cos(two_pi*s);
  double si = std::sin(two_pi*s);

  double mnz = std::fabs(nz);
  if (mnz>tol)  // General case
  {
    double rx = nx/nz;
    double ry = ny/nz;
    double q = co*rx +si*ry;
    double a = 1.0/std::sqrt(1+q*q);
    T vx = static_cast<T>(a*co);
    T vy = static_cast<T>(a*si);
    T vz = -static_cast<T>(rx*vx + ry*vy);
    return vgl_vector_3d<T>(vx, vy, vz);
  }
  else  // Special cases, nz ~ 0
  {
    double mny = std::fabs(ny);
    if (mny>tol)
    {
      double r = nx/ny;
      double a = 1/std::sqrt(1+co*co*r*r);
      T vx = static_cast<T>(a*co);
      T vz = static_cast<T>(a*si);
      T vy = -static_cast<T>(a*co*r);
      return vgl_vector_3d<T>(vx, vy, vz);
    }
    else
    {
      // assume mnx>tol provided that input vector was not null
      double r = ny/nx;
      double a = 1/std::sqrt(1+co*co*r*r);
      T vy = static_cast<T>(a*co);
      T vz = static_cast<T>(a*si);
      T vx = -static_cast<T>(a*co*r);
      return vgl_vector_3d<T>(vx, vy, vz);
    }
  }
}

//: Write "<vgl_vector_3d x,y,z> " to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, vgl_vector_3d<T> const& p)
{
  return s << "<vgl_vector_3d "<< p.x() << ',' << p.y() << ',' << p.z() << "> ";
}

//: Read from stream, possibly with formatting
//  Either just reads three blank-separated numbers,
//  or reads three comma-separated numbers,
//  or reads three numbers in parenthesized form "(123, 321, 567)"
// \relatesalso vgl_vector_3d
template <class T>
std::istream& vgl_vector_3d<T>::read(std::istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  T tx, ty, tz;
  is >> std::ws; // jump over any leading whitespace
  if (is.eof()) return is; // nothing to be set because of EOF (TODO: should throw an exception)
  char c = is.peek();
  if (c == '(') { is.ignore(); paren=true; }
  if(paren){
    is >> std::ws >> tx >> std::ws;
    if (is.eof()) return is;
    if (is.peek() == ',') is.ignore();
    is >> std::ws >> ty >> std::ws;
    if (is.eof()) return is;
    if (is.peek() == ',') is.ignore();
    is >> std::ws >> tz >> std::ws;
    if (is.eof()) return is;
    if (is.peek() == ')') is.ignore();
    else return is; // closing parenthesis is missing (TODO: throw an exception)
  }else if( c == '<'){
    std::string temp;
    is >> temp >> std::ws; // read <vgl_vector_3d
    is >> tx >>  std::ws;
    c = is.peek();
    if(c != ','){
      std::cout << "Invalid syntax: >> vgl_vector_3d" << std::endl;
      set(0.0, 0.0, 0.0);
      return is;
    }else is.ignore();
    is >> ty >> std::ws;
    c = is.peek();
    if(c != ','){
      std::cout << "Invalid syntax: >> vgl_vector_3d" << std::endl;
      set(0.0, 0.0, 0.0);
      return is;
    }else is.ignore();
    is >> tz >> std::ws;
    if(is.peek() != '>'){
      std::cout << "Invalid syntax: >> vgl_vector_3d" << std::endl;
      set(0.0, 0.0,0.0);
      return is;
    }else is.ignore();
  }else{
          is >> tx >> std::ws;
        c = is.peek();
        if(c == ',') is.ignore();
        is >> std::ws >> ty >> std::ws;
        c = is.peek();
        if(c == ',') is.ignore();
    is >> std::ws >> tz >> std::ws;
  }
  set(tx,ty,tz);
  return is;
}

//: Read from stream, possibly with formatting
//  Either just reads three blank-separated numbers,
//  or reads three comma-separated numbers,
//  or reads three numbers in parenthesized form "(123, 321, 567)"
// \relatesalso vgl_vector_3d
template <class T>
std::istream&  operator>>(std::istream& is, vgl_vector_3d<T>& p)
{
  return p.read(is);
}

#undef VGL_VECTOR_3D_INSTANTIATE
#define VGL_VECTOR_3D_INSTANTIATE(T) \
template class vgl_vector_3d<T >;\
/*template vgl_vector_3d<T >  operator+         (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  operator-         (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >& operator+=        (vgl_vector_3d<T >&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >& operator-=        (vgl_vector_3d<T >&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  operator+         (vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  operator-         (vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  operator*         (double, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  operator*         (vgl_vector_3d<T > const&, double); */\
/*template vgl_vector_3d<T >  operator/         (vgl_vector_3d<T > const&, double); */\
/*template vgl_vector_3d<T >& operator*=        (vgl_vector_3d<T >&, double); */\
/*template vgl_vector_3d<T >& operator/=        (vgl_vector_3d<T >&, double); */\
/*template T                  dot_product       (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template T                  inner_product     (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >  cross_product     (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template double             cos_angle         (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
template               double             angle             (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&);\
template               bool               orthogonal        (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&, double);\
template               bool               parallel          (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&, double);\
template               vgl_vector_3d<T >  orthogonal_vectors(vgl_vector_3d<T > const&, double);\
/*template double             operator/         (vgl_vector_3d<T > const&, vgl_vector_3d<T > const&); */\
/*template vgl_vector_3d<T >& normalize         (vgl_vector_3d<T >&); */\
/*template vgl_vector_3d<T >  normalized        (vgl_vector_3d<T > const&); */\
template               std::ostream&       operator<<        (std::ostream&, vgl_vector_3d<T >const&);\
template               std::istream&       operator>>        (std::istream&, vgl_vector_3d<T >&)

#endif // vgl_vector_3d_hxx_
