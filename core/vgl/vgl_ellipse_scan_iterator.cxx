// This is vxl/vgl/vgl_ellipse_scan_iterator.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <vgl/vgl_ellipse_scan_iterator.h>

#include <vcl_cmath.h>

// Helper functions
namespace {
  inline double my_max( double x, double y ) { return x<y ? y : x; }
}


vgl_ellipse_scan_iterator::vgl_ellipse_scan_iterator( double xc, double yc, double rx, double ry, double theta )
  : xc_( xc ),
    yc_( yc ),
    rx_( rx*rx ),
    ry_( ry*ry ),
    theta_( theta ),
    y_( 0 ),
    min_y_( 0 )
{
}

vgl_ellipse_scan_iterator::~vgl_ellipse_scan_iterator()
{
}

void
vgl_ellipse_scan_iterator::reset()
{
  // The max value.
  double y0;
  if ( vcl_sin( theta_ ) == 0.0 ) {
    y0 = vcl_sqrt(ry_);
  } else {
    double t = vcl_atan2( vcl_sqrt(ry_) , vcl_sqrt(rx_) * vcl_tan( theta_ ) );
    y0 = vcl_sqrt(rx_) * vcl_cos( t ) * vcl_sin( theta_ ) + vcl_sqrt(ry_) * vcl_sin( t ) * vcl_cos( theta_ );
  }
  if ( y0 < 0 ) y0 = -y0;

  y_ = int( vcl_floor( yc_ + y0 ) ) + 1;
  min_y_ = int( vcl_ceil( yc_ - y0 ) );
}

bool
vgl_ellipse_scan_iterator::next()
{
  --y_;
  if ( y_ < min_y_ ) return false;

  double st = vcl_sin( -theta_ );
  double ct = vcl_cos( -theta_ );
  double A = rx_ * st * st + ry_ * ct * ct;

  double x0, x1; // the intersection points of the scan line; x0 >= x1

  if ( A > 0 ) {
    // not a denegerate horizontal line
    //
    double B = (rx_ - ry_) * (y_-yc_) * ct*st;
//  double C = - rx_*ry_ + (rx_*ct*ct + ry_*st*st)*(y_-yc_)*(y_-yc_);
    double D = rx_*ry_*(rx_*st*st + ry_*ct*ct - (y_-yc_)*(y_-yc_)); // = B*B-A*C
    if (D < 0) D=0; // could be slightly < 0 due to rounding errors

    x0 = (-B + vcl_sqrt( D )) / A;
    x1 = (-B - vcl_sqrt( D )) / A;
  } else {
    // "ellipse" is a horizontal line or a point
    //
    x0 = vcl_sqrt( my_max(rx_,ry_) );
    x1 = -x0;
  }

  start_x_= int( vcl_ceil( xc_ + x1 - 1e-9 ) ); // avoid problems with rounding
  end_x_ = int( vcl_floor( xc_ + x0 + 1e-9 ) ); // by slightly shifting.

  if ( start_x_ > end_x_ ) {
    // Could happen with very thin ellipses, near the end points
    return next();
  } else {
    return true;
  }
}
