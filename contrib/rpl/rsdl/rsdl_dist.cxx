#include <iostream>
#include <cmath>
#include "rsdl_dist.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

double
rsdl_dist_sq( const rsdl_point& p, const rsdl_point& q )
{
  unsigned int Nc = p.num_cartesian();
  unsigned int Na = p.num_angular();
  assert( Nc == q.num_cartesian() && Na == q.num_angular() );

  double sum_sq = 0;

  for ( unsigned int i=0; i<Nc; ++i ) {
    sum_sq += vnl_math::sqr( p.cartesian(i) - q.cartesian(i) );
  }

  for ( unsigned int j=0; j<Na; ++j ) {
    double diff = vnl_math::abs( p.angular(j) - q.angular(j) );
    if ( diff > vnl_math::pi ) {
      diff = vnl_math::twopi - diff;
    }
    sum_sq += vnl_math::sqr( diff );
  }

  return sum_sq;
}


double
rsdl_dist( const rsdl_point& p, const rsdl_point& q )
{
  return std::sqrt( rsdl_dist_sq( p, q ) );
}


double
rsdl_dist_sq( const rsdl_point & p, const rsdl_bounding_box &  b )
{
  double sum_sq = 0;
  unsigned int Nc = p.num_cartesian();
  unsigned int Na = p.num_angular();
  assert( Nc == b.num_cartesian() && Na == b.num_angular() );

  for ( unsigned int i=0; i<Nc; ++i ) {
    double x0 = b.min_cartesian(i), x1 = b.max_cartesian(i);
    double x = p.cartesian(i);
    if ( x < x0 ) {
      sum_sq += vnl_math::sqr( x0 - x );
    }
    else if ( x > x1 ) {
      sum_sq += vnl_math::sqr( x1 - x );
    }
  }

  for ( unsigned int j=0; j<Na; ++j ) {
    double a0 = b.min_angular(j), a1 = b.max_angular(j);
    double a = p.angular(j);
    if ( a0 > a1 ) {             // interval wraps around 0
      if ( a < a0 && a > a1 ) {  // outside interval, calculate distance
        sum_sq += vnl_math::sqr( std::min( a0-a, a-a1 ) );
      }
    }
    else {                       // interval does not wrap around
      if ( a > a1 ) {            // a is above a1
        sum_sq += vnl_math::sqr( std::min( a - a1, vnl_math::twopi + a0 - a ) );
      }
      else if ( a0 > a ) {       // a is below a0
        sum_sq += vnl_math::sqr( std::min( a0 - a, vnl_math::twopi + a - a1 ) );
      }
    }
  }

  return sum_sq;
}

double
rsdl_dist( const rsdl_point & p, const rsdl_bounding_box& b )
{
  return std::sqrt( rsdl_dist_sq( p, b ) );
}


bool
rsdl_dist_point_in_box( const rsdl_point & pt,
                        const rsdl_bounding_box & box )
{
  unsigned int Nc = pt.num_cartesian();
  unsigned int Na = pt.num_angular();
  assert( Nc == box.num_cartesian() && Na == box.num_angular() );

  for ( unsigned int i=0; i<Nc; ++i ) {
    double x = pt.cartesian(i);
    if ( x < box.min_cartesian(i) || box.max_cartesian(i) < x )
      return false;
  }

  for ( unsigned int j=0; j<Na; ++j ) {
    double a = pt.angular(j);
    if ( box.min_angular(j) < box.max_angular(j) ) {
      if ( a < box.min_angular(j) || box.max_angular(j) < a )
        return false;
    }
    else {
      if ( a > box.max_angular(j) && a < box.min_angular(j) )
        return false;
    }
  }

  return true;
}

void
rsdl_dist_box_relation( const rsdl_bounding_box & inner,
                        const rsdl_bounding_box & outer,
                        bool& inside,
                        bool& intersects )
{
  unsigned int Nc = inner.num_cartesian();
  unsigned int Na = inner.num_angular();
  assert( Nc == outer.num_cartesian() && Na == outer.num_angular() );
  inside = intersects = true;

  for ( unsigned int i=0; i<Nc && intersects; ++i ) {
    double x0 = inner.min_cartesian(i), x1 = inner.max_cartesian(i);
    double y0 = outer.min_cartesian(i), y1 = outer.max_cartesian(i);

    if ( x0 < y0 || y1 < x1 )
      inside = false;

    if ( y1 < x0 || x1 < y0 )
      intersects = false;
  }

  for ( unsigned int j=0; j<Na && intersects; ++j ) {
    double r0 = inner.min_angular(j), r1 = inner.max_angular(j);
    double s0 = outer.min_angular(j), s1 = outer.max_angular(j);

    if ( s0 <= s1 ) {    // outer angular interval doesn't wrap
      if ( r0 <= r1 ) {  // inner angular interval doesn't wrap
        if ( r0 < s0 || s1 < r1 )   // part of inner interval is outside
          inside = false;
        if ( r1 < s0 || r0 > s1 )   // all of inner interval is to the left or to the right
          intersects = false;
      }
      else {             //  inner angular interval does wrap
        inside = false;            // point of wrap-around is in inner but not in outer
        if ( r1 < s0 && s1 < r0 )
          intersects = false;
      }
    }
    else {               // outer angular interval does wrap
      if ( r0 <= r1 ) {  // inner angular interval doesn't wrap
        if ( r1 > s1 && r0 < s0 )
          inside = false;
        if ( r0 > s1 && r1 < s0 )
          intersects = false;
      }
      else {             // inner angular interval does wrap;  intersects must be true
        if ( r1 > s1 || r0 < s0 )
          inside = false;
      }
    }
  }
}
