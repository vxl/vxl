// This is core/vgl/vgl_point_2d.hxx
#ifndef vgl_point_2d_hxx_
#define vgl_point_2d_hxx_
//:
// \file

#include <iostream>
#include <iomanip>
#include <string>
#include "vgl_point_2d.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_2d.h>

#include <vcl_compiler.h>

//: Construct from homogeneous point
template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_homg_point_2d<Type> const& p)
  : x_(p.x()/p.w()), y_(p.y()/p.w()) // could be infinite!
{
}

//: Construct from 2 lines (intersection).
template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_line_2d<Type> const& l1,
                                 vgl_line_2d<Type> const& l2)
{
  vgl_homg_line_2d<Type> h1(l1.a(), l1.b(), l1.c());
  vgl_homg_line_2d<Type> h2(l2.a(), l2.b(), l2.c());
  vgl_homg_point_2d<Type> p(h1, h2); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w()); // could be infinite!
}

template <class T>
double cross_ratio(vgl_point_2d<T>const& p1, vgl_point_2d<T>const& p2,
                   vgl_point_2d<T>const& p3, vgl_point_2d<T>const& p4)
{
  // least squares solution: (Num_x-CR*Den_x)^2 + (Num_y-CR*Den_y)^2 minimal.
  double Num_x = (p1.x()-p3.x())*(p2.x()-p4.x());
  double Num_y = (p1.y()-p3.y())*(p2.y()-p4.y());
  double Den_x = (p1.x()-p4.x())*(p2.x()-p3.x());
  double Den_y = (p1.y()-p4.y())*(p2.y()-p3.y());
  if (Den_x == Den_y) return 0.5*(Num_x+Num_y)/Den_x;
  else return (Den_x*Num_x+Den_y*Num_y)/(Den_x*Den_x+Den_y*Den_y);
}

//: Write "<vgl_point_2d x,y> " to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, vgl_point_2d<Type> const& p)
{
  return s << "<vgl_point_2d "<< p.x() << ',' << p.y() << " > ";
}

//: Read from stream, possibly with formatting
//  Either just reads two blank-separated numbers,
//  or reads two comma-separated numbers,
//  or reads two numbers in parenthesized form "(123, 321)"
template <class Type>
std::istream& vgl_point_2d<Type>::read(std::istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  bool angle = false;
  Type tx, ty;
  is >> std::ws; // jump over any leading whitespace
  char c;
  c = is.peek();
  if(c == '<')
  {
    is.ignore();
    angle = true;
  }

  if (is.eof())
    return is; // nothing to be set because of EOF (TODO: should throw an exception)

  if (is.peek() == '(')
  {
    is.ignore();
    paren=true;
  }

  is >> std::ws >> tx >> std::ws;
  if (is.eof())
    return is;

  if (is.peek() == ',')
    is.ignore();

  is >> std::ws >> ty >> std::ws;
  if (paren)
  {
    if (is.eof())
      return is;
    if (is.peek() == ')')
      is.ignore();
    else
      return is; // closing parenthesis is missing (TODO: throw an exception)
  }

  is >> std::ws;

  if (angle)
  {
    if (is.eof())
      return is;
    if (is.peek() == '>')
      is.ignore();
    else
      return is; // closing parenthesis is missing (TODO: throw an exception)
  }

  set(tx,ty);
  return is;
}

//: Read x y from stream
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_point_2d<Type>& p)
{
  return p.read(is);
}

#undef VGL_POINT_2D_INSTANTIATE
#define VGL_POINT_2D_INSTANTIATE(T) \
template class vgl_point_2d<T >; \
template double cross_ratio(vgl_point_2d<T >const&, vgl_point_2d<T >const&, \
                            vgl_point_2d<T >const&, vgl_point_2d<T >const&); \
template std::ostream& operator<<(std::ostream&, const vgl_point_2d<T >&); \
template std::istream& operator>>(std::istream&, vgl_point_2d<T >&)

#endif // vgl_point_2d_hxx_
