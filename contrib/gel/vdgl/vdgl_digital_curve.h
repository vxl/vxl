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
#include <vdgl/vdgl_interpolator_ref.h>

class vdgl_digital_curve : public vsol_curve_2d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:
  
  // Constructors/Destructors--------------------------------------------------
  vdgl_digital_curve( vdgl_interpolator_ref interpolator);

  // Operators----------------------------------------------------------------

  // these must be defined as they are virtual in vsol_curve_2d
  vsol_point_2d_ref p0() const;
  vsol_point_2d_ref p1() const;
  double length() const;
  vsol_spatial_object_2d_ref clone(void) const;

  // warning: the results of these methods are undefined
  void set_p0(const vsol_point_2d_ref &);
  void set_p1(const vsol_point_2d_ref &);

  // Data Access---------------------------------------------------------------
  
  // note that the index here runs from 0 to 1
  double get_x( const double s) const;
  double get_y( const double s) const;
  double get_theta(const double s) const;

  vdgl_interpolator_ref get_interpolator() { return interpolator_; }


  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------
  
  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------

  vdgl_interpolator_ref interpolator_;

private:
  // Helpers-------------------------------------------------------------------
};

#endif // _vdgl_digital_curve_h
