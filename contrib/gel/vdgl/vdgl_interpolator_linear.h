#ifndef vdgl_interpolator_linear_h
#define vdgl_interpolator_linear_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_interpolator_linear - Represents a 2D interpolator_linear for a vdgl_edgel_chain
// .INCLUDE vgl/vdgl_interpolator_linear.h
// .FILE vdgl_interpolator_linear.txx
//
// .SECTION Description
//  A 2d edgel chain interpolator_linear
//
// .SECTION Author
//    Geoff Cross
// Created: xxx xx xxxx

#include <vdgl/vdgl_interpolator.h>

class vdgl_interpolator_linear : public vdgl_interpolator {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator_linear( vdgl_edgel_chain_sptr chain);
  ~vdgl_interpolator_linear();

  // Operators----------------------------------------------------------------

  // interpolation 0th degree
  double get_x( const double index);
  double get_y( const double index);

  // interpolation 1st degree
  double get_theta( const double index);

  // interpolation 2nd degree
  double get_curvature( const double index);

  // integral
  double get_length();

  // bounding box
  double get_min_x();
  double get_max_x();
  double get_min_y();
  double get_max_y();

  // closest point
  vsol_point_2d_sptr closest_point_on_curve ( vsol_point_2d_sptr p );
  
  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------

  double lengthcache_;
  double minxcache_;
  double maxxcache_;
  double minycache_;
  double maxycache_;

private:
  // Helpers-------------------------------------------------------------------

  void recompute_all();
  void recompute_length();
  void recompute_bbox();

};


#endif // _vdgl_interpolator_linear_h
