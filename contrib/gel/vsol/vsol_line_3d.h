// This is gel/vsol/vsol_line_3d.h
#ifndef vsol_line_3d_h_
#define vsol_line_3d_h_
//*****************************************************************************
//:
// \file
// \brief Straight line segment in a 3D space.
//
// The direction gives the orientation and the length of the segment
//
// \author Francois BERTEL
// \date   2000-05-03
//
// \verbatim
//  Modifications
//   2000-05-03 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004-05-14 Peter Vanroose  Added describe()
//   2004-09-06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <iostream>
#include <iosfwd>
#include <vsol/vsol_curve_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_fwd.h> // vgl_vector_3d, vgl_homg_line_3d_2_points
#include <vcl_compiler.h>

class vsol_line_3d : public vsol_curve_3d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: First point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p0_;

  //---------------------------------------------------------------------------
  //: Last point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p1_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from the direction and the middle point
  //---------------------------------------------------------------------------
  vsol_line_3d(vgl_vector_3d<double> const& new_direction,
               vsol_point_3d_sptr const& new_middle);

  //---------------------------------------------------------------------------
  //: Constructor from the first and the last point of the straight line
  //---------------------------------------------------------------------------
  vsol_line_3d(vsol_point_3d_sptr const& new_p0,
               vsol_point_3d_sptr const& new_p1)
    : p0_(new_p0), p1_(new_p1) {}

  //---------------------------------------------------------------------------
  //: Copy constructor
  //  no duplication of the points
  //---------------------------------------------------------------------------
  vsol_line_3d(vsol_line_3d const& other) = default;

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_line_3d() override = default;

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a line_3d, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_line_3d const*cast_to_line()const override{return this;}
  vsol_line_3d *cast_to_line() override {return this;}

 private: // has been superseded by is_a()
  //: Return the curve type
  vsol_curve_3d_type curve_type() const override { return vsol_curve_3d::LINE; }

 public:
  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Middle point of the straight line segment
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr middle() const;

  //---------------------------------------------------------------------------
  //: direction of the straight line segment.
  //---------------------------------------------------------------------------
  vgl_vector_3d<double> direction() const;

  //---------------------------------------------------------------------------
  //: First point of the straight line segment; pure virtual of vsol_curve_3d
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p0() const override { return p0_; }

  //---------------------------------------------------------------------------
  //: Last point of the straight line segment; pure virtual of vsol_curve_3d
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p1() const override { return p1_; }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_line_3d const& other) const;
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Has `this' not the same points than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_line_3d const& o) const {return !operator==(o);}

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

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the straight line segment
  //---------------------------------------------------------------------------
  void set_p0(vsol_point_3d_sptr const& new_p0) override;

  //---------------------------------------------------------------------------
  //: Set the last point of the straight line segment
  //---------------------------------------------------------------------------
  void set_p1(vsol_point_3d_sptr const& new_p1) override;

  //---------------------------------------------------------------------------
  //: Set the length of `this'. Doesn't change middle point and orientation.
  //  If p0 and p1 are equal then the direction is set to (1,0,0)
  //  REQUIRE: new_length>=0
  //---------------------------------------------------------------------------
  void set_length(const double new_length);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Return the tangent to `this' at `p'. Has to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_homg_line_3d_2_points<double>* tangent_at_point(vsol_point_3d_sptr const& p) const;

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
  std::string is_a() const override { return std::string("vsol_line_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;
};

//: Binary save vsol_line_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_line_3d* p);

//: Binary load vsol_line_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_line_3d* &p);

#endif // vsol_line_3d_h_
