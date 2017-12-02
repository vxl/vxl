// This is gel/vsol/vsol_conic_2d.h
#ifndef vsol_conic_2d_h_
#define vsol_conic_2d_h_
//*****************************************************************************
//:
// \file
// \brief Euclidean general conic class in 2D space
//
// The conic is defined by its cartesian equation
// a x^2 +b xy +c y^2 +d x +e y +f
// This class inherits both from vsol_curve_2d and vgl_conic<double>.
// From the latter it inherits most of its geometric functionality, but
// robustified (with "eps" values to catch rounding errors), non-templated,
// and with non-homogeneous Euclidean 2D geometry terminology instead of
// homogeneous 3-tuples.
//
// \author Francois BERTEL
// \date   2000-04-28
//
// \verbatim
//  Modifications
//   2000-04-28 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-08-29 Peter Vanroose  midpoint() added
//   2001-08-29 Peter Vanroose  conic intersection added (implemented in vgl)
//   2001-08-29 Peter Vanroose  closest_point and distance to point added
//   2001-08-30 Peter Vanroose  now inheriting from vgl_conic
//   2001-08-31 Peter Vanroose  constructor added from centre, size, orientation
//   2002-04-05 Peter Vanroose  axis() added
//   2003-01-08 Peter Vanroose  moved static private methods to vsol_conic_2d.cxx
//   2004-05-11 Joseph Mundy    added binary I/O methods
//   2004-09-23 Ming-Ching Chang Changed cast_to_conic_2d() to cast_to_conic()
// \endverbatim
//*****************************************************************************

#include <list>
#include <iostream>
#include <vgl/vgl_fwd.h>
#include <vgl/vgl_conic.h> // parent class
#include <vgl/vgl_conic_segment_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_compiler.h>

//: Euclidean general conic class, part of the vsol_curve_2d hierarchy

class vsol_conic_2d : public vsol_curve_2d, public vgl_conic<double>
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: First point of the curve
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0_;

  //---------------------------------------------------------------------------
  //: Last point of the curve
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1_;

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

 private: // has been superseded by is_a()
  //: Return the curve type
  virtual vsol_curve_2d_type curve_type() const { return vsol_curve_2d::CONIC; }

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor
  //  produces and invalid conic (needed for binary I/O)
  //---------------------------------------------------------------------------
  vsol_conic_2d() : vsol_curve_2d(), vgl_conic<double>() {}

  //---------------------------------------------------------------------------
  //: Constructor from coefficients of the cartesian equation
  //  `a'x^2+`b'xy+`c'y^2+`d'x+`e'y+`f'
  //---------------------------------------------------------------------------
  vsol_conic_2d(double ta, double tb, double tc, double td, double te, double tf)
    : vsol_curve_2d(), vgl_conic<double>(ta, tb, tc, td, te, tf) {}

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
  vsol_conic_2d(vsol_point_2d const& cntr, double rx, double ry, double theta);

  //---------------------------------------------------------------------------
  //: Parabola constructor from direction, top and eccentricity parameter.
  //  This constructor can only be used for non-degenerate parabolas:
  //  specify the direction of the symmetry axis, the top, and an eccentricity
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
  void set_central_parameters(vsol_point_2d const& cntr, double rx, double ry, double theta);

  //---------------------------------------------------------------------------
  //: Set parabola from direction, top and eccentricity parameter.
  //  This can only be used for non-degenerate parabolas:
  //  specify the direction of the symmetry axis, the top, and an eccentricity
  //  parameter theta.
  //---------------------------------------------------------------------------
  void set_parabola_parameters(vgl_vector_2d<double> const& dir,
                               vsol_point_2d const& top, double theta);

  //---------------------------------------------------------------------------
  //: Constructor from vgl_conic_segment_2d
  //---------------------------------------------------------------------------
  vsol_conic_2d(vgl_conic_segment_2d<double> & cs) : vsol_curve_2d(), vgl_conic<double>(cs.conic()),
     p0_(new vsol_point_2d(cs.point1())), p1_(new vsol_point_2d(cs.point2())) {}

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_conic_2d(vsol_conic_2d const& co):vsol_curve_2d(co),vgl_conic<double>(co) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_conic_2d() {}

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone() const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first point of `this';  pure virtual of vsol_curve_2d
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p0() const { return p0_; }

  //---------------------------------------------------------------------------
  //: Return the last point of `this';  pure virtual of vsol_curve_2d
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p1() const { return p1_; }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same coefficients and the same end points than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_conic_2d const& other) const;
  virtual bool operator==(vsol_spatial_object_2d const&) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coeffs than `other', or different end points ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_conic_2d const& o) const {return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of the conic from its coefficients
  //---------------------------------------------------------------------------
  vsol_conic_type real_type() const;
  std::string real_conic_type() const { return static_cast<vgl_conic<double> >(*this).real_type(); }

  //---------------------------------------------------------------------------
  //: Is `this' a real ellipse ?
  //---------------------------------------------------------------------------
  bool is_real_ellipse() const;

  //---------------------------------------------------------------------------
  //: Is `this' a real circle ?
  //---------------------------------------------------------------------------
  bool is_real_circle() const;

  //---------------------------------------------------------------------------
  //: Is `this' a complex ellipse ?
  //---------------------------------------------------------------------------
  bool is_complex_ellipse() const;

  //---------------------------------------------------------------------------
  //: Is `this' a complex circle ?
  //---------------------------------------------------------------------------
  bool is_complex_circle() const;

  //---------------------------------------------------------------------------
  //: Is `this' a parabola ?
  //---------------------------------------------------------------------------
  bool is_parabola() const;

  //---------------------------------------------------------------------------
  //: Is `this' a hyperbola ?
  //---------------------------------------------------------------------------
  bool is_hyperbola() const;

  //---------------------------------------------------------------------------
  //: Is `this' a pair of real intersecting lines ?
  //---------------------------------------------------------------------------
  bool is_real_intersecting_lines() const;

  //---------------------------------------------------------------------------
  //: Is `this' a pair of complex intersecting lines ?
  //---------------------------------------------------------------------------
  bool is_complex_intersecting_lines() const;

  //---------------------------------------------------------------------------
  //: Is `this' a pair of coincident lines ?
  //---------------------------------------------------------------------------
  bool is_coincident_lines() const;

  //---------------------------------------------------------------------------
  //: Return 3 ellipse parameters:
  //  -                            centre (`cx',`cy'),
  //  -                            orientation `phi',
  //  -                            size (`width',`height')
  //  REQUIRE: is_real_ellipse()
  //---------------------------------------------------------------------------
  void ellipse_parameters(double &cx,
                          double &cy,
                          double &phi,
                          double &width,
                          double &height) const;

  //---------------------------------------------------------------------------
  //: Return ellipse angular position
  //  -   input pt  , a point on the ellipse
  //  -   output angle
  //  -
  //  REQUIRE: is_real_ellipse()
  //---------------------------------------------------------------------------
  double ellipse_angular_position(vsol_point_2d_sptr const& pt) const;

  //---------------------------------------------------------------------------
  //: Return 3 hyperbola parameters:
  //  -                            centre (`cx',`cy'),
  //  -                            orientation `phi',
  //  -                            size (`half-axis',-`half-secondary-axis')
  //  REQUIRE: is_hyperbola()
  //---------------------------------------------------------------------------
  void hyperbola_parameters(double &cx,
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
  void parabola_parameters(double &cx,
                           double &cy,
                           double &cosphi,
                           double &sinphi) const;

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  virtual double length() const; // pure virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Return the matrix associated with the coefficients.
  //---------------------------------------------------------------------------
  vnl_double_3x3 matrix() const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the curve
  //  REQUIRE: in(new_p0)
  //---------------------------------------------------------------------------
  virtual void set_p0(vsol_point_2d_sptr const& new_p0);

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  //---------------------------------------------------------------------------
  virtual void set_p1(vsol_point_2d_sptr const& new_p1);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the centre or symmetry point of a central conic.
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr midpoint() const;

  //---------------------------------------------------------------------------
  //: Return the main symmetry axis, if not degenerate.
  //---------------------------------------------------------------------------
  vsol_line_2d_sptr axis() const;

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ? (ie `p' verifies the equation, within some margin)
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_2d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Returns the tangent to the conic in the point p, if p is on the conic.
  //  In general, returns the polar line of the point w.r.t. the conic.
  //---------------------------------------------------------------------------
  virtual vgl_homg_line_2d<double>* tangent_at_point(vsol_point_2d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Return the set of (real) intersection points of this conic with a line
  //---------------------------------------------------------------------------
  std::list<vsol_point_2d_sptr> intersection(vsol_line_2d const& line) const;

  //---------------------------------------------------------------------------
  //: Return the set of (real) intersection points of two conics
  //---------------------------------------------------------------------------
  std::list<vsol_point_2d_sptr> intersection(vsol_conic_2d const& co) const;

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
  inline void describe(std::ostream &strm, int blanking=0) const
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "vsol_conic_2d<" << static_cast<vgl_conic<double> >(*this)
         << '>' << std::endl;
  }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a conic, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_conic_2d const*cast_to_conic() const { return this; }
  virtual vsol_conic_2d *cast_to_conic() { return this; }

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const { return "vsol_conic_2d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }
};

//: Binary save vsol_conic_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_conic_2d* p);

//: Binary load vsol_conic_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_conic_2d* &p);

#endif // vsol_conic_2d_h_
