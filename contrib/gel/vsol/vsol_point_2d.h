// This is gel/vsol/vsol_point_2d.h
#ifndef vsol_point_2d_h_
#define vsol_point_2d_h_
//*****************************************************************************
//:
// \file
// \brief Point in 2D space
//
// \author François BERTEL
// \date   2000/04/14
//
// \verbatim
//  Modifications
//   2000/04/14 François BERTEL Creation
//   2000/05/12 François BERTEL Replacement of vnl_vector_fixed<double,2> by vnl_double_2
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2000/09/18 Peter Tu        using vgl_point_2d instead of vnl_vector
//   2001/06/30 Peter Vanroose  Added constructor from vgl_point_2d
//   2001/07/03 Peter Vanroose  Replaced vnl_double_2 by vgl_vector_2d
//   2001/07/03 Peter Vanroose  Replaced new/delete by vgl_point_2d as member
//   2004/06/04 Josph Mundy     Added Binary I/O
// \endverbatim
//*****************************************************************************

class vsol_point_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>

class vsol_point_2d : public vsol_spatial_object_2d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Coordinates of the point
  //---------------------------------------------------------------------------
  vgl_point_2d<double> p_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from vgl_point_2d (automatic cast)
  //---------------------------------------------------------------------------
  inline vsol_point_2d(vgl_point_2d<double> const& p) : p_(p) {}

  //---------------------------------------------------------------------------
  //: Constructor from cartesian coordinates `x' and `y'
  //---------------------------------------------------------------------------
  inline vsol_point_2d(double x, double y) : p_(x,y) {}

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  inline vsol_point_2d(const vsol_point_2d &pt) : vsol_spatial_object_2d(pt), p_(pt.x(),pt.y()) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_point_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  //---------------------------------------------------------------------------
  //: Safe casting
  //---------------------------------------------------------------------------

  virtual vsol_point_2d* cast_to_point(void) { return this;}
  virtual const vsol_point_2d* cast_to_point(void) const { return this;}

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the abscissa
  //---------------------------------------------------------------------------
  inline double x(void) const { return p_.x(); }

  //---------------------------------------------------------------------------
  //: Return the ordinate
  //---------------------------------------------------------------------------
  inline double y(void) const { return p_.y(); }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_point_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_point_2d &o) const {return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a point. It is a POINT
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the abscissa
  //---------------------------------------------------------------------------
  virtual void set_x(const double new_x);

  //---------------------------------------------------------------------------
  //: Set the ordinate
  //---------------------------------------------------------------------------
  virtual void set_y(const double new_y);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: return the point
  //---------------------------------------------------------------------------
  vgl_point_2d<double> get_p () const { return p_; }

  //---------------------------------------------------------------------------
  //: Return the distance (N2) between `this' and `other'
  //---------------------------------------------------------------------------
  virtual double distance(const vsol_point_2d &other) const;
  virtual double distance(vsol_point_2d_sptr other) const;

  //---------------------------------------------------------------------------
  //: Return the middle point between `this' and `other'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr middle(const vsol_point_2d &other) const;

  //---------------------------------------------------------------------------
  //: Add `v' to `this'
  //---------------------------------------------------------------------------
  virtual void add_vector(vgl_vector_2d<double> const&v);

  //---------------------------------------------------------------------------
  //: Add `v' and `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr plus_vector(vgl_vector_2d<double> const&v) const;

  //---------------------------------------------------------------------------
  //: Return the vector `this',`other'.
  //---------------------------------------------------------------------------
  virtual vgl_vector_2d<double> to_vector(const vsol_point_2d &other) const;

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const;

};
//: Stream operator

vcl_ostream&  operator<<(vcl_ostream& s, vsol_point_2d const& p);

//: Binary save vsol_point_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_point_2d_sptr const& p);

//: Binary load vsol_point_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_point_2d_sptr &p);

#endif // vsol_point_2d_h_
