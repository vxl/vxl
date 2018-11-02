// This is core/vgl/vgl_vector_2d.hxx
#ifndef vgl_vector_2d_hxx_
#define vgl_vector_2d_hxx_
//:
// \file

#include <cmath>
#include <iostream>
#include <string>
#include "vgl_vector_2d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
double vgl_vector_2d<T>::length() const
{
  return std::sqrt( 0.0+sqr_length() );
}

template<class T>
double angle(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b)
{
  return std::acos(cos_angle(a,b));
}


template<class T>
double signed_angle(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b)
{
  return std::atan2(double(cross_product(a, b)), double(dot_product(a, b)));
}


template <class T>
bool orthogonal(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b, double eps)
{
  T dot = dot_product(a,b); // should be zero
  if (eps <= 0 || dot == T(0)) return dot == T(0);
  eps *= eps * a.sqr_length() * b.sqr_length();
  dot *= dot;
  return dot < eps;
}

template <class T>
bool parallel(vgl_vector_2d<T> const& a, vgl_vector_2d<T> const& b, double eps)
{
  T cross = cross_product(a,b); // should be zero
  if (eps <= 0 || cross == T(0)) return cross == T(0);
  eps *= eps * a.sqr_length() * b.sqr_length();
  return cross*cross < eps;
}


template <class T>
vgl_vector_2d<T>  rotated(vgl_vector_2d<T> const& a, double angle)
{
  return vgl_vector_2d<T>( T(std::cos(angle)*a.x()-std::sin(angle)*a.y()),
                           T(std::sin(angle)*a.x() + std::cos(angle)*a.y()) );
}


//: Write "<vgl_vector_2d x,y> " to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, vgl_vector_2d<T> const& p)
{
  return s << "<vgl_vector_2d "<< p.x() << ',' << p.y() <<  "> ";
}

//: Read from stream, possibly with formatting
//  Either just reads two blank-separated numbers,
//  or reads two comma-separated numbers,
//  or reads two numbers in parenthesized form "(123, 321)"
//  Also can read the form "<vgl_vector_2d x,y>"
template <class T>
std::istream& vgl_vector_2d<T>::read(std::istream& is)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  T tx, ty;
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
    if (is.peek() == ')') is.ignore();
    else  return is; // closing parenthesis is missing (TODO: throw an exception)
  }else if(c == '<'){
    std::string temp;
    is >> temp >> std::ws; // read <vgl_vector_2d
    is >> tx >>  std::ws;
        c = is.peek();
        if(c != ','){
                std::cout << "Invalid syntax: >> vgl_vector_2d" << std::endl;
      set(0.0, 0.0);
      return is;
    }else is.ignore();
        is >> ty>>std::ws;
    if(is.peek() != '>'){
      std::cout << "Invalid syntax: >> vgl_vector_2d" << std::endl;
      set(0.0, 0.0);
      return is;
    }else is.ignore();
  }else{
    is >> tx >> std::ws;
        c = is.peek();
        if(c == ',') is.ignore();
        is >> std::ws >> ty;
  }
  set(tx,ty);
  return is;
}

//: Read x y from stream
template <class T>
std::istream&  operator>>(std::istream& is, vgl_vector_2d<T>& p)
{
  return p.read(is);
}


#undef VGL_VECTOR_2D_INSTANTIATE
#define VGL_VECTOR_2D_INSTANTIATE(T) \
template class vgl_vector_2d<T >;\
/*template vgl_vector_2d<T >      operator+    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >      operator-    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >&     operator+=   (vgl_vector_2d<T >&, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >&     operator-=   (vgl_vector_2d<T >&, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >      operator+    (vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >      operator-    (vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >      operator*    (double, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >      operator*    (vgl_vector_2d<T > const&, double); */\
/*template vgl_vector_2d<T >      operator/    (vgl_vector_2d<T > const&, double); */\
/*template vgl_vector_2d<T >&     operator*=   (vgl_vector_2d<T >&, double); */\
/*template vgl_vector_2d<T >&     operator/=   (vgl_vector_2d<T >&, double); */\
/*template T      dot_product  (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template T      inner_product(vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template T      cross_product(vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template double cos_angle    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
template               double angle        (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&);\
template               double signed_angle (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&);\
template               bool   orthogonal   (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&, double);\
template               bool   parallel     (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&, double);\
/*template double operator/    (vgl_vector_2d<T > const&, vgl_vector_2d<T > const&); */\
/*template vgl_vector_2d<T >&     normalize    (vgl_vector_2d<T >&); */\
/*template vgl_vector_2d<T >      normalized   (vgl_vector_2d<T > const&); */\
template vgl_vector_2d<T >    rotated      (vgl_vector_2d<T > const&, double);\
template        std::ostream&  operator<<   (std::ostream&, vgl_vector_2d<T >const&);\
template        std::istream&  operator>>   (std::istream&, vgl_vector_2d<T >&)

#endif // vgl_vector_2d_hxx_
