// This is gel/vsol/vsol_digital_curve_3d.h
#ifndef vsol_digital_curve_3d_h_
#define vsol_digital_curve_3d_h_
//:
// \file
// \brief Digital curve in 3D with interpolation
// Identical in interface and implementation to vsol_digital_curve_2d
//
// \author Peter Vanroose
// \date   24 September 2004

#include <vgl/vgl_fwd.h>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_curve_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_digital_curve_3d_sptr.h>
#include <vcl_vector.h>

//: Digital curve class, part of the vsol_curve_3d hierarchy
// The curve is made up of vsol points and has no addition data members
// \relates vdgl_digital_curve

class vsol_digital_curve_3d : public vsol_curve_3d
{
 protected:
  //: List of points
  vcl_vector<vsol_point_3d_sptr> samples_;

 public:
  // Default Constructor
  vsol_digital_curve_3d() : vsol_curve_3d(), samples_() {}

  //: Constructor from a list of points
  vsol_digital_curve_3d(vcl_vector<vsol_point_3d_sptr> const& sample_points)
    : vsol_curve_3d(), samples_(sample_points) {}

  // Copy constructor
  vsol_digital_curve_3d(vsol_digital_curve_3d const& other);

  // Destructor
  virtual ~vsol_digital_curve_3d() {}

  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  virtual vsol_spatial_object_3d* clone() const;

  //: Return the first point of `this'
  virtual vsol_point_3d_sptr p0() const; // pure virtual of vsol_curve_3d

  //: Return the last point of `this'
  virtual vsol_point_3d_sptr p1() const; // pure virtual of vsol_curve_3d

  //: Return point `i'
  //  REQUIRE: valid_index(i)
  vsol_point_3d_sptr point(unsigned int i) const;

  //: Linearly interpolate a point on the curve given a floating point index
  //  \note index is NOT arc length.  For example, if size()==10
  //        then interp(5.5) is interpolated half way between points
  //        at indices 5 and 6.  In general this is not at 5.5 units along
  //        the curve or even at 55% through the curve.
  //  \note interp(i) and point(i) will return the same point if i is integer.
  vgl_point_3d<double> interp(double index) const;

  //: Has `this' the same points than `other' in the same order ?
  virtual bool operator==(vsol_digital_curve_3d const&) const;
  virtual bool operator==(vsol_spatial_object_3d const&) const; // virtual of vsol_spatial_object_3d

  //: Has `this' the same points than `other' and in the same order ?
  inline bool operator!=(vsol_digital_curve_3d const& c) const {return !operator==(c);}

  //: Set the first point of the curve
  //  REQUIRE: in(new_p0)
  virtual void set_p0(vsol_point_3d_sptr const& new_p0);

  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  virtual void set_p1(vsol_point_3d_sptr const& new_p1);

  //: Add another point to the curve
  void add_vertex(vsol_point_3d_sptr const& new_p);

  //: Return `this' if `this' is a digital_curve, 0 otherwise
  virtual vsol_digital_curve_3d const*cast_to_digital_curve()const{return this;}
  virtual vsol_digital_curve_3d *cast_to_digital_curve() {return this;}

 private: // has been superceeded by is_a()
  //: Return the curve type
  virtual vsol_curve_3d_type curve_type() const { return vsol_curve_3d::DIGITAL_CURVE; }

 public:
  //: Return the length of `this'
  virtual double length() const; // pure virtual of vsol_curve_3d

  //: Compute the bounding box of `this'
  virtual void compute_bounding_box() const;

  //: Return the number of sample points of this digital curve
  unsigned int size() const { return samples_.size(); }

  //: Is `i' a valid index for the list of sample points ?
  //  This is the case if i is between 0 (inclusive) and size() (exclusive).
  bool valid_index(unsigned int i) const { return i<samples_.size(); }

  //: output description to stream
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
  virtual vcl_string is_a() const { return vcl_string("vsol_digital_curve_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const { return cls==is_a(); }
};

//: Binary save vsol_digital_curve_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_digital_curve_3d const* p);

//: Binary load vsol_digital_curve_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_digital_curve_3d* &p);

//: Return the floating point index of the point on the curve nearest to \p point
double closest_index(vgl_point_3d<double> const& point,
                     vsol_digital_curve_3d_sptr const& curve);

//: Split the input curve into two pieces at the floating point index
bool split(vsol_digital_curve_3d_sptr const& input,
           double index,
           vsol_digital_curve_3d_sptr& output1,
           vsol_digital_curve_3d_sptr& output2);

#endif // vsol_digital_curve_3d_h_
