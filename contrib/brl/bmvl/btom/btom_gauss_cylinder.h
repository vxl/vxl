//--*-c++-*--tells emacs this is c++
#ifndef gauss_cylinder_h_
#define gauss_cylinder_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A formulation of a cylinder with Gaussian distribution
//
// \author
//   J.L. Mundy
//
// Intrinsic parameters
//             xy_sigma_     - the in-slice Gaussian std-dev.
//             z_sigma_      - the across-slice Gaussian std-dev.
//             length_sigma_ - truncation of the cylinder axis.
//
// Extrinsic parameters are:
//             x_origin_     - x coordinate of the cylinder origin
//             y_origin_     - y coordinate of the cylinder origin
//             z_origin_     - z coordinate of the cylinder origin
//             elevation_    - elevation orientation of cylinder axis (deg)
//             azimuth_      - azimutha orientation of the cylinder axis (deg)
// \verbatim
//  Modifications:
//   J.L. Mundy March 04, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

class btom_gauss_cylinder : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:
  // Constructors/Initializers/Destructors-------------------------------------

  btom_gauss_cylinder(float xy_sigma=1.0, float z_sigma=5.0,
                      float length_sigma=10.0, float density=1.0,
                      float x_origin=0.0, float y_origin=0.0, float z_position=0.0,
                      float elevation=0.0, float azimuth=0.0);
  ~btom_gauss_cylinder();

  // Data Access---------------------------------------------------------------
  float get_xy_sigma() {return xy_sigma_;}
  float get_z_sigma() {return z_sigma_;}
  float get_length_sigma() {return length_sigma_;}
  float get_density() {return density_;}
  float get_x_origin() {return x_origin_;}
  float get_y_origin() {return y_origin_;}
  float get_z_position() {return z_position_;}
  float get_elevation() {return elevation_;}
  float get_azimuth() {return azimuth_;}

  void set_xy_sigma(float xy_sigma) {xy_sigma_=xy_sigma;}
  void set_z_sigma(float z_sigma) {z_sigma_=z_sigma;}
  void set_length_sigma(float length_sigma) {length_sigma_=length_sigma;}
  void set_density(float density) {density_ = density;}
  void set_x_origin(float x_origin) {x_origin_=x_origin;}
  void set_y_origin(float y_origin) {y_origin_=y_origin;}
  void set_z_position(float z_position) {z_position_=z_position;}
  void set_elevation(float elevation) {elevation_=elevation;}
  void set_azimuth(float azimuth) {azimuth_=azimuth;}

  // Utility Methods-------------------------------------------------------
  //The main simulation function
  float cylinder_intensity(float x, float y);
  float radon_transform(float theta, float t);
  // print params
  friend
    vcl_ostream& operator << (vcl_ostream& os, const btom_gauss_cylinder& gc);

 protected:
  // Utilities

  // Data Members--------------------------------------------------------------
  float xy_sigma_;
  float z_sigma_;
  float length_sigma_;
  float density_;
  float x_origin_;
  float y_origin_;
  float z_position_;
  float elevation_;
  float azimuth_;
};

#endif
