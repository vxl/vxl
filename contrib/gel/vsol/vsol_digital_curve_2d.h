// This is gel/vsol/vsol_digital_curve_2d.h
#ifndef vsol_digital_curve_2d_h_
#define vsol_digital_curve_2d_h_
//*****************************************************************************
//:
// \file
// \brief Digital curve in 2D with interpolation
//
// This class inherits from vsol_curve_2d.
//
// \author Matt Leotta
// \date   2004-07-13
//
// \verbatim
//  Modifications
// \endverbatim
//*****************************************************************************

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_fwd.h>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Digital curve class, part of the vsol_curve_2d hierarchy
// This class is more basic and "pure" than the vdgl counterpart.
// The curve is made up of vsol point and has no addition data members

class vsol_digital_curve_2d : public vsol_curve_2d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: List of vsol_point_2d
  //---------------------------------------------------------------------------
  std::vector<vsol_point_2d_sptr> samples_;

 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d();

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector of points
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d(std::vector<vsol_point_2d_sptr> samples);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d(const vsol_digital_curve_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_digital_curve_2d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first point of `this'
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0() const override; // pure virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Return the last point of `this'
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1() const override; // pure virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Return point `i'
  //  REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr point(const int i) const;

  //---------------------------------------------------------------------------
  //: Interpolate a point on the curve given a floating point index
  //  Linear interpolation is used for now
  //  \note index is NOT arc length.  For example, if size()==10
  //        then interp(5.5) is interpolated half way between points
  //        at indices 5 and 6.  In general this is not at 5.5 units along
  //        the curve or even at 55% through the curve.
  //---------------------------------------------------------------------------
  vgl_point_2d<double> interp(double index) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_digital_curve_2d &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_digital_curve_2d &o) const {return !operator==(o);}


  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the curve
  //  REQUIRE: in(new_p0)
  //---------------------------------------------------------------------------
  void set_p0(const vsol_point_2d_sptr &new_p0) override;

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  //---------------------------------------------------------------------------
  void set_p1(const vsol_point_2d_sptr &new_p1) override;

  //---------------------------------------------------------------------------
  //: Add another point to the curve
  //---------------------------------------------------------------------------
  void add_vertex(const vsol_point_2d_sptr &new_p);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d const*cast_to_digital_curve()const override{return this;}
  vsol_digital_curve_2d *cast_to_digital_curve() override {return this;}

 private: // has been superseded by is_a()
  //: Return the curve type
  vsol_curve_2d_type curve_type() const override { return vsol_curve_2d::DIGITAL_CURVE; }

 public:
  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  double length() const override; // pure virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box() const override;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size() const { return samples_.size(); }

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(unsigned int i) const { return i<samples_.size(); }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;

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
  std::string is_a() const override { return std::string("vsol_digital_curve_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }
};

//: Binary save vsol_digital_curve_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_digital_curve_2d* p);

//: Binary load vsol_digital_curve_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_digital_curve_2d* &p);

//: Return the floating point index of the point on the curve nearest to \p point
double closest_index(const vgl_point_2d<double>& point,
                     const vsol_digital_curve_2d_sptr& curve);

//: Split the input curve into two pieces at the floating point index
bool split(const vsol_digital_curve_2d_sptr& input,
           double index,
           vsol_digital_curve_2d_sptr& output1,
           vsol_digital_curve_2d_sptr& output2);

#endif // vsol_digital_curve_2d_h_
