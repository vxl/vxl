#ifndef vdgl_digital_curve_h
#define vdgl_digital_curve_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_digital_curve - Represents a 2D digital_curve
// .INCLUDE vgl/vdgl_digital_curve.h
// .FILE vdgl_digital_curve.txx
//
// .SECTION Description
//  A 2d image digital_curve
//
// .SECTION Author
//    Geoff Cross
// Created: xxx xx xxxx

#include <vsol/vsol_curve_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <vdgl/vdgl_interpolator_sptr.h>

class vdgl_digital_curve : public vsol_curve_2d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------
  vdgl_digital_curve( vdgl_interpolator_sptr interpolator);

  // Operators----------------------------------------------------------------

  // these must be defined as they are virtual in vsol_curve_2d
  vsol_point_2d_sptr p0() const;
  vsol_point_2d_sptr p1() const;
  double length() const;
  vsol_spatial_object_2d_sptr clone(void) const;
  // warning: the results of these methods are undefined
  void set_p0(const vsol_point_2d_sptr &);
  void set_p1(const vsol_point_2d_sptr &);

  // Split
  bool split ( vtol_vertex_2d* v, vdgl_digital_curve*& dc1, vdgl_digital_curve*& dc2 ); 

  // Data Access---------------------------------------------------------------

  // note that the index here runs from 0 to 1
  double get_x( const double s) const;
  double get_y( const double s) const;
  double get_theta(const double s) const;

  vdgl_interpolator_sptr get_interpolator() { return interpolator_; }


  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vdgl_digital_curve *cast_to_digital_curve(void) {return this;}

  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------

  vdgl_interpolator_sptr interpolator_;

private:
  // Helpers-------------------------------------------------------------------
};

#endif // _vdgl_digital_curve_h
