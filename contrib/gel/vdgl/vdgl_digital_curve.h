// This is gel/vdgl/vdgl_digital_curve.h
#ifndef vdgl_digital_curve_h_
#define vdgl_digital_curve_h_
//:
// \file
// \brief Represents a 2D digital_curve
// \author  Geoff Cross
//
// \verbatim
//  Modifications:
//   10-Apr-2002 Peter Vanroose - Implemented split()
//   30-Nov-2002 J.L. Mundy added constructor from end points
//   18-May-2004 J.L. Mundy added binary I/O methods and equality tests
//   23-Sep-2004 Ming-Ching Chang Change cast_to_digital_curve() to cast_to_vdgl_digital_curve()
//                                to distinguish from vsol cast_to_digital_curve()
// \endverbatim

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>

class vdgl_digital_curve : public vsol_curve_2d
{
 protected:
  // Data Members--------------------------------------------------------------
  vdgl_interpolator_sptr interpolator_;

 public:
  // Constructors/Destructors--------------------------------------------------
  vdgl_digital_curve(); // for use by binary I/O only
  vdgl_digital_curve( const vdgl_interpolator_sptr& interpolator);
  vdgl_digital_curve(vsol_point_2d_sptr const& p0,
                     vsol_point_2d_sptr const& p1);

  // Operators----------------------------------------------------------------
  short order() const;
  // these must be defined as they are virtual in vsol_curve_2d
  vsol_point_2d_sptr p0() const override;
  vsol_point_2d_sptr p1() const override;
  double length() const override;
  vsol_spatial_object_2d* clone() const override;

  // warning: the results of these methods are undefined
  void set_p0(const vsol_point_2d_sptr &) override;
  void set_p1(const vsol_point_2d_sptr &) override;

  //: Split a digital curve into two pieces at the given point.
  //  If the location is not on the curve, the nearest point which does lie on
  //  the curve is selected.  If the point is outside the curve bounds, then
  //  only dc1 is returned, as the entire curve. dc2 is NULL.
  bool split(vsol_point_2d_sptr const& v,
             vdgl_digital_curve_sptr& dc1, vdgl_digital_curve_sptr& dc2);

  // Data Access---------------------------------------------------------------

  // note that the index here runs from 0 to 1
  double get_x( const double s) const;
  double get_y( const double s) const;
  double get_grad( const double s) const;
  double get_theta(const double s) const;
  double get_tangent_angle(const double s) const;
  int n_pts() const; //the number of discrete points underlying the curve
  vdgl_interpolator_sptr get_interpolator()const { return interpolator_; }

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  vdgl_digital_curve const*cast_to_vdgl_digital_curve()const override{return this;}
  vdgl_digital_curve *cast_to_vdgl_digital_curve() override {return this;}

 private: // has been superseded by is_a()
  //: Return the curve type
  vsol_curve_2d_type curve_type() const override { return vsol_curve_2d::DIGITAL_CURVE; }

 public:
  //: bounding box
  void compute_bounding_box() const override;

  //---------------------------------------------------------------------------
  //: Has `this' the same order interpolation and edgel values as other
  //---------------------------------------------------------------------------
  virtual bool operator==(const vdgl_digital_curve &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vdgl_digital_curve &o) const {return !operator==(o);}

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
  std::string is_a() const override { return std::string("vdgl_digital_curve"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }
};

//: Stream operator
std::ostream& operator<<(std::ostream& s, const vdgl_digital_curve& dc);

//: Binary save vdgl_digital_curve* to stream.
void vsl_b_write(vsl_b_ostream &os, const vdgl_digital_curve* dc);

//: Binary load vdgl_digital_curve* from stream.
void vsl_b_read(vsl_b_istream &is, vdgl_digital_curve* &dc);

//: Print human readable summary of vdgl_digital_curve* to a stream.
void vsl_print_summary(std::ostream &os, const vdgl_digital_curve* dc);

#endif // vdgl_digital_curve_h_
