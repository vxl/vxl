#ifndef VSOL_CONIC_2D_H
#define VSOL_CONIC_2D_H
//*****************************************************************************
//
// .NAME vsol_conic_2d - Section of a conic curve in a 2D space
// .LIBRARY vsol
// .INCLUDE vsol/vsol_conic_2d.h
// .FILE    vsol/vsol_conic_2d.cxx

//:
// \file
// \brief Euclidean general conic class
// The conic is defined by its cartesian equation
// a x^2 +b xy +c y^2 +d x +e y +f
// This class inherits both from vsol_curve_2d and vgl_conic<double>.
// From the latter it inherits most of its geometric functionality, but
// robustified (with "eps" values to catch rounding errors), non-templated,
// and with non-homogeneous Euclidean 2D geometry terminology instead of
// homogeneous 3-tuples.
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2001/08/31 Peter Vanroose  constructor added from centre, size, orientation
// 2001/08/30 Peter Vanroose  now inheriting from vgl_conic
// 2001/08/29 Peter Vanroose  closest_point and distance to point added
// 2001/08/29 Peter Vanroose  conic intersection added (implemented in vgl)
// 2001/08/29 Peter Vanroose  midpoint() added
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2000/04/28 François BERTEL Creation
//*****************************************************************************

class vsol_conic_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_conic_2d_sptr.h>
#include <vgl/vgl_fwd.h>
#include <vgl/vgl_conic.h> // parent class

#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_cmath.h> // for vcl_abs(double)
#include <vcl_list.h>

//: Euclidean general conic class, part of the vsol_curve_2d hierarchy
class vsol_conic_2d
  : public vsol_curve_2d, public vgl_conic<double>
{
public:
  //---------------------------------------------------------------------------
  //: the different kinds of conic
  //---------------------------------------------------------------------------
  enum vsol_conic_type
  {
    invalid=0, // not 'degenerate' since numbers 7 to 11 are degenerate - PVr
    real_ellipse,
    real_circle,
    complex_ellipse,
    complex_circle,
    hyperbola,
    parabola,
    real_intersecting_lines,
    complex_intersecting_lines,
    real_parallel_lines,
    complex_parallel_lines,
    coincident_lines
  };

  //---------------------------------------------------------------------------
  //: Return the curve type
  //---------------------------------------------------------------------------
  virtual vsol_curve_2d_type curve_type() const { return vsol_curve_2d::CONIC; }

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from coefficient of the cartesian equation
  //  `new_a'x^2+`new_b'xy+`new_c'y^2+`new_d'x+`new_e'y+`new_f'
  //---------------------------------------------------------------------------
  explicit vsol_conic_2d(double new_a,
                         double new_b,
                         double new_c,
                         double new_d,
                         double new_e,
                         double new_f);

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
  vsol_conic_2d(vsol_point_2d const& c, double rx, double ry, double theta);

  //---------------------------------------------------------------------------
  //: Parabola constructor from direction, top and excentricity parameter.
  //  This constructor can only be used for non-degenerate parabolas:
  //  specify the direction of the symmetry axis, the top, and an excentricity
  //  parameter theta.
  //---------------------------------------------------------------------------
  vsol_conic_2d(vgl_vector_2d<double> const& dir, vsol_point_2d const& top, double theta);

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
  void set_central_parameters(vsol_point_2d const& c, double rx, double ry, double theta);

  //---------------------------------------------------------------------------
  //: Set parabola from direction, top and excentricity parameter.
  //  This can only be used for non-degenerate parabolas:
  //  specify the direction of the symmetry axis, the top, and an excentricity
  //  parameter theta.
  //---------------------------------------------------------------------------
  void set_parabola_parameters(vgl_vector_2d<double> const& dir,
                               vsol_point_2d const& top, double theta);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_conic_2d(const vsol_conic_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_conic_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first point of `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p0(void) const; // virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Return the last point of `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p1(void) const; // virtual of vsol_curve_2d

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same coefficients and the same end points than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_conic_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coeffs than `other', or different end points ?
  //---------------------------------------------------------------------------
  virtual bool operator!=(const vsol_conic_2d &other) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a line. It is a CURVE
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Return the real type of the conic from its coefficients
  //---------------------------------------------------------------------------
  virtual vsol_conic_type real_type(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' an real ellipse ?
  //---------------------------------------------------------------------------
  virtual bool is_real_ellipse(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a real circle ?
  //---------------------------------------------------------------------------
  virtual bool is_real_circle(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a complex ellipse ?
  //---------------------------------------------------------------------------
  virtual bool is_complex_ellipse(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a complex circle ?
  //---------------------------------------------------------------------------
  virtual bool is_complex_circle(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a parabola ?
  //---------------------------------------------------------------------------
  virtual bool is_parabola(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' a hyperbola ?
  //---------------------------------------------------------------------------
  virtual bool is_hyperbola(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' an real intersecting lines ?
  //---------------------------------------------------------------------------
  virtual bool is_real_intersecting_lines(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' an complex intersecting lines ?
  //---------------------------------------------------------------------------
  virtual bool is_complex_intersecting_lines(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' an coincident lines ?
  //---------------------------------------------------------------------------
  virtual bool is_coincident_lines(void) const;

  //---------------------------------------------------------------------------
  //: Return 3 ellipse parameters:
  //  -                            centre (`cx',`cy'),
  //  -                            orientation `phi',
  //  -                            size (`width',`height')
  //  REQUIRE: is_real_ellipse()
  //---------------------------------------------------------------------------
  virtual void ellipse_parameters(double &cx,
                                  double &cy,
                                  double &phi,
                                  double &width,
                                  double &height) const;

  //---------------------------------------------------------------------------
  //: Return 3 hyperbola parameters:
  //  -                            centre (`cx',`cy'),
  //  -                            orientation `phi',
  //  -                            size (`half-axis',-`half-secondary-axis')
  //  REQUIRE: is_hyperbola()
  //---------------------------------------------------------------------------
  virtual void hyperbola_parameters(double &cx,
                                    double &cy,
                                    double &phi,
                                    double &main_axis,
                                    double &secondary_axis) const;

  //---------------------------------------------------------------------------
  //: Return 2 parabola parameters:
  //  -                             top (`cx',`cy'),
  //  -                             orientation (`cosphi',`sinphi')
  //  REQUIRE: is_parabola()
  //---------------------------------------------------------------------------
  virtual void parabola_parameters(double &cx,
                                   double &cy,
                                   double &cosphi,
                                   double &sinphi) const;

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  virtual double length(void) const;

  //---------------------------------------------------------------------------
  //: Return the matrix associated with the coefficients.
  //---------------------------------------------------------------------------
  virtual vnl_double_3x3 matrix(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the curve
  //  REQUIRE: in(new_p0)
  //---------------------------------------------------------------------------
  virtual void set_p0(const vsol_point_2d_sptr &new_p0);

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  //---------------------------------------------------------------------------
  virtual void set_p1(const vsol_point_2d_sptr &new_p1);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the centre or symmetry point of a central conic.
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr midpoint() const;

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ? (ie `p' verifies the equation, within some margin)
  //---------------------------------------------------------------------------
  virtual bool in(const vsol_point_2d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: Returns the tangent to the conic in the point p, if p is on the conic.
  //  In general, returns the polar line of the point w.r.t. the conic.
  //---------------------------------------------------------------------------
  virtual vgl_homg_line_2d<double> *
  tangent_at_point(vsol_point_2d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Return the set of (real) intersection points of this conic with a line
  //---------------------------------------------------------------------------
  vcl_list<vsol_point_2d_sptr> intersection(vsol_line_2d const& line) const;

  //---------------------------------------------------------------------------
  //: Return the set of (real) intersection points of two conics
  //---------------------------------------------------------------------------
  vcl_list<vsol_point_2d_sptr> intersection(vsol_conic_2d const& c) const;

  //---------------------------------------------------------------------------
  //: Return the point on the conic boundary which is closest to the given point
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr closest_point_on_curve(vsol_point_2d_sptr const& pt) const;

  //---------------------------------------------------------------------------
  //: Return the shortest distance of the point to the conic boundary
  //---------------------------------------------------------------------------
  double distance(vsol_point_2d_sptr const& pt) const;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(vcl_ostream &strm, int blanking=0) const
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << static_cast<vgl_conic<double> >(*this) << '\n';
  }

private:
  //***************************************************************************
  // Internals
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Are `x' and `y' almost equal ?
  //  the comparison uses an adaptative epsilon
  //---------------------------------------------------------------------------
  inline static bool are_equal(double x, double y) {
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
  static bool is_zero(double x) { return vcl_abs(x)<=1e-6; }

  //---------------------------------------------------------------------------
  // virtuals of vsol_spatial_object_2d
  //---------------------------------------------------------------------------

  //: return reference point.  This is the midpoint for central conics.
  virtual vcl_vector<double> *GetLocation();
  //: return orientation of the main symmetry axis, if not degenerate.
  virtual vcl_vector<double> *GetOrientation();
  //: return 2-dimensional size vector
  virtual vcl_vector<double> *GetSize();
  //: move location() to (0,0).
  virtual bool Translate();

private:
  //***************************************************************************
  // Implementation
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: First point of the curve
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0_;

  //---------------------------------------------------------------------------
  //: Last point of the curve
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1_;
};

#endif // #ifndef VSOL_CONIC_2D_H
