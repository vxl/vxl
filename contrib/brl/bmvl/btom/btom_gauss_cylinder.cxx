#include "btom_gauss_cylinder.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//: Constructor
btom_gauss_cylinder::btom_gauss_cylinder(float xy_sigma, float z_sigma,
                                         float length_sigma, float density,
                                         float x_origin, float y_origin,
                                         float z_position,
                                         float elevation, float azimuth)
{
  xy_sigma_=xy_sigma;
  z_sigma_=z_sigma;
  length_sigma_=length_sigma;
  density_=density;
  x_origin_=x_origin;
  y_origin_=y_origin;
  z_position_=z_position;
  elevation_=elevation;
  azimuth_=azimuth;
}

//: Destructor
btom_gauss_cylinder::~btom_gauss_cylinder()
{
}

//-------------------------------------------------------------------------
//: compute the cylinder intensity at a given location
//
float btom_gauss_cylinder::cylinder_intensity(float x, float y)
{
  //  cout << *this << endl;
  double theta = (90-elevation_)*vnl_math::pi/180.;
  double phi = azimuth_*vnl_math::pi/180.;
  double cth = vcl_cos(theta);
  double cths = cth*cth;
  double tth = vcl_tan(theta);
  double tths = tth*tth;
  double sph = vcl_sin(phi);
  double cph = vcl_cos(phi);
  double xyss = 1.0/(xy_sigma_*xy_sigma_);
  double zss = 1.0/(z_sigma_*z_sigma_);
  double wss = 1.0/(length_sigma_*length_sigma_);
  double D = (xyss+ tths*zss + wss/cths);
  double Dinv = 1.0/D;
  double Dinv2 = Dinv*tth/(xy_sigma_*z_sigma_);
  double xd = 1-xyss*Dinv;
  double zd = 1-zss*tths*Dinv;
  double z = z_position_;
  double az = z/z_sigma_;
  double azs = az*az;
  double xp = x - x_origin_;
  double yp = y - y_origin_;
  double xr = cph*xp - sph*yp;
  double yr = sph*xp + cph*yp;
  double ax = xr/xy_sigma_;
  double ay = yr/xy_sigma_;
  double axs = ax*ax;
  double ays = ay*ay;
  double ty = vcl_exp(-ays);
  double tx = vcl_exp(-axs*xd);
  double tz = vcl_exp(-azs*zd);
  double tz1 = vcl_exp(-2*ax*az*Dinv2);
  float pix = float(density_*tx*ty*tz*tz1);
  return pix;
}

//-------------------------------------------------------------------------
//: compute the radon transform of a vertical gaussian cylinder.
//  theta is in degrees on the range [0, 360]
//
float btom_gauss_cylinder::radon_transform(float theta, float t)
{
  double th_rad = theta*vnl_math::pi/180.;
  double neu = vcl_sin(th_rad)*x_origin_ +vcl_cos(th_rad)*y_origin_ -t;
  double neusq = neu*neu;
  double arg = neusq/(xy_sigma_*xy_sigma_);
  double radon = xy_sigma_*vcl_exp(-arg);
  return (float)radon;
}

vcl_ostream& operator<< (vcl_ostream& os, const btom_gauss_cylinder& gc)
{
  os << "btom_gauss_cylinder:\n[---\n"
     << "xy_sigma " << gc.xy_sigma_ << '\n'
     << "z_sigma " << gc.z_sigma_ << '\n'
     << "length_sigma " << gc.length_sigma_ << '\n'
     << "density " << gc.density_ << '\n'
     << "x_origin " << gc.x_origin_ << '\n'
     << "y_origin " << gc.y_origin_ << '\n'
     << "z_position " << gc.z_position_ << '\n'
     << "elevation " << gc.elevation_ << '\n'
     << "azimuth " << gc.azimuth_ << '\n'
     << "---]\n";
  return os;
}

