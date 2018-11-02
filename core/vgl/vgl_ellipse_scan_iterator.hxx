// This is core/vgl/vgl_ellipse_scan_iterator.hxx
#ifndef vgl_ellipse_scan_iterator_hxx_
#define vgl_ellipse_scan_iterator_hxx_

#include <cmath>
#include "vgl_ellipse_scan_iterator.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Helper functions
namespace {
  template <class T> inline T my_max( T x, T y ) { return x<y ? y : x; }
}

template <class T>
vgl_ellipse_scan_iterator<T>::vgl_ellipse_scan_iterator( T xc, T yc, T rx, T ry, T theta )
  : xc_( xc ),
    yc_( yc ),
    rx_( rx*rx ),
    ry_( ry*ry ),
    theta_( theta ),
    y_( 0 ),
    min_y_( 0 )
{
}

template <class T>
vgl_ellipse_scan_iterator<T>::~vgl_ellipse_scan_iterator() = default;

template <class T>
void vgl_ellipse_scan_iterator<T>::reset()
{
  // The max value.
  T y0;
  if ( std::sin( theta_ ) == 0.0 ) {
    y0 = std::sqrt(ry_);
  }
  else {
    T t = std::atan2( std::sqrt(ry_) , std::sqrt(rx_) * std::tan( theta_ ) );
    y0 = std::sqrt(rx_) * std::cos( t ) * std::sin( theta_ ) + std::sqrt(ry_) * std::sin( t ) * std::cos( theta_ );
  }
  if ( y0 < 0 ) y0 = -y0;

  y_ = int( std::floor( yc_ + y0 ) ) + 1;
  min_y_ = int( std::ceil( yc_ - y0 ) );
}

template <class T>
bool vgl_ellipse_scan_iterator<T>::next()
{
  --y_;
  if ( y_ < min_y_ ) return false;

  T st = std::sin( -theta_ );
  T ct = std::cos( -theta_ );
  T A = rx_ * st * st + ry_ * ct * ct;

  T x0, x1; // the intersection points of the scan line; x0 >= x1

  if ( A > 0 ) {
    // not a degenerate horizontal line
    //
    T B = (rx_ - ry_) * (y_-yc_) * ct*st;
//  T C = - rx_*ry_ + (rx_*ct*ct + ry_*st*st)*(y_-yc_)*(y_-yc_);
    T D = rx_*ry_*(rx_*st*st + ry_*ct*ct - (y_-yc_)*(y_-yc_)); // = B*B-A*C
    if (D < 0) D=0; // could be slightly < 0 due to rounding errors

    x0 = (-B + std::sqrt( D )) / A;
    x1 = (-B - std::sqrt( D )) / A;
  }
  else {
    // "ellipse" is a horizontal line or a point
    //
    x0 = std::sqrt( my_max(rx_,ry_) );
    x1 = -x0;
  }

  start_x_= int( std::ceil( xc_ + x1 - 1e-9 ) ); // avoid problems with rounding
  end_x_ = int( std::floor( xc_ + x0 + 1e-9 ) ); // by slightly shifting.

  if ( start_x_ > end_x_ ) {
    // Could happen with very thin ellipses, near the end points
    return next();
  }
  else {
    return true;
  }
}

#undef VGL_ELLIPSE_SCAN_ITERATOR_INSTANTIATE
#define VGL_ELLIPSE_SCAN_ITERATOR_INSTANTIATE(T) \
template class vgl_ellipse_scan_iterator<T >

#endif // vgl_ellipse_scan_iterator_hxx_
