// This is gel/vsol/vsol_conic_2d.cxx
#include "vsol_conic_2d.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/io/vgl_io_conic.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h> // for vcl_abs(double)
#include <vcl_cassert.h>

//---------------------------------------------------------------------------
//: Are `x' and `y' almost equal ?
//  The comparison uses an adaptive epsilon
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
//  The comparison uses a fixed epsilon, as the adaptive one from
//  are_equal() makes no sense here.
//---------------------------------------------------------------------------
inline static bool is_zero(double x) { return vcl_abs(x)<=1e-6; }

//***************************************************************************
// Initialization
//***************************************************************************

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
  vsol_curve_2d(), vgl_conic<double>(vgl_homg_point_2d<double>(c.x(),c.y(),1.0), rx, ry, theta)
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
  vsol_curve_2d(), vgl_conic<double>(vgl_homg_point_2d<double>(dir.x(),dir.y(),0.0), top.x(), top.y(), theta)
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
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_conic_2d::clone() const
{
  return new vsol_conic_2d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same coefficients and (geometrical) end points than `other'?
//  The test anticipates that the conic may have null endpoints
//---------------------------------------------------------------------------
bool vsol_conic_2d::operator==(vsol_conic_2d const& other) const
{
  if (this==&other)
    return true;
  // Delegate to both parent classes:
  bool conic_eq = vgl_conic<double>::operator==(other);
  // Check endpoints
  bool epts_eq = vsol_curve_2d::endpoints_equal(other);
  return conic_eq&&epts_eq;
}

//: spatial object equality

bool vsol_conic_2d::operator==(vsol_spatial_object_2d const& obj) const
{
  return
    obj.cast_to_curve() && obj.cast_to_curve()->cast_to_conic() &&
    *this == *obj.cast_to_curve()->cast_to_conic();
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Find the real type of the conic from its coefficients
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_type vsol_conic_2d::real_type() const
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
bool vsol_conic_2d::is_real_ellipse() const
{
  vsol_conic_type tmp=real_type();
  return (tmp==real_ellipse)||(tmp==real_circle);
}

//---------------------------------------------------------------------------
//: Is `this' a real circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_circle() const
{
  return real_type()==real_circle;
}

//---------------------------------------------------------------------------
//: Is `this' a complex ellipse ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_ellipse() const
{
  vsol_conic_type tmp=real_type();
  return (tmp==complex_ellipse)||(tmp==complex_circle);
}

//---------------------------------------------------------------------------
//: Is `this' a complex circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_circle() const
{
  return real_type()==complex_circle;
}

//---------------------------------------------------------------------------
//: Is `this' a parabola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_parabola() const
{
  return real_type()==parabola;
}

//---------------------------------------------------------------------------
//: Is `this' a hyperbola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_hyperbola() const
{
  return real_type()==hyperbola;
}

//---------------------------------------------------------------------------
//: Is `this' an real intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_intersecting_lines() const
{
  return real_type()==real_intersecting_lines;
}

//---------------------------------------------------------------------------
//: Is `this' an complex intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_intersecting_lines() const
{
  return real_type()==complex_intersecting_lines;
}

//---------------------------------------------------------------------------
//: Is `this' an coincident lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_coincident_lines() const
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
//-----------------------------------------------------------------------
// Return the angular position given a point on the ellipse 
double vsol_conic_2d::ellipse_angular_position(vsol_point_2d_sptr const& pt) const
{
  // require
  assert(is_real_ellipse());
  
  // Find the closest point to pt on the ellipse
  vsol_point_2d_sptr closest = this->closest_point_on_curve(pt);
  double x = closest->x(), y = closest->y();

  // Extract the ellipse parameters  
  double cx, cy, major_axis, minor_axis, angle;
  this->ellipse_parameters(cx, cy, angle, major_axis, minor_axis);
  
  x -= cx; y -= cy;

  //In this shifted frame:
  double phi = 
    vcl_atan2(major_axis*(vcl_cos(angle)*y-vcl_sin(angle)*x),
              minor_axis*(vcl_cos(angle)*x + vcl_sin(angle)*y));
  if(phi<0.0)
    phi += 2.0*vnl_math::pi;
  return phi;
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
double vsol_conic_2d::length() const
{
  assert(false); // TO DO
  return -1;
}

//---------------------------------------------------------------------------
//: Return the matrix associated with the coefficients.
//---------------------------------------------------------------------------
vnl_double_3x3 vsol_conic_2d::matrix() const
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
void vsol_conic_2d::set_p0(vsol_point_2d_sptr const& new_p0)
{
  // require
  assert(in(new_p0));

  p0_=new_p0;
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_conic_2d::set_p1(vsol_point_2d_sptr const& new_p1)
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
bool vsol_conic_2d::in(vsol_point_2d_sptr const& p) const
{
  const double x=p->x();
  const double y=p->y();
  return is_zero(a()*x*x+b()*x*y+c()*y*y+d()*x+e()*y+f());
}

//---------------------------------------------------------------------------
//: Return the tangent to the conic in the point p, if p is on the conic.
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

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_conic_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, static_cast<vgl_conic<double> >(*this));
  vsl_b_write(os, p0_);
  vsl_b_write(os, p1_);
}

//: Binary load self from stream (not typically used)
void vsol_conic_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   default:
    assert(!"vsol_conic I/O version should be 1");
   case 1:
    vgl_conic<double> q;
    vsl_b_read(is, q);
    vsl_b_read(is, p0_);
    vsl_b_read(is, p1_);
    this->set(q.a(), q.b(), q.c(), q.d(), q.e(), q.f());
  }
}

//: Return IO version number;
short vsol_conic_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_conic_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//external functions

//: Binary save vsol_conic_2d* to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_conic_2d* c)
{
  if (!c){
    vsl_b_write(os, false); // Indicate null conic stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null conic stored
    c->b_write(os);
  }
}

//: Binary load vsol_conic_2d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_conic_2d* &c)
{
  delete c;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    c = new vsol_conic_2d();
    c->b_read(is);
  }
  else
    c = 0;
}

