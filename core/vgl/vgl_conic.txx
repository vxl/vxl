// This is core/vgl/vgl_conic.txx
#ifndef vgl_conic_txx_
#define vgl_conic_txx_
//:
// \file
// Written by Peter Vanroose, ESAT, K.U.Leuven, Belgium, 30 August 2001.

#include "vgl_conic.h"

//#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_compiler.h>

static char *vgl_conic_name[]= {
  "invalid conic",
  "real ellipse",
  "real circle",
  "imaginary ellipse",
  "imaginary circle",
  "hyperbola",
  "parabola",
  "real intersecting lines",
  "complex intersecting lines",
  "real parallel lines",
  "complex parallel lines",
  "coincident lines"
};

//--------------------------------------------------------------
//: Returns the type name of the conic.

template <class T>
vcl_string vgl_conic<T>::real_type() const { return vgl_conic_name[(int)type_]; }

template <class T>
typename vgl_conic<T>::vgl_conic_type vgl_conic<T>::type_by_name(vcl_string const& name)
{
  for (int i = (int)no_type; i < num_conic_types; i++)
    if (name == vgl_conic_name[i])
      return (vgl_conic<T>::vgl_conic_type)i;
  return no_type; // should never reach this point
}

template <class T>
vcl_string vgl_conic<T>::type_by_number(typename vgl_conic<T>::vgl_conic_type type)
{
  if (type <= 0 || type >= num_conic_types) return vgl_conic_name[no_type];
  return vgl_conic_name[type];
}

//-------------------------------------------------------------
//: equality test
template <class T>
bool vgl_conic<T>::operator==(vgl_conic<T> const& that) const
{
  if ( type() != that.type() ) return false;
  return   a()*that.b() == b()*that.a()
        && a()*that.c() == c()*that.a()
        && a()*that.d() == d()*that.a()
        && a()*that.e() == e()*that.a()
        && a()*that.f() == f()*that.a()
        && b()*that.c() == c()*that.b()
        && b()*that.d() == d()*that.b()
        && b()*that.e() == e()*that.b()
        && b()*that.f() == f()*that.b()
        && c()*that.d() == d()*that.c()
        && c()*that.e() == e()*that.c()
        && c()*that.f() == f()*that.c()
        && d()*that.e() == e()*that.d()
        && d()*that.f() == f()*that.d()
        && e()*that.f() == f()*that.e();
}

//-------------------------------------------------------------
//: set values
template <class T>
void vgl_conic<T>::set(T a, T b, T c, T d, T e, T f)
{
  a_ = a; b_ = b; c_ = c; d_ = d; e_ = e; f_ = f;
  set_type_from_equation();
}

//-------------------------------------------------------------
//: constructor using polynomial coefficients.
template <class T>
vgl_conic<T>::vgl_conic(T const c[])
  : type_(no_type), a_(c[0]), b_(c[1]), c_(c[2]), d_(c[3]), e_(c[4]), f_(c[5])
{
  set_type_from_equation();
}

//-------------------------------------------------------------
//: constructor using polynomial coefficients.
template <class T>
vgl_conic<T>::vgl_conic(T a, T b, T c, T d, T e, T f)
  : type_(no_type), a_(a), b_(b), c_(c), d_(d), e_(e), f_(f)
{
  set_type_from_equation();
}

//: ctor using centre, signed radii, and angle, or (for parabola) top + excentricity
template <class T>
vgl_conic<T>::vgl_conic(vgl_homg_point_2d<T> const& c, T rx, T ry, T theta)
{
  if (c.w() == 0) { // This is a parabola
    a_ = c.y()*c.y();
    b_ = -2*c.x()*c.y();
    c_ = c.x()*c.x();
    // polar line of (rx,ry) must have direction (c.y(),-c.x()), hence
    // 2*a_*rx + b_*ry + d_ = 2*t*c.x() and b_*rx + 2*c_*ry +e_ = 2*t*c.y() :
    theta /= vcl_sqrt(c.x()*c.x()+c.y()*c.y()); // cannot be 0
    d_ = -2*a_*rx - b_*ry + 2*theta*c.x();
    e_ = -2*c_*ry - b_*rx + 2*theta*c.y();
    // conic must go through (rx,ry):
    f_ = -a_*rx*rx-b_*rx*ry-c_*ry*ry-d_*rx-e_*ry;
  }
  else { // hyperbola or ellipse
    rx = (rx < 0) ? (-rx*rx) : (rx > 0) ? (rx*rx) : 0;
    ry = (ry < 0) ? (-ry*ry) : (ry > 0) ? (ry*ry) : 0;

    double ct = vcl_cos(-theta);
    double st = vcl_sin(-theta);
    T u = c.x();
    T v = c.y();
    a_ = T(rx*st*st + ry*ct*ct);
    b_ = T(2*(rx-ry)*ct*st);
    c_ = T(rx*ct*ct + ry*st*st);
    d_ = T(-2*(rx*st*st + ry*ct*ct)*u - 2*(rx-ry)*ct*st*v);
    e_ = T(-2*(rx-ry)*ct*st*u - 2*(rx*ct*ct + ry*st*st)*v);
    f_ = T((rx*st*st +ry*ct*ct)*u*u + 2*(rx-ry)*ct*st*u*v + (rx*ct*ct + ry*st*st)*v*v - rx*ry);
  }
  set_type_from_equation();
}

//--------------------------------------------------------------------------
//: set conic type from polynomial coefficients.
// This method must be called by all constructors (except the default
// constructor) and all methods that change the coefficients.

template <class T>
void vgl_conic<T>::set_type_from_equation()
{
  T A = a_, B = b_/2, C = c_, D = d_/2, E = e_/2, F = f_;

  /* determinant, subdeterminants and trace values */
  T det = A*(C*F - E*E) - B*(B*F - D*E) + D*(B*E - C*D); // determinant
  T J = A*C - B*B;  // upper 2x2 determinant
  T K = (C*F - E*E) + (A*F - D*D); // sum of two other 2x2 determinants
  T I = A + C; // trace of upper 2x2

  if (det != 0) {
    if (J > 0) {
      if (det*I < 0) {
        if (A==C && B==0)      type_ = real_circle;
        else                   type_ = real_ellipse;
      }
      else {
        if (A==C && B==0)      type_ = imaginary_circle;
        else                   type_ = imaginary_ellipse;
      }
    }
    else if (J < 0)            type_ = hyperbola;
    else /* J == 0 */          type_ = parabola;
  }
  else {    // limiting cases
    if (J < 0)                 type_ = real_intersecting_lines;
    else if (J > 0)            type_ = complex_intersecting_lines;
    else /* J == 0 */ {
      if ( A == 0 && B == 0 && C == 0 ) { // line at infinity is component
        if ( D !=0 || E != 0 ) type_ = real_intersecting_lines;
        else if (F != 0)       type_ = coincident_lines; // 2x w=0
        else                   type_ = no_type; // all coefficients are 0
      }
      else if (K < 0)          type_ = real_parallel_lines;
      else if (K > 0)          type_ = complex_parallel_lines;
      else                     type_ = coincident_lines;
    }
  }
}

template <class T>
bool vgl_conic<T>::contains(vgl_homg_point_2d<T> const& p) const
{
  return p.x()*p.x()*a_+p.x()*p.y()*b_+p.y()*p.y()*c_+p.x()*p.w()*d_+p.y()*p.w()*e_+p.w()*p.w()*f_ == 0;
}

template <class T>
bool vgl_conic<T>::is_degenerate() const
{
  T A = a_, B = b_/2, C = c_, D = d_/2, E = e_/2, F = f_;
  T det = A*(C*F - E*E) - B*(B*F - D*E) + D*(B*E - C*D);
  return det == 0;
}

//: Returns the list of component lines, when degenerate and real components.
//  Otherwise returns an empty list.
template <class T>
vcl_list<vgl_homg_line_2d<T> > vgl_conic<T>::components() const
{
  if (!is_degenerate() ||
      type() == complex_intersecting_lines ||
      type() == complex_parallel_lines)
    return vcl_list<vgl_homg_line_2d<T> >(); // no real components

  T A = a_, B = b_/2, C = c_, D = d_/2, E = e_/2, F = f_;

  if (type() == coincident_lines) {
    // coincident lines: rank of the matrix of this conic must be 1
    vgl_homg_line_2d<T> l;
    if (A!=0 || B!=0 || D!=0)
      l = vgl_homg_line_2d<T>(A,B,D);
    else if (C!=0 || E!=0)
      l = vgl_homg_line_2d<T>(B,C,E);
    else // only F!=0 : 2x line at infinity w=0
      l = vgl_homg_line_2d<T>(D,E,F);
    return vcl_list<vgl_homg_line_2d<T> >(2,l); // two identical lines
  }

  // Both component lines must pass through the centre of this conic
  vgl_homg_point_2d<T> c = centre();

  if (type() == real_parallel_lines) {
    // In this case the centre lies at infinity.
    // Either these lines both intersect the X axis, or both intersect the Y axis:
    if (A!=0 || D!=0) { // X axis: intersections satisfy y=0 && Axx+2Dxw+Fww=0:
      vgl_homg_line_2d<T> l1(c, vgl_homg_point_2d<T>(-D+vcl_sqrt(D*D-A*F),0,A)),
                          l2(c, vgl_homg_point_2d<T>(-D-vcl_sqrt(D*D-A*F),0,A));
      vcl_list<vgl_homg_line_2d<T> > v(1,l1); v.push_back(l2);
      return v;
    }
    else { // Y axis: x=0 && Cyy+2Eyw+Fww=0:
      vgl_homg_line_2d<T> l1(c, vgl_homg_point_2d<T>(0,-E+vcl_sqrt(E*E-C*F),C)),
                          l2(c, vgl_homg_point_2d<T>(0,-E-vcl_sqrt(E*E-C*F),C));
      vcl_list<vgl_homg_line_2d<T> > v(1,l1); v.push_back(l2);
      return v;
    }
  }

  // Only remaining case: type() == real_intersecting_lines.
  if (A==0 && B==0 && C==0) { // line at infinity (w=0) is a component
    vcl_list<vgl_homg_line_2d<T> > v(1,vgl_homg_line_2d<T>(0,0,1));
    v.push_back(vgl_homg_line_2d<T>(d_,e_,f_));
    return v;
  }
  // If not, the two component lines are determined by c and their direction,
  // i.e., they pass through one of the two pts satisfying w=0 && Axx+2Bxy+Cyy=0:
  vgl_homg_line_2d<T> l1(c, vgl_homg_point_2d<T>(-B+vcl_sqrt(B*B-A*C),A,0)),
                      l2(c, vgl_homg_point_2d<T>(-B-vcl_sqrt(B*B-A*C),A,0));
  vcl_list<vgl_homg_line_2d<T> > v(1,l1); v.push_back(l2);
  return v;
}

//: Return true if a central conic
//  (This is an affine property, not a projective one.)
//  Equivalent to saying that the line at infinity does not touch the conic.
template <class T>
bool vgl_conic<T>::is_central() const {
  return type_ == real_ellipse|| type_ == imaginary_ellipse|| type_ == hyperbola
      || type_ == real_circle || type_ == imaginary_circle
      || type_ == real_intersecting_lines|| type_ == complex_intersecting_lines;
}

//--------------------------------------------------------------------------------
template <class T>
void vgl_conic<T>::translate_by(T x, T y)
{
  d_ += 2*a_*x + b_*y;
  f_ += c_ * y*y - a_ * x*x + d_ * x + e_ * y;
  e_ += 2*c_*y + b_*x;
  // This does not change the type, so no need to run set_type_from_equation()
}

template <class T>
vgl_conic<T> vgl_conic<T>::dual_conic() const
{
  T A = a_, B = b_/2, C = c_, D = d_/2, E = e_/2, F = f_;
  return vgl_conic<T>(E*E-C*F, 2*(B*F-D*E), D*D-A*F, 2*(C*D-B*E), 2*(A*E-B*D), B*B-A*C);
}

//: return the polar line of the homogeneous 2-D point p.
template <class T>
vgl_homg_line_2d<T> vgl_conic<T>::polar_line(vgl_homg_point_2d<T> const& p) const
{
  return vgl_homg_line_2d<T> (p.x()*a_  +p.y()*b_/2+p.w()*d_/2,
                              p.x()*b_/2+p.y()*c_  +p.w()*e_/2,
                              p.x()*d_/2+p.y()*e_/2+p.w()*f_  );
}

//: return the polar point of the homogeneous 2-D line l.
template <class T>
vgl_homg_point_2d<T> vgl_conic<T>::polar_point(vgl_homg_line_2d<T> const& l) const
{
  if (!is_degenerate()) {
    vgl_conic<T> c = this->dual_conic();
    return vgl_homg_point_2d<T> (l.a()*c.a()  +l.b()*c.b()/2+l.c()*c.d()/2,
                                 l.a()*c.b()/2+l.b()*c.c()  +l.c()*c.e()/2,
                                 l.a()*c.d()/2+l.b()*c.e()/2+l.c()*c.f()  );
  }
  else // a degenerate conic has no dual; in this case, return the centre:
    if (a_==0 && b_==0 && d_==0) // two horizontal lines
      return vgl_homg_point_2d<T>(1,0,0);
    else if (a_*c_*4==b_*b_ && a_*e_*2==b_*d_)
      return vgl_homg_point_2d<T>(b_*f_*2-e_*d_, d_*d_-a_*f_*4, a_*e_*2-b_*d_);
    else
      return vgl_homg_point_2d<T>(b_*e_-c_*d_*2, b_*d_-a_*e_*2, a_*c_*4-b_*b_);
}

//: Write "<vgl_conic aX^2+bXY+cY^2+dXW+eYW+fW^2=0>" to stream
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, vgl_conic<T> const& c) {
  s << "<vgl_conic ";
  if (c.a() == 1) s << "X^2";
  else if (c.a() == -1) s << "-X^2";
  else if (c.a() != 0) s << c.a() << "X^2";
  if (c.b() > 0) s << '+';
  if (c.b() == 1) s << "XY";
  else if (c.b() == -1) s << "-XY";
  else if (c.b() != 0) s << c.b() << "XY";
  if (c.c() > 0) s << '+';
  if (c.c() == 1) s << "Y^2";
  else if (c.c() == -1) s << "-Y^2";
  else if (c.c() != 0) s << c.c() << "Y^2";
  if (c.d() > 0) s << '+';
  if (c.d() == 1) s << "XW";
  else if (c.d() == -1) s << "-XW";
  else if (c.d() != 0) s << c.d() << "XW";
  if (c.e() > 0) s << '+';
  if (c.e() == 1) s << "YW";
  else if (c.e() == -1) s << "-YW";
  else if (c.e() != 0) s << c.e() << "YW";
  if (c.f() > 0) s << '+';
  if (c.f() == 1) s << "W^2";
  else if (c.f() == -1) s << "-W^2";
  else if (c.f() != 0) s << c.f() << "W^2";
  return s << "=0 " << c.real_type() << "> ";
}

//: Read a b c d e f from stream
template <class T>
vcl_istream& operator>>(vcl_istream& is,  vgl_conic<T>& q) {
  T a, b, c, d, e, f; is >> a >> b >> c >> d >> e >> f;
  q.set(a,b,c,d,e,f); return is;
}

#undef VGL_CONIC_INSTANTIATE
#define VGL_CONIC_INSTANTIATE(T) \
template class vgl_conic<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_conic<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_conic<T >&)

#endif // vgl_conic_txx_
