// This is gel/vsol/vsol_line_2d.h
#ifndef vsol_line_2d_h_
#define vsol_line_2d_h_
//*****************************************************************************
//:
// \file
// \brief Straight line segment in a 2D space.
//
// The direction gives the orientation and the length of the segment
//
// \author François BERTEL
// \date   2000/04/28
//
// \verbatim
//  Modifications
//   2000/04/28 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2001/07/03 Peter Vanroose  Added constructor from vgl_line_segment_2d
//   2001/07/03 Peter Vanroose  Replaced vnl_double_2 by vgl_vector_2d
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_fwd.h> // vgl_line_segment_2d, vgl_homg_line_2d, vgl_point_2d
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
class vsol_line_2d : public vsol_curve_2d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: First point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0_;

  //---------------------------------------------------------------------------
  // Description: Last point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from the direction and the middle point
  //---------------------------------------------------------------------------
  vsol_line_2d(const vgl_vector_2d<double> &new_direction,
               const vsol_point_2d_sptr &new_middle);

  //---------------------------------------------------------------------------
  //: Constructor from the direction and the middle point
  //---------------------------------------------------------------------------
  vsol_line_2d(const vgl_vector_2d<double> &new_direction,
               const vgl_point_2d<double> &new_middle);

  //---------------------------------------------------------------------------
  //: Constructor from the first and the last point of the straight line
  //---------------------------------------------------------------------------
  vsol_line_2d(const vsol_point_2d_sptr &new_p0,
               const vsol_point_2d_sptr &new_p1);

  //---------------------------------------------------------------------------
  //: Constructor from two vgl_point_2d (end points)
  //---------------------------------------------------------------------------
  vsol_line_2d(vgl_point_2d<double> const& p0, vgl_point_2d<double> const& p1);

  //---------------------------------------------------------------------------
  //: Constructor from a vgl_line_segment_2d
  //---------------------------------------------------------------------------
  vsol_line_2d(const vgl_line_segment_2d<double> &l);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //  no duplication of the points
  //---------------------------------------------------------------------------
  vsol_line_2d(const vsol_line_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_line_2d();

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a line_2d, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_line_2d const*cast_to_line_2d(void)const{return this;}
  virtual vsol_line_2d *cast_to_line_2d(void) {return this;}

  //---------------------------------------------------------------------------
  //: Return the curve type
  //---------------------------------------------------------------------------
  virtual vsol_curve_2d_type curve_type(void) const { return vsol_curve_2d::LINE; }

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Middle point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr middle(void) const;

  //---------------------------------------------------------------------------
  //: direction of the straight line segment.
  //---------------------------------------------------------------------------
  vgl_vector_2d<double> direction(void) const;

  //---------------------------------------------------------------------------
  //: First point of the straight line segment
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p0(void) const;

  //---------------------------------------------------------------------------
  //: Last point of the straight line segment
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p1(void) const;

  //---------------------------------------------------------------------------
  //: Get an unbounded vgl_homg_line_2d
  //---------------------------------------------------------------------------
  vgl_homg_line_2d<double> vgl_hline_2d() const;

  //---------------------------------------------------------------------------
  //: Get a vgl_line_segment_2d
  //---------------------------------------------------------------------------
  vgl_line_segment_2d<double> vgl_seg_2d() const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_line_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same points than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_line_2d &o) const {return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a line. It is a CURVE
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  virtual double length(void) const;

  //---------------------------------------------------------------------------
  //: Return the tangent angle (in degrees) of `this'. 0<angle<360
  //---------------------------------------------------------------------------
  double tangent_angle(void) const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the straight line segment
  //---------------------------------------------------------------------------
  virtual void set_p0(const vsol_point_2d_sptr &new_p0);

  //---------------------------------------------------------------------------
  //: Set the last point of the straight line segment
  //---------------------------------------------------------------------------
  virtual void set_p1(const vsol_point_2d_sptr &new_p1);

  //---------------------------------------------------------------------------
  //: Set the length of `this'. Doesn't change middle point and orientation.
  //  If p0 and p1 are equal then the direction is set to (1,0)
  //  REQUIRE: new_length>=0
  //---------------------------------------------------------------------------
  void set_length(const double new_length);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(const vsol_point_2d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: Return the tangent to `this' at `p'. Has to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_homg_line_2d<double>* tangent_at_point(const vsol_point_2d_sptr &p) const;

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

vcl_ostream&  operator<<(vcl_ostream& s, vsol_line_2d const& p);

#include "vsol_line_2d_sptr.h"
//: Binary save vsol_line_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_line_2d_sptr const& p);

//: Binary load vsol_line_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_line_2d_sptr &p);

#endif // vsol_line_2d_h_
