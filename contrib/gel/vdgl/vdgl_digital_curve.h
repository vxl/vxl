// This is gel/vdgl/vdgl_digital_curve.h
#ifndef vdgl_digital_curve_h_
#define vdgl_digital_curve_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Represents a 2D digital_curve
//
// \author  Geoff Cross
//
// \verbatim
//  Modifications:
//   10-Apr-2002 Peter Vanroose - Implemented split()
//   30-Nov-2002 J.L. Mundy added constructor from end points
// \endverbatim

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
  vdgl_digital_curve( vdgl_interpolator_sptr interpolator);
  vdgl_digital_curve(vsol_point_2d_sptr const& p0,
                     vsol_point_2d_sptr const& p1);
  // Operators----------------------------------------------------------------

  // these must be defined as they are virtual in vsol_curve_2d
  vsol_point_2d_sptr p0() const;
  vsol_point_2d_sptr p1() const;
  double length() const;
  vsol_spatial_object_2d* clone(void) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  // warning: the results of these methods are undefined
  void set_p0(const vsol_point_2d_sptr &);
  void set_p1(const vsol_point_2d_sptr &);

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
  vdgl_interpolator_sptr get_interpolator() { return interpolator_; }

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vdgl_digital_curve const*cast_to_digital_curve(void)const{return this;}
  virtual vdgl_digital_curve *cast_to_digital_curve(void) {return this;}

  //: bounding box
  virtual void compute_bounding_box(void) const;
};

#endif // vdgl_digital_curve_h_
