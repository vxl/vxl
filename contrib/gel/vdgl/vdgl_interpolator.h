#ifndef vdgl_interpolator_h
#define vdgl_interpolator_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_interpolator - Represents a 2D interpolator for a vdgl_edgel_chain
// .INCLUDE vgl/vdgl_interpolator.h
// .FILE vdgl_interpolator.txx
//
// .SECTION Description
//  A 2d edgel chain interpolator
//
// .SECTION Author
//    Geoff Cross
// Created: xxx xx xxxx

#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>

#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_distance.h>

class vdgl_interpolator : public vul_timestamp,
                          public vbl_ref_count {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------

  vdgl_interpolator( vdgl_edgel_chain_sptr chain) : chain_(chain) {};

  // Operators----------------------------------------------------------------

  // interpolation 0th degree
  virtual double get_x( const double index)= 0;
  virtual double get_y( const double index)= 0;

  // interpolation 1st degree
  virtual double get_theta( const double index)= 0;

  // interpolation 2nd degree
  virtual double get_curvature( const double index)= 0;

  // integral
  virtual double get_length()= 0;

  // bounding box
  virtual double get_min_x()= 0;
  virtual double get_max_x()= 0;
  virtual double get_min_y()= 0;
  virtual double get_max_y()= 0;

  // find closest point on the curve to the input point
  // virtual vgl_point_2d<double> closest_point_on_curve2 ( vsol_point_2d_sptr p ) = 0;
  virtual vsol_point_2d_sptr closest_point_on_curve ( vsol_point_2d_sptr p ) = 0;
  virtual float distance_curve_to_point ( vsol_point_2d_sptr p )
  {
    vsol_point_2d_sptr curve_point;
    curve_point = this->closest_point_on_curve ( p );
    if ( curve_point.ptr() == NULL )
      {
      return (float)10e10;
      }
    float f = vgl_distance ( p->get_p(), curve_point->get_p() );

      // curve_point->distance ( p );
    return f;
  }

  // Data Access---------------------------------------------------------------

  vdgl_edgel_chain_sptr get_edgel_chain() const { return chain_; }

  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------

  vdgl_edgel_chain_sptr chain_;

private:
  // Helpers-------------------------------------------------------------------
};


#endif // _vdgl_interpolator_h
