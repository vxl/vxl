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
// \author Francois BERTEL
// \date   2000-04-28
//
// \verbatim
//  Modifications
//   2000-04-28 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Added constructor from vgl_line_segment_2d
//   2001-07-03 Peter Vanroose  Replaced vnl_double_2 by vgl_vector_2d
//   2004-05-14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

#include <string>
#include <iostream>
#include <iosfwd>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_fwd.h> // vgl_line_segment_2d, vgl_homg_line_2d, vgl_point_2d
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  //: Default Constructor
  //---------------------------------------------------------------------------
  vsol_line_2d();

  //---------------------------------------------------------------------------
  //: Constructor from the direction and the middle point
  //---------------------------------------------------------------------------
  vsol_line_2d(vgl_vector_2d<double> const& new_direction,
               const vsol_point_2d_sptr &new_middle);

  //---------------------------------------------------------------------------
  //: Constructor from the direction and the middle point
  //---------------------------------------------------------------------------
  vsol_line_2d(vgl_vector_2d<double> const& new_direction,
               const vgl_point_2d<double> &new_middle);

  //---------------------------------------------------------------------------
  //: Constructor from the first and the last point of the straight line
  //---------------------------------------------------------------------------
  vsol_line_2d(vsol_point_2d_sptr const& new_p0,
               vsol_point_2d_sptr const& new_p1)
    : vsol_curve_2d(), p0_(new_p0), p1_(new_p1) {}

  //---------------------------------------------------------------------------
  //: Constructor from two vgl_point_2d (end points)
  //---------------------------------------------------------------------------
  vsol_line_2d(vgl_point_2d<double> const& p0, vgl_point_2d<double> const& p1);

  //---------------------------------------------------------------------------
  //: Constructor from a vgl_line_segment_2d
  //---------------------------------------------------------------------------
  vsol_line_2d(vgl_line_segment_2d<double> const& l);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //  no duplication of the points
  //---------------------------------------------------------------------------
  vsol_line_2d(vsol_line_2d const& other) = default;

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_line_2d() override = default;

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a line_2d, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_line_2d const*cast_to_line()const override{return this;}
  vsol_line_2d *cast_to_line() override {return this;}

 private: // has been superseded by is_a()
  //: Return the curve type
  vsol_curve_2d_type curve_type() const override { return vsol_curve_2d::LINE; }

 public:
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Middle point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr middle() const;

  //---------------------------------------------------------------------------
  //: direction of the straight line segment.
  //---------------------------------------------------------------------------
  vgl_vector_2d<double> direction() const;

  //---------------------------------------------------------------------------
  //: First point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0() const override { return p0_; }

  //---------------------------------------------------------------------------
  //: Last point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1() const override { return p1_; }

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
  virtual bool operator==(vsol_line_2d const& other) const;
  bool operator==(vsol_spatial_object_2d const& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same points than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_line_2d const& o) const {return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box() const override;

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  double length() const override;

  //---------------------------------------------------------------------------
  //: Return the tangent angle (in degrees) of `this'. 0<angle<360
  //---------------------------------------------------------------------------
  double tangent_angle() const;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the straight line segment
  //---------------------------------------------------------------------------
  void set_p0(vsol_point_2d_sptr const& new_p0) override;

  //---------------------------------------------------------------------------
  //: Set the last point of the straight line segment
  //---------------------------------------------------------------------------
  void set_p1(vsol_point_2d_sptr const& new_p1) override;

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
  virtual bool in(vsol_point_2d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Return the tangent to `this' at `p'. Has to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_homg_line_2d<double>* tangent_at_point(vsol_point_2d_sptr const& p) const;

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_line_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;
};

//: Binary save vsol_line_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_line_2d* p);

//: Binary load vsol_line_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_line_2d* &p);

#endif // vsol_line_2d_h_
