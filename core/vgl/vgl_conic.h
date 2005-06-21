// This is core/vgl/vgl_conic.h
#ifndef vgl_conic_h_
#define vgl_conic_h_
//:
// \file
// \brief A quadratic plane curve
//
//  This example tells you the type of the given conic equation,
//  and prints the equation in readable form:
// \code
//   vgl_conic<double> c(1, 0, 2, 0, 0, -3);
//   vcl_cout << c.real_type() << '\n'; // prints "real ellipse"
//   vcl_cout << c << '\n'; // prints the equation: X^2 + 2 Y^2 - 3 = 0
// \endcode
//
// \verbatim
// Modifications
//   Peter Vanroose, 10 sep 1996 wrote description and example file.
//   Peter Vanroose, 17 jun 1998 added PolarLine() and PolarPoint().
//   Peter Vanroose, 18 jun 1998 added Hyperbola and Circle interface.
//   Peter Vanroose, 19 jun 1998 added dual space functions.
//   Peter Vanroose, 21 jun 1998 added Parabola interface.
//   Peter Vanroose, 27 jun 1998 added ComputeParabolaParameters().
//   M.Vergauwen & P.Vanroose, 4 jul 1998 added Intersect() & CommonTangents()
//   Peter Vanroose, 29 aug 2001 ported from Geometry to vgl
//   Peter Vanroose, 30 aug 2001 complete rewrite of most of the code
//   Peter Vanroose, 31 aug 2001 added extensive testing + fixed some bugs
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_list.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>

//: A quadratic plane curve
//
//  A conic is either an ellipse (or circle), a hyperbola, or a parabola.
//  It is represented by a quadratic equation in two nonhomogeneous
//  or three homogeneous coordinates.  Conversely, every quadratic
//  equation represents a conic, be it that it can be degenerate:
//  either in two (intersecting or parallel) lines, or in two
//  coincident lines.  Also, it can have no "visible", real points,
//  when it is an imaginary ellipse, or consist of two complementary
//  imaginary lines in which case it only has one real point, which could
//  still be at infinity.
//
//  These 11 cases are the possible values of vgl_conic::real_type().
//  The default constructor sets the type to "invalid conic";
//  otherwise the correct type is automatically set when the equation
//  of the conic is given to the constructor that takes 6 numeric values
//  (a,b,c,d,e,f): the cartesian equation is then
//  $ax^2 + bxy + cy^2 + dx + ey + f = 0$; the homogeneous equation is
//  $ax^2 + bxy + cy^2 + dxw + eyw + fw^2 = 0$.  (Sometimes with $z$ for $w$.)
//  The numeric type (typically double or float) is the template argument
//  of this class.
//
//  When the conic is degenerate and consists of two lines, the method
//  components() returns a list of two (possibly identical) lines.
//  Otherwise, this method returns an empty list.

template <class T>
class vgl_conic
{
 public:
  enum vgl_conic_type {
    no_type=0,
    real_ellipse,
    real_circle,
    imaginary_ellipse,
    imaginary_circle,
    hyperbola,
    parabola,
    real_intersecting_lines,
    complex_intersecting_lines,
    real_parallel_lines,
    complex_parallel_lines,
    coincident_lines,
    num_conic_types // is here to enable iterating through this list
  };

 private:
  // DATA MEMBERS

  vgl_conic_type type_;
  T a_; //!< coefficient of \a x^2
  T b_; //!< coefficient of \a xy
  T c_; //!< coefficient of \a y^2
  T d_; //!< coefficient of \a xw
  T e_; //!< coefficient of \a yw
  T f_; //!< coefficient of \a w^2

 public:
  inline vgl_conic_type type() const { return type_; }

  //: Returns the type of the conic as a string.
  // Possible returned strings are:
  // "real ellipse", "real circle", "imaginary ellipse", "imaginary circle",
  // "hyperbola", "parabola",
  // "real intersecting lines", "complex intersecting lines",
  // "real parallel lines", "complex parallel lines", "coincident lines".
  // The default constructor sets the type to "invalid conic".
  vcl_string real_type() const;

  //: Returns the internal enum value corresponding to the string argument.
  // Useful for comparison purposes, or for use in "case" statements.
  static vgl_conic_type type_by_name(vcl_string const& name);

  //: Converts the conic type from enum (internal representation) to string.
  static vcl_string type_by_number(vgl_conic_type type);

  //: Returns the coefficient of \f$X^2\f$
  inline T a() const { return  a_; }

  //: Returns the coefficient of \f$XY\f$
  inline T b() const { return  b_; }

  //: Returns the coefficient of \f$Y^2\f$
  inline T c() const { return  c_; }

  //: Returns the coefficient of \f$XW\f$
  inline T d() const { return  d_; }

  //: Returns the coefficient of \f$YW\f$
  inline T e() const { return  e_; }

  //: Returns the coefficient of \f$W^2\f$
  inline T f() const { return  f_; }

  // CONSTRUCTORS AND RELATED STUFF

  // default constructor
  vgl_conic() : type_(no_type) {}
#if 0 // The compiler defaults for these are all right
  // copy constructor
  vgl_conic(vgl_conic<T> const& c)
    : type_(c.type()), a_(c.a()), b_(c.b()), c_(c.c()), d_(c.d()), e_(c.e()), f_(c.f()) {}
  // assignment operator
  vgl_conic& operator=(vgl_conic<T> const& c) {
    type_=c.type(); a_=c.a(); b_=c.b(); c_=c.c(); d_=c.d(); e_=c.e(); f_=c.f();
    return *this;
  }
  // destructor
  ~vgl_conic() {}
#endif

  //: constructor using polynomial coefficients.
  //  The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
  //  where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
  vgl_conic(T a, T b, T c, T d, T e, T f);

  //: constructor using polynomial coefficients, given as a C array.
  //  The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
  //  where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
  vgl_conic(T const coeff[]);

  //: constructor using centre, signed radii, and angle.
  //  This constructor can only be used for non-degenerate, real
  //  conics: If the centre point c is a finite point and rx and ry
  //  have the same sign, an ellipse is defined (any ellipse can
  //  uniquely be specified this way); rx is the length of one main
  //  axis, ry of the other axis.  Hyperbolas are obtained if rx and
  //  ry have opposite sign; the positive one determines the distance
  //  from bots tops to the centre, and the other one specified the
  //  'minor' axis length. The rotation is about the centre of the
  //  ellipse or hyperbola. A parabola is obtained when the centre has
  //  w()=0, i.e., is a point at infinity. In that case (rx,ry) is the
  //  top, and theta is an excentricity parameter (since the centre
  //  already specifies the direction of the symmetry axis).
  vgl_conic(vgl_homg_point_2d<T> const& c, T rx, T ry, T theta);

  //: set or reset the conic using polynomial coefficients.
  //  The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
  //  where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
  void set(T a, T b, T c, T d, T e, T f);

  //: comparison operator.
  //  Comparison is on the conic, not the equation coefficients.  Hence two
  //  conics are identical if their coefficient vectors are multiples of
  //  each other.
  bool operator==(vgl_conic<T> const& c) const;

  // UTILITY FUNCTIONS

  //: Returns true if this conic is degenerate, i.e., if it consists of 2 lines.
  bool is_degenerate() const;
  
  //: Returns true if a central conic, i.e., an ellipse, circle, or hyperbola.
  //  Also the degenerate versions of these return true.
  //  Returns false if a parabola or two parallel or coinciding lines.
  bool is_central() const;

  //: Returns true if the point pt belongs to the conic.
  //  I.e., if it *exactly* satisfies the conic equation.
  bool contains(vgl_homg_point_2d<T> const& pt) const;

  //: Returns the list of component lines, when degenerate and real components.
  //  Otherwise returns an empty list.
  //  If two coinciding lines, the list contains two identical elements.
  //  Hence this list always has length 0 or 2.
  vcl_list<vgl_homg_line_2d<T> > components() const;

  // Elementary geometric functions ----------------------------------

  //: Returns the polar line of the given point, w.r.t. this conic.
  //  For a non-degenerate conic, the polar line of a point outside of the conic
  //  is the connection line of the two points on the conic that form the conic
  //  "contour" as seen from that point, i.e., the touch points of the two
  //  tangents to the conic going through the given point.
  //
  //  For a point on the conic, it is just the tangent in that point.
  //
  //  And for a point inside the conic, it is the set of all polar points of
  //  the lines through the given point. This set happens to be a straight line.
  vgl_homg_line_2d<T> polar_line(vgl_homg_point_2d<T> const& p) const;

  //: Returns the polar point of the given line, w.r.t. this conic.
  //  For a non-degenerate conic, the polar point of a line that intersects the
  //  conic in two points is the intersection point of the two tangent lines
  //  though those two points.  Hence it is the point of which this line is
  //  the polar line.
  //
  //  For a tangent line to the conic, it is just the tangent point.
  //
  //  And for a line not intersecting the conic, it is the common intersection
  //  point (inside the conic) of the polar lines of all points of that line.
  vgl_homg_point_2d<T>  polar_point(vgl_homg_line_2d<T> const& l) const;

  //: Returns the tangent to the conic in the point p, if p is on the conic.
  //  In general, returns the polar line of the point w.r.t. the conic.
  vgl_homg_line_2d<T> tangent_at(vgl_homg_point_2d<T> const& p) const { return polar_line(p); }

  //: Returns the centre of the conic, or its point at infinity if a parabola.
  // When two intersecting or parallel lines, returns their intersection point.
  // In all cases this is the polar point of the line at infinity.
  vgl_homg_point_2d<T> centre() const { return polar_point(vgl_homg_line_2d<T>(0,0,1)); }

  //: Converts the coefficients to a geometric description of an ellipse. 
  //  Returns false if the conic is not an ellipse. Double is appropriate
  //  since integer coefficients can produce non-integer ellipse parameters.
  bool ellipse_geometry(double& xc, double& yc, double& major_axis_length,
                        double& minor_axis_length, double& angle_in_radians);

  // Functions related to dual space ---------------------------------

  //: Returns the dual or tangential representation of this conic.
  //  The homogeneous coordinates of the points belonging to the dual conic
  //  are the coefficients of the equations of all tangents to the original
  //  conic.
  vgl_conic dual_conic() const;

  //: Returns the dual or tangential representation of this conic.
  vgl_conic tangential_form() const { return dual_conic(); }

  //: Modify this conic by translating it over distance \a x in the \a X direction and distance \a y in the \a Y direction.
  void translate_by(T x, T y);

 private:
  //--------------------------------------------------------------------------
  //: set conic type from polynomial coefficients and store in member type_
  // This method must be called by all constructors (except the default
  // constructor) and all methods that change the coefficients.
  void set_type_from_equation();
};

//: Write "<vgl_conic aX^2+bXY+cY^2+dXW+eYW+fW^2>" to stream
// \relates vgl_conic
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_conic<T> const& c);

//: Read a b c d e f from stream
// \relates vgl_conic
template <class T>
vcl_istream&  operator>>(vcl_istream& s, vgl_conic<T>& c);

#define VGL_CONIC_INSTANTIATE(T) extern "please include vgl/vgl_conic.txx first"

#endif // vgl_conic_h_
