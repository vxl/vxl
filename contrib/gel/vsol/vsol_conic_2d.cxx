// This is gel/vsol/vsol_conic_2d.cxx
#include "vsol_conic_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h> // for vcl_abs(double)

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
//: Are `x' and `y' almost equal ?
//  the comparison uses an adaptive epsilon
//---------------------------------------------------------------------------
inline static bool are_equal(double x, double y)
{
  // epsilon is a fixed fraction of the absolute average of x and y
  const double epsilon=1e-6*(vcl_abs(x)+vcl_abs(y));
  // <=epsilon but not <epsilon, to compare to null values
  return vcl_abs(x-y)<=epsilon;
}

//---------------------------------------------------------------------------
//: Is `x' almost zero ?
//  the comparison uses a fixed epsilon, as the adaptive one from
//  are_equal() makes no sense here.
//---------------------------------------------------------------------------
inline static bool is_zero(double x) { return vcl_abs(x)<=1e-6; }

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from coefficient of the cartesian equation
// Description: `new_a'x^2+`new_b'xy+`new_c'y^2+`new_d'x+`new_e'y+`new_f'
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(double new_a,
                             double new_b,
                             double new_c,
                             double new_d,
                             double new_e,
                             double new_f) :
  vgl_conic<double>(new_a, new_b, new_c, new_d, new_e, new_f)
{
}

//---------------------------------------------------------------------------
//: Ellipse/hyperbola constructor from centre, size and orientation.
//  This constructor can only be used for non-degenerate, real ellipses and
//  hyperbolas: if rx and ry have the same sign, an ellipse is defined
//  (and any ellipse can uniquely be specified this way);
//  rx is the length of one main axis, ry of the other axis.
//  Hyperbolas are obtained if rx and ry have opposite sign; the positive
//  one determines the distance from bots tops to the centre, and the other
//  one specified the 'minor' axis length.
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(vsol_point_2d const& c, double rx, double ry, double theta) :
  vgl_conic<double>(vgl_homg_point_2d<double>(c.x(),c.y(),1.0), rx, ry, theta)
{
}

//---------------------------------------------------------------------------
//: Set ellipse/hyperbola from centre, size and orientation.
//  Can only be used for non-degenerate, real ellipses and
//  hyperbolas: if rx and ry have the same sign, an ellipse is defined
//  (and any ellipse can uniquely be specified this way);
//  rx is the length of one main axis, ry of the other axis.
//  Hyperbolas are obtained if rx and ry have opposite sign; the positive
//  one determines the distance from bots tops to the centre, and the other
//  one specified the 'minor' axis length.
//---------------------------------------------------------------------------
void vsol_conic_2d::set_central_parameters(vsol_point_2d const& c, double rx, double ry, double theta)
{
  vgl_conic<double> g(vgl_homg_point_2d<double>(c.x(),c.y(),1.0), rx, ry, theta);
  set(g.a(),g.b(),g.c(),g.d(),g.e(),g.f());
}

//---------------------------------------------------------------------------
//: Parabola constructor from direction, top and excentricity parameter.
//  This constructor can only be used for non-degenerate parabolas:
//  specify the direction of the symmetry axis, the top, and an excentricity
//  parameter theta.
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(vgl_vector_2d<double> const& dir,
                             vsol_point_2d const& top, double theta) :
  vgl_conic<double>(vgl_homg_point_2d<double>(dir.x(),dir.y(),0.0), top.x(), top.y(), theta)
{
}

//---------------------------------------------------------------------------
//: Set parabola from direction, top and excentricity parameter.
//  This can only be used for non-degenerate parabolas:
//  specify the direction of the symmetry axis, the top, and an excentricity
//  parameter theta.
//---------------------------------------------------------------------------
void vsol_conic_2d::set_parabola_parameters(vgl_vector_2d<double> const& dir,
                                            vsol_point_2d const& top, double theta)
{
  vgl_conic<double> g(vgl_homg_point_2d<double>(dir.x(),dir.y(),0.0), top.x(), top.y(), theta);
  set(g.a(),g.b(),g.c(),g.d(),g.e(),g.f());
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(const vsol_conic_2d &other) :
  vgl_conic<double>(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_conic_2d::~vsol_conic_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_conic_2d::clone(void) const
{
  return new vsol_conic_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_conic_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_conic_2d::p1(void) const
{
  return p1_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same coefficients and the same end points than `other' ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::operator==(const vsol_conic_2d &other) const
{
  // Delegate to both parent classes:
  return vgl_conic<double>::operator==(other) &&
         p0() == other.p0() && p1() == other.p1();
}

//: spatial object equality

bool vsol_conic_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::CONIC
  ? *this == (vsol_conic_2d const&) (vsol_curve_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a conic. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_conic_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Find the real type of the conic from its coefficients
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_type vsol_conic_2d::real_type(void) const
{
  if (type() == vgl_conic<double>::real_circle)
    return real_circle;
  else if (type() == vgl_conic<double>::real_ellipse)
    return real_ellipse;
  else if (type() == vgl_conic<double>::imaginary_circle)
    return complex_circle;
  else if (type() == vgl_conic<double>::imaginary_ellipse)
    return complex_ellipse;
  else if (type() == vgl_conic<double>::hyperbola)
    return hyperbola;
  else if (type() == vgl_conic<double>::parabola)
    return parabola;
  else if (type() == vgl_conic<double>::real_intersecting_lines)
    return real_intersecting_lines;
  else if (type() == vgl_conic<double>::complex_intersecting_lines)
    return complex_intersecting_lines;
  else if (type() == vgl_conic<double>::coincident_lines)
    return coincident_lines;
  else if (type() == vgl_conic<double>::real_parallel_lines)
    return real_parallel_lines;
  else if (type() == vgl_conic<double>::complex_parallel_lines)
    return complex_parallel_lines;
  else return invalid; // 'degenerate' was is not a good name: some of the above are already degenerate!
}

//---------------------------------------------------------------------------
//: Is `this' an real ellipse ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_ellipse(void) const
{
  vsol_conic_type tmp=real_type();
  return (tmp==real_ellipse)||(tmp==real_circle);
}

//---------------------------------------------------------------------------
//: Is `this' a real circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_circle(void) const
{
  return real_type()==real_circle;
}

//---------------------------------------------------------------------------
//: Is `this' a complex ellipse ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_ellipse(void) const
{
  vsol_conic_type tmp=real_type();
  return (tmp==complex_ellipse)||(tmp==complex_circle);
}

//---------------------------------------------------------------------------
//: Is `this' a complex circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_circle(void) const
{
  return real_type()==complex_circle;
}

//---------------------------------------------------------------------------
//: Is `this' a parabola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_parabola(void) const
{
  return real_type()==parabola;
}

//---------------------------------------------------------------------------
//: Is `this' a hyperbola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_hyperbola(void) const
{
  return real_type()==hyperbola;
}

//---------------------------------------------------------------------------
//: Is `this' an real intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_intersecting_lines(void) const
{
  return real_type()==real_intersecting_lines;
}

//---------------------------------------------------------------------------
//: Is `this' an complex intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_intersecting_lines(void) const
{
  return real_type()==complex_intersecting_lines;
}

//---------------------------------------------------------------------------
//: Is `this' an coincident lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_coincident_lines(void) const
{
  return real_type()==coincident_lines;
}

//---------------------------------------------------------------------------
//: Return 3 ellipse parameters: centre (`cx',`cy'), orientation `phi', size (`width',`height')
// Require: is_real_ellipse()
//---------------------------------------------------------------------------
void vsol_conic_2d::ellipse_parameters(double &cx,
                                       double &cy,
                                       double &phi,
                                       double &width,
                                       double &height) const
{
  // require
  assert(is_real_ellipse());

  const double b2=b()/2;
  const double d2=d()/2;
  const double e2=e()/2;
  const double det=a()*c()-b2*b2;

  if (is_zero(b2)) // only for accuracy
  {
    cx=-d2/a();
    cy=-e2/c();
  }
  else
  {
    cx=(b2*e2-c()*d2)/det;
    cy=(b2*d2-a()*e2)/det;
  }

  double f0=a()*cx*cx+b()*cx*cy+c()*cy*cy+d()*cx+e()*cy+f();

  if (is_zero(f0)) // avoid dividing by zero
    f0=1;
  const double a0=-a()/f0;
  const double b0=-b2/f0;
  const double c0=-c()/f0;

  // Now rotate the ellipse such that the main axis is horizontal.
  if (are_equal(a0,c0)&&is_zero(b0))
    phi=0; // circle
  else
    phi=vcl_atan2(-2*b0,c0-a0)/2; //ellipse

  const double cosphi=vcl_cos(phi);
  const double sinphi=vcl_sin(phi);
  width =vcl_sqrt(1.0/(a0*cosphi*cosphi+2*b0*cosphi*sinphi+c0*sinphi*sinphi));
  height=vcl_sqrt(1.0/(a0*sinphi*sinphi-2*b0*cosphi*sinphi+c0*cosphi*cosphi));
}

//---------------------------------------------------------------------------
//: Return 3 hyperbola parameters: centre (`cx',`cy'), orientation `phi', size (`half-axis',`half-secondary-axis')
// Require: is_hyperbola()
//---------------------------------------------------------------------------
void vsol_conic_2d::hyperbola_parameters(double &cx,
                                       double &cy,
                                       double &phi,
                                       double &width,
                                       double &height) const
{
  // require
  assert(is_hyperbola());

  const double b2=b()/2;
  const double d2=d()/2;
  const double e2=e()/2;
  const double det=a()*c()-b2*b2;

  cx=(b2*e2-c()*d2)/det;
  cy=(b2*d2-a()*e2)/det;

  double f0=a()*cx*cx+b()*cx*cy+c()*cy*cy+d()*cx+e()*cy+f();

  if (is_zero(f0)) // this should not happen
    f0=1;
  const double a0=-a()/f0;
  const double b0=-b2/f0;
  const double c0=-c()/f0;

  // Now rotate the hyperbola such that the main axis is horizontal.
  if (is_zero(b0)) { // axis already horizontal or vertical
    if (a0 > 0) phi = 0;
    else        phi = vcl_atan2(0.0,1.0); // 90 degrees
  }
  else
    phi=vcl_atan2(2*b0,a0-c0)/2;

  const double cosphi=vcl_cos(phi);
  const double sinphi=vcl_sin(phi);
  width = vcl_sqrt( 1.0/(a0*cosphi*cosphi+2*b0*cosphi*sinphi+c0*sinphi*sinphi));
  height=-vcl_sqrt(-1.0/(a0*sinphi*sinphi-2*b0*cosphi*sinphi+c0*cosphi*cosphi));
}

//---------------------------------------------------------------------------
//: Return 2 parabola parameters: top (`cx',`cy'), orientation (`cosphi',`sinphi')
// Require: is_parabola()
//---------------------------------------------------------------------------
void vsol_conic_2d::parabola_parameters(double & /* cx */,
                                        double & /* cy */,
                                        double &cosphi,
                                        double &sinphi) const
{
  // require
  assert(is_parabola());

  // Note that for a parabola B*B == 4*A*C, hence the quadratic part
  // of the equation is a square: (nX+mY)^2, with n=sqrt(A), m=sqrt(C)
  // Hence norm cannot be zero since the parabola is not degererate:
  const double norm=a()+c();
  // The parabola direction is then (-m,n):
  cosphi=-vcl_sqrt(c()/norm);
  sinphi=vcl_sqrt(a()/norm);
  // Finally, the top can be found as the point with tangent direction
  // orthogonal to the direction of the axis:
  // TODO
  vcl_cerr << "vsol_conic_2d::parabola_parameters() not yet fully implemented\n";
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_conic_2d::length(void) const
{
  assert(false); // TO DO
  return -1;
}

//---------------------------------------------------------------------------
//: Return the matrix associated with the coefficients.
//---------------------------------------------------------------------------
vnl_double_3x3 vsol_conic_2d::matrix(void) const
{
  vnl_double_3x3 result;

  // row 0
  result.put(0,0,a());
  result.put(0,1,b()/2);
  result.put(0,2,d()/2);
  // row 1
  result.put(1,0,b()/2);
  result.put(1,1,c());
  result.put(1,2,e()/2);
  // row 2
  result.put(2,0,d()/2);
  result.put(2,1,e()/2);
  result.put(2,2,f());

  return result;
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_conic_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  // require
  assert(in(new_p0));

  p0_=new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_conic_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  // require
  assert(in(new_p1));

  p1_=new_p1;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the centre or symmetry point of a central conic.
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_conic_2d::midpoint() const
{
  vgl_homg_point_2d<double> p = this->centre();
  return new vsol_point_2d(p.x()/p.w(), p.y()/p.w());
}

//---------------------------------------------------------------------------
//: Is `p' in `this' ? (ie `p' verifies the equation, within some margin)
//---------------------------------------------------------------------------
bool vsol_conic_2d::in(const vsol_point_2d_sptr &p) const
{
  const double x=p->x();
  const double y=p->y();
  return is_zero(a()*x*x+b()*x*y+c()*y*y+d()*x+e()*y+f());
}

//---------------------------------------------------------------------------
//: Returns the tangent to the conic in the point p, if p is on the conic.
//  In general, returns the polar line of the point w.r.t. the conic.
//---------------------------------------------------------------------------
vgl_homg_line_2d<double> *
vsol_conic_2d::tangent_at_point(vsol_point_2d_sptr const& p) const
{
  return new vgl_homg_line_2d<double>(
    vgl_conic<double>::tangent_at(vgl_homg_point_2d<double>(p->x(),p->y(),1.0)));
}

//---------------------------------------------------------------------------
//: Return the set of (real) intersection points of this conic with a line
//---------------------------------------------------------------------------
vcl_list<vsol_point_2d_sptr>
vsol_conic_2d::intersection(vsol_line_2d const& l) const
{
  vgl_homg_point_2d<double> p0(l.p0()->x(), l.p0()->y(), 1.0),
                            p1(l.p1()->x(), l.p1()->y(), 1.0);
  vgl_homg_line_2d<double> line(p0,p1);
  vcl_list<vgl_homg_point_2d<double> > vv =
    vgl_homg_operators_2d<double>::intersection(*this,line);
  vcl_list<vsol_point_2d_sptr> v;
  vcl_list<vgl_homg_point_2d<double> >::iterator it = vv.begin();
  for (; !(it == vv.end()); ++it) {
    if ((*it).w() != 0)  v.push_back(new vsol_point_2d((*it)));
  }
  return v;
}

//---------------------------------------------------------------------------
//: Return the set of (real) intersection points of two conics
//---------------------------------------------------------------------------
vcl_list<vsol_point_2d_sptr>
vsol_conic_2d::intersection(vsol_conic_2d const& c) const
{
  vcl_list<vgl_homg_point_2d<double> > vv =
    vgl_homg_operators_2d<double>::intersection(*this,c);
  vcl_list<vsol_point_2d_sptr> v;
  vcl_list<vgl_homg_point_2d<double> >::iterator it = vv.begin();
  for (; !(it == vv.end()); ++it) {
    if ((*it).w() != 0)  v.push_back(new vsol_point_2d((*it)));
  }
  return v;
}

//---------------------------------------------------------------------------
//: Return the point on the conic boundary which is closest to the given point
//---------------------------------------------------------------------------
vsol_point_2d_sptr
vsol_conic_2d::closest_point_on_curve(vsol_point_2d_sptr const& pt) const
{
  // The nearest point must have a polar line which is orthogonal to its
  // connection line with the given point; all points with this property form
  // a certain conic  (actually a hyperbola) :
  vcl_list<vsol_point_2d_sptr> candidates; // all intersection points
  if (b()==0 && a()==c()) {
    // this ellipse is a circle ==> degenerate hyperbola (line + line at infinity)
    candidates = intersection(vsol_line_2d(midpoint(),pt));
  } else {
    // Non-degenerate hyperbola:
    vsol_conic_2d conic(b()/2,
                        c()-a(),
                        -b()/2,
                        a()*pt->y()-b()/2*pt->x()+e()/2,
                        b()/2*pt->y()-c()*pt->x()-d()/2,
                        d()/2*pt->y()-e()/2*pt->x());
    // Now it suffices to intersect the hyperbola with "this" ellipse:
    candidates = conic.intersection(*this);
  }
  // And find the intersection point closest to the given location:
  vsol_point_2d_sptr p = 0;
  double dist = 1e31; // infinity
  vcl_list<vsol_point_2d_sptr>::iterator it = candidates.begin();
  for (; it != candidates.end(); ++it) {
    double d = (*it)->distance(pt);
    if (d < dist) { p = (*it); dist = d; }
  }
  return p;
}

//---------------------------------------------------------------------------
//: Return the shortest distance of the point to the conic boundary
//---------------------------------------------------------------------------
double vsol_conic_2d::distance(vsol_point_2d_sptr const& pt) const
{
  vsol_point_2d_sptr p = closest_point_on_curve(pt);
  return p->distance(pt);
}

//---------------------------------------------------------------------------
//: Return the main symmetry axis, if not degenerate.
//---------------------------------------------------------------------------
vsol_line_2d_sptr vsol_conic_2d::axis() const
{
  double cx, cy, phi, wd, ht;
  if (this->is_real_ellipse()) {
    this->ellipse_parameters(cx, cy, phi, wd, ht);
    vgl_vector_2d<double> v(vcl_cos(phi),vcl_sin(phi));
    return new vsol_line_2d(v,midpoint());
  }
  else if (this->is_hyperbola()) {
    this->hyperbola_parameters(cx, cy, phi, wd, ht);
    vgl_vector_2d<double> v(vcl_cos(phi),vcl_sin(phi));
    return new vsol_line_2d(v,midpoint());
  }
  else if (this->is_parabola()) {
    this->parabola_parameters(cx, cy, wd, ht);
    vgl_vector_2d<double> v(wd,ht);
    return new vsol_line_2d(v,new vsol_point_2d(cx,cy));
  }
  else return 0;
}
