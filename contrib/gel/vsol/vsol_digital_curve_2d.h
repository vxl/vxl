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
// \date   2004/07/13
//
// \verbatim
//  Modifications
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vgl/vgl_fwd.h>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vcl_vector.h>

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
  vcl_vector<vsol_point_2d_sptr> samples_;

 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d();

  //---------------------------------------------------------------------------
  //: Constructor from a vcl_vector of points
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d(const vcl_vector<vsol_point_2d_sptr> &samples);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_digital_curve_2d(const vsol_digital_curve_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_digital_curve_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first point of `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p0(void) const; // virtual of vsol_digital_curve_2d

  //---------------------------------------------------------------------------
  //: Return the last point of `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p1(void) const; // virtual of vsol_digital_curve_2d

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
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

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
  virtual void set_p0(const vsol_point_2d_sptr &new_p0);

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  //---------------------------------------------------------------------------
  virtual void set_p1(const vsol_point_2d_sptr &new_p1);

  //---------------------------------------------------------------------------
  //: Add another point to the curve
  //---------------------------------------------------------------------------
  void add_vertex(const vsol_point_2d_sptr &new_p);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the curve type
  //---------------------------------------------------------------------------
  virtual vsol_curve_2d_type curve_type() const { return vsol_curve_2d::DIGITAL; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_digital_curve_2d const*cast_to_vsol_digital_curve(void)const{return this;}
  virtual vsol_digital_curve_2d *cast_to_digital_curve(void) {return this;}

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  virtual double length(void) const; // virtual of vsol_curve_2d

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return samples_.size(); }

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
  void describe(vcl_ostream &strm, int blanking=0) const;

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
  vcl_string is_a() const { return vcl_string("vsol_digital_curve_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const { return cls==is_a(); }
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
