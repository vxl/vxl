#ifdef __GNUC__
#pragma implementation
#endif

#include <vgl/vgl_ellipse_scan_iterator.h>

#include <vcl_cmath.h>
#include <vcl_cassert.h>

// Helper functions
namespace {
  inline double my_min( double x, double y ) { return x<y ? x : y; }
  inline double my_max( double x, double y ) { return x<y ? y : x; }
}


vgl_ellipse_scan_iterator::vgl_ellipse_scan_iterator( double xc, double yc, double a, double b, double theta )
  : xc_( xc ),
    yc_( yc ),
    a_( a*a ),
    b_( b*b ),
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
  if( vcl_sin( theta_ ) == 0.0 ) {
    y0 = vcl_sqrt(b_);
  } else {
    double t = vcl_atan( vcl_sqrt(b_) / (vcl_sqrt(a_) * vcl_tan( theta_ )) );
    y0 = vcl_sqrt(a_) * vcl_cos( t ) * vcl_sin( theta_ ) + vcl_sqrt(b_) * vcl_sin( t ) * vcl_cos( theta_ );
  }
  if( y0 < 0 ) y0 = -y0;

  y_ = int( vcl_floor( yc_ + y0 ) ) + 1;
  min_y_ = int( vcl_ceil( yc_ - y0 ) );
  
}

bool
vgl_ellipse_scan_iterator::next()
{
  --y_;
  if( y_ < min_y_ ) return false;

  double st = vcl_sin( -theta_ );
  double ct = vcl_cos( -theta_ );
  double A = a_ * st * st + b_ * ct * ct;
  double B = 2.0 * (a_ - b_) * (y_-yc_) * ct * st;
  double C = - a_ * b_ + (a_ * ct * ct + b_ * st * st) * (y_-yc_) * (y_-yc_);

  double x0, x1; // the intersection points of the scan line

  if( A > 0 ) {
    // not a denegerate horizontal line
    //
    assert( B*B - 4*A*C >= 0 );
    
    x0 = (-B + vcl_sqrt( B*B - 4*A*C )) / (2.0 * A);
    x1 = (-B - vcl_sqrt( B*B - 4*A*C )) / (2.0 * A);
  } else {
    // "ellipse" is a horizontal line or a point
    //
    x1 = vcl_sqrt( my_max(a_,b_) );
    x0 = -x1;
  }

  start_x_ = int( vcl_ceil( xc_ + my_min(x0,x1) ) );
  end_x_ = int( vcl_floor( xc_ + my_max(x0,x1) ) );

  if( start_x_ > end_x_ ) {
    // Could happen with very thin ellipses, near the end points
    return next();
  } else {
    return true;
  }
}
