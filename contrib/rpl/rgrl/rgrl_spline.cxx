//:
// \file
// \author Lee, Ying-Lin (Bess)
// \date   Sept 2003

#include "rgrl_spline.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>

static double g( int a1, int a2, int m );
static double g_prime( int a1, int a2, int m );
static double g_double_prime( int a1, int a2, int m );

//: calculate the uniform cubic B-spline basis functions $B_i(u)$, 
// where $u \in [0,1]$ is a local parameter and
// \f[B_0(u) = (1-u)^3 / 6\f],
// \f[B_1(u) = (3u^3 - 6u^2 +4) / 6\f],
// \f[B_2(u) = (-3u^3 + 3u^2 +3u + 1) / 6\f],
// \f[B_3(u) = u^3 / 6\f].
static double bspline_basis_function( int i, double u );
static double bspline_basis_prime_function( int i, double u );

rgrl_spline::
rgrl_spline( vnl_vector< unsigned > const& m )
  : m_( m )
{
  int n = 1;
  for (unsigned i=0; i<m.size(); ++i)
    n *= (m_[i]+3);
  c_.set_size(n);
  c_.fill( 0.0 );
}

rgrl_spline::
rgrl_spline( vnl_vector< unsigned > const& m, vnl_vector< double > const& c )
  : m_( m ), c_( c )
{
  int n = 1;
  for (unsigned i=0; i<m.size(); ++i)
    n *= (m_[i]+3);
  assert( c.size() == n );
}

void
rgrl_spline::
set_control_points( vnl_vector<double> const& c )
{
  unsigned n = 1;
  for (unsigned i=0; i < m_.size(); ++i)
    n *= (m_[i]+3);
  assert( n == c.size() );
  c_ = c;
}

// f( x ) 
//
// At the beginning, I truncated f(x) so that f(x)=0 when x<0 or
// x>m. But this causes the boundary not continuous. Now I fix it.
// fixed????? -Gehua
// I can see it is still f(x)=0 when x<0 or x>m
double 
rgrl_spline::
f_x( vnl_vector<double> const& point ) const
{
  assert( point.size() == m_.size() );

  for( unsigned i=0; i<m_.size(); ++i ) {
//    assert( point[i] >= 0 && point[i] <= m_[i] );
  // check if it's in the valid region
  if( point[i] < 0 || point[i] > m_[i] )

//      // if it's out of the support region of control points, don't need
//      // to calculate the value.
//      if( point[i] < -3 || point[i] >= m_[i]+3 )
      return 0;
  }

  // to decrease memory allocation
  // make gr as static
  static vnl_vector< double > gr;
  basis_response( point, gr );

  return inner_product( c_, gr );

}

// Jacobian of f( x, y, z ) = ( d f(x,y,z) / dx, d f(x,y,z) / dy, d f(x,y,z) / dz )
vnl_vector< double >
rgrl_spline::
jacobian( vnl_vector< double > const& point ) const
{
  assert( point.size() == m_.size() );

  // check if it's in the valid region
  for( unsigned i=0; i<m_.size(); ++i ) {
//    assert( point[i] >= 0 && point[i] <= m_[i] );
    if( point[i] < 0 || point[i] > m_[i] )
      return vnl_vector< double >(m_.size(), 0.0);
  }

  unsigned num_control_pts = 1;
  unsigned dim = m_.size();
  for (unsigned n=0; n<dim; ++n)
    num_control_pts *= m_[n]+3;

  vnl_matrix<double> br( dim, num_control_pts, 0 );

  vnl_vector< int > floor( dim );
  vnl_vector< int > ceil( dim );
  vnl_vector< double > u( dim );
  vnl_vector< int > min( dim );
  vnl_vector< int > max( dim );

  for( unsigned n=0; n<dim; ++n ) {
    floor[n] = (int)vcl_floor( point[n] ) ;
    ceil[n] = (int)vcl_ceil( point[n] );
    u[n] = point[n] - floor[n];
    min[ n ] = ( floor[n] < 0 ) ? 0 : floor[n];
    max[ n ] = ( ceil[n] > (int)m_[n] ) ? m_[n] + 2 : ceil[n]+2;
  }

  // 3D case
  if (dim == 3) {
    int a = (m_[0]+3) * (m_[1]+3);
    int b = m_[0]+3;
    for( int i=min[2], index = a*i; i<=max[2]; ++i, index+=a ) {
      double b1 = bspline_basis_function( i-floor[2], u[2] );
      double b1_prime = bspline_basis_prime_function( i-floor[2], u[2] );
      for( int j=min[1], d = b*j + index; j<=max[1]; ++j, d+=b ) {
        double b2 = bspline_basis_function( j-floor[1], u[1] );
        double b2_prime = bspline_basis_prime_function( j-floor[1], u[1] );
        for( int k=min[0], e = d+k; k<=max[0]; ++k, ++e ) {
          double b3 = bspline_basis_function( k-floor[0], u[0] );
          double b3_prime = bspline_basis_prime_function( k-floor[0], u[0] );
          br(0, e) = b1_prime * b2 * b3;
          br(1, e) = b1 * b2_prime * b3;
          // ???? - Gehua
          // should this be br(2,e)?
          br(2, e) = b1 * b2 * b3_prime;
        }
      }
    }
  }
  // 2D case
  else if (dim == 2) {
    int a =(m_[0]+3);
    for( int i=min[1], index = a*i; i<=max[1]; ++i, index+=a ) {
      double b1 = bspline_basis_function( i-floor[1], u[1] );
      double b1_prime = bspline_basis_prime_function( i-floor[1], u[1] );
      for( int j=min[0], b = j + index; j<=max[0]; ++j, ++b ) {
        double b2 = bspline_basis_function( j-floor[0], u[0] );
        double b2_prime = bspline_basis_prime_function( j-floor[0], u[0] );
        br(0, b) = b1_prime * b2;
        br(1, b) = b1 * b2_prime;
      }
    }
  }
  // 1D case
  else if (dim == 1) {
    for( int i=min[0], b = i; i<= max[0]; ++i, ++b ) {
      double b1_prime = bspline_basis_prime_function( i-floor[0], u[0] );
      br( 0, b ) = b1_prime;
    }
  }
  
  // ---Gehua
  // Instead of matrix multiplication, another way is to move it 
  // into the loops, such as:
  // br(0, b) = c_(b) * b1_prime * b2;
  // This touches less memory.
  return br * c_;
}

void
rgrl_spline::
basis_response( vnl_vector<double> const& point, vnl_vector<double>& br ) const
{
  basis_response_helper( point, br, &bspline_basis_function );
}

void
rgrl_spline::
basis_response_helper( vnl_vector<double> const& point, vnl_vector<double>& br, func_type basis_func ) const
{
  unsigned num_element = 1;
  unsigned dim = m_.size();
  for (unsigned n=0; n<dim; ++n)
    num_element *= m_[n]+3;

  br.set_size(num_element);
  br.fill(0);

  vnl_vector< int > floor(dim);
  vnl_vector< int > ceil(dim);
  vnl_vector< double > u(dim);
  vnl_vector< int > min( dim );
  vnl_vector< int > max( dim );

  for( unsigned n=0; n<dim; ++n ) {
    floor[n] = (int)vcl_floor( point[n] ) ;
    ceil[n] = (int)vcl_ceil( point[n] );
    u[n] = point[n] - floor[n];
    min[ n ] = ( floor[n] < 0 ) ? 0 : floor[n];
    max[ n ] = ( ceil[n] > (int)m_[n] ) ? m_[n] + 2 : ceil[n]+2;
  }

  // 3D case
  if (dim == 3) {
    int a = (m_[0]+3) * (m_[1]+3);
    int b = m_[0]+3;
    for( int i=min[2], index = a*i; i<=max[2]; ++i, index+=a ) {
      double b1 = basis_func( i-floor[2], u[2] );
      for( int j=min[1], d = b*j + index; j<=max[1]; ++j, d+=b ) {
        double b2 = basis_func( j-floor[1], u[1] );
        for( int k=min[0], e = d+k; k<=max[0]; ++k, ++e ) {
          double b3 = basis_func( k-floor[0], u[0] );
          br[e] = b1 * b2 * b3;
        }
      }
    }
  }
  // 2D case
  else if (dim == 2) {
    int a =(m_[0]+3);
    for( int i=min[1], index = a*i; i<=max[1]; ++i, index+=a ) {
      double b1 = basis_func( i-floor[1], u[1] );
      for( int j=min[0], b = j + index; j<=max[0]; ++j, ++b ) {
        double b2 = basis_func( j-floor[0], u[0] );
        br[b] = b1 * b2;
      }
    }
  }
  // 1D case
  else if (dim == 1) {
    for( int i=min[0], b = i; i<= max[0]; ++i, ++b ) {
      double b1 = basis_func( i-floor[0], u[0] );
      br[b] = b1;
    }
  }
  
}

double
rgrl_spline::
element_1d_thin_plate(unsigned i, unsigned j) const
{
  // a1, b1 : {-1, 0, ..., m_[0] + 1}
  int a1 = i - 1;
  int b1 = j - 1;

  if ( vnl_math_abs( a1 - b1 ) > 3 ) return 0;

//  double gx = g( delta_[0], a1, b1, m_[0] ) * delta_[0];
//  double gx_prime = g_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
//    double gx_double_prime = g_double_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
    double gx_double_prime = g_double_prime( a1, b1, m_[0] );

  return gx_double_prime;
}

double
rgrl_spline::
element_2d_thin_plate(unsigned i, unsigned j) const
{
  // a1, b1 : {-1, 0, ..., m_[0] + 1}
  // a2, b2 : {-1, 0, ..., m_[1] + 1}
  int a1 = i % ( m_[0] + 3 ) - 1;
  int a2 = ( i / ( m_[0] + 3 ) ) - 1;

  int b1 = j % ( m_[0] + 3 ) - 1;
  int b2 = ( j / ( m_[0] + 3 ) ) - 1;

  if ( vnl_math_abs( a1 - b1 ) > 3 ) return 0;
  if ( vnl_math_abs( a2 - b2 ) > 3 ) return 0;

//    double gx = g( delta_[0], a1, b1, m_[0] ) * delta_[0];
//    double gx_prime = g_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
//    double gx_double_prime = g_double_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
  double gx = g( a1, b1, m_[0] );
  double gx_prime = g_prime( a1, b1, m_[0] );
  double gx_double_prime = g_double_prime( a1, b1, m_[0] );

//    double gy = g( delta_[0], a2, b2, m_[1] ) * delta_[1];
//    double gy_prime = g_prime( delta_[0], a2, b2, m_[1] ) * delta_[1];
//    double gy_double_prime = g_double_prime( delta_[0], a2, b2, m_[1] ) * delta_[1];
  double gy = g( a2, b2, m_[1] );
  double gy_prime = g_prime( a2, b2, m_[1] );
  double gy_double_prime = g_double_prime( a2, b2, m_[1] );

  return 
    ( gx_double_prime * gy +
      gx * gy_double_prime +
      2 * gx_prime * gy_prime );
}

double
rgrl_spline::
element_3d_thin_plate(unsigned i, unsigned j) const
{
  // a1, b1 : {-1, 0, ..., m_[0] + 1}
  // a2, b2 : {-1, 0, ..., m_[1] + 1}
  // a3, b3 : {-1, 0, ..., m_[2] + 1}
  int a1 = i % ( m_[0] + 3 ) - 1;
  int a2 = ( i / ( m_[0] + 3 ) ) % ( m_[1] + 3 ) - 1;
  int a3 = ( i / ( m_[0] + 3 ) ) / ( m_[1] + 3 ) - 1;

  int b1 = j % ( m_[0] + 3 ) - 1;
  int b2 = ( j / ( m_[0] + 3 ) ) % ( m_[1] + 3 ) - 1;
  int b3 = ( j / ( m_[0] + 3 ) ) / ( m_[1] + 3 ) - 1;

  if ( vnl_math_abs( a1 - b1 ) > 3 ) return 0;
  if ( vnl_math_abs( a2 - b2 ) > 3 ) return 0;
  if ( vnl_math_abs( a3 - b3 ) > 3 ) return 0;

//    double gx = g( delta_[0], a1, b1, m_[0] ) * delta_[0];
//    double gx_prime = g_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
//    double gx_double_prime = g_double_prime( delta_[0], a1, b1, m_[0] ) * delta_[0];
  double gx = g( a1, b1, m_[0] );
  double gx_prime = g_prime( a1, b1, m_[0] );
  double gx_double_prime = g_double_prime( a1, b1, m_[0] );

//    double gy = g( delta_[0], a2, b2, m_[1] ) * delta_[1];
//    double gy_prime = g_prime( delta_[0], a2, b2, m_[1] ) * delta_[1];
//    double gy_double_prime = g_double_prime( delta_[0], a2, b2, m_[1] ) * delta_[1];
  double gy = g( a2, b2, m_[1] );
  double gy_prime = g_prime( a2, b2, m_[1] );
  double gy_double_prime = g_double_prime( a2, b2, m_[1] );

//    double gz = g( delta_[0], a3, b3, m_[2] ) * delta_[2];
//    double gz_prime = g_prime( delta_[0], a3, b3, m_[2] ) * delta_[2];
//    double gz_double_prime = g_double_prime( delta_[0], a3, b3, m_[2] ) * delta_[2];
  double gz = g( a3, b3, m_[2] );
  double gz_prime = g_prime( a3, b3, m_[2] );
  double gz_double_prime = g_double_prime( a3, b3, m_[2] );

  return 
    ( gx_double_prime * gy * gz +
      gx * gy_double_prime * gz +
      gx * gy * gz_double_prime +
      2 * gx_prime * gy_prime * gz +
      2 * gx_prime * gy * gz_prime +
      2 * gx * gy_prime * gz_prime ) ;
}

void
rgrl_spline::
thin_plate_regularization(vnl_matrix<double>& regularization) const
{
  unsigned dim = m_.size();
  // The volume 
  double vol = 1;
  for( unsigned i=0; i<dim; ++i ) {
//    vol *= m_[i]*delta_[i];
    vol *= m_[i];
  }
  // The number of control points
  unsigned num = 1;
  for( unsigned i=0; i<dim; ++i ) {
    num *= m_[i] + 3;
  }

  regularization.set_size(num, num); 
  for( unsigned i=0; i<num; ++i ) {
    for( unsigned j=i; j<num; ++j ) {
      if( dim == 1 )
        regularization[i][j] = regularization[j][i] = element_1d_thin_plate(i, j) / vol;
      else if( dim == 2 )
        regularization[i][j] = regularization[j][i] = element_2d_thin_plate(i, j) / vol;
      else if( dim == 3 )
        regularization[i][j] = regularization[j][i] = element_3d_thin_plate(i, j) / vol;
    }
  }
}

// b_{i}(u), 0 <= u <= 1
double
bspline_basis_function( int i, double u )
{
  assert( u <= 1.0 && u >= 0.0 );
  assert( i <= 3 && i >= 0 );
  double b = 0;

  switch(i) {
  case 0: {
    double s = 1 - u;
    b = s*s*s / 6;
    break;}
  case 1:{
    b = ( 3*u*u*u - 6*u*u + 4 ) / 6;
    break;}
  case 2:{
    b = ( -3*u*u*u + 3*u*u + 3*u + 1 ) / 6;
    break;}
  case 3:{
    b = u*u*u / 6;
    break;}
  default: {
    vcl_cerr << "rgrl_spline::basis_function: wrong index for basis functions : " << i << "\n";}
  }
  return b;
}

double
bspline_basis_prime_function( int i, double u )
{
  assert( u <= 1 && u >= 0 );
  assert( i <= 3 && i >= 0 );
  double b = 0;

  switch(i) {
  case 0: {
    double s = 1 - u;
    b = -s*s / 2;
    break;}
  case 1:{
    b = ( 3*u*u - 4*u ) / 2;
    break;}
  case 2:{
    b = ( -3*u*u + 2*u + 1 ) / 2;
    break;}
  case 3:{
    b = u*u / 2;
    break;}
  default: {
    vcl_cerr << "rgrl_spline::basis_function: wrong index for basis functions\n";}
  }
  return b;
}

static
double
//g( double u, int a1, int a2, int m )
g( int a1, int a2, int m )
{
  int min_a, max_a;
  if ( a1 < a2 ) {
    min_a = a1;
    max_a = a2;
  }
  else {
    min_a = a2;
    max_a = a1;
  }

  assert( min_a >= -1 );
  assert( max_a <= m+1 );
  int diff_a = vnl_math_abs( a1 - a2 );

  double ans = 0;
  if( diff_a == 0 ) {
    double ans = 0;
    if( max_a <= m-2 ) {
      // \int [b_0(x)*b_0(x)] dx = \int [(1-u)^6 / 36] dx
      ans += 1.0 / 252 ;
    }
    if( min_a >= 0 && max_a <= m-1 ) {
      // \int [b_1(x)*b_1(x)] dx = \int [(3u^3-6u^2+4)^2 / 36] dx
      ans += 0.2 + 1.0 / 28;
    }
    if( min_a >= 1 && max_a <= m ) {
      // \int [b_2(x)*b_2(x)] dx = \int [b_1(x)*b_1(x)] dx
      ans += 0.2 + 1.0 / 28;
    }
    if( min_a >= 2 ) {
      // \int [b_3(x)*b_3(x)] dx = \int [b_0(x)*b_0(x)] dx
      ans += 1.0 / 252;
    }
    return ans;
  }

  if( diff_a == 1 ) {
    if( max_a <= m-1 ) {
      // \int [b_0(u)*b_1(u)] du = \int [-3u^6 + 15u^5 - 27u^4 + 17u^3 + 6u^2 - 12u + 4]/36 du
      ans += 0.0375 - 1.0 / 84;
    }
    if( min_a >= 0 && max_a <= m ) {
      // \int [b_1(u)*b_2(u)] du = \int [-9u^6 + 27u^5 - 9u^4 -27u^3 + 6u^2 + 12u + 4]/36 du
      ans += -0.1125 - 1.0 / 28 + 1.0 / 3;
    }
    if( min_a >= 1 ) {
      // \int [b_2(u)*b_3(u)] du = \int [b_0(u)*b_1(u)] du
      ans += 0.0375 - 1.0 / 84;
    }
    return ans;
  }

  if( diff_a == 2 ) {
    if( max_a <= m ) {
      // \int [b_0(u)*b_2(u)] du = \int [3u^6 - 12u^5 + 15u^4 - 4u^3 - 3u^2 + 1]/36 du
      ans += 1.0 / 84;
    }
    if( min_a >= 0 ) {
      // \int [b_1(u)*b_3(u)] du = \int [b_0(u)*b_2(u)] du
      ans += 1.0 / 84;
    }
    return ans;
  }
  
  if( diff_a == 3 ) {
    // \int [b_0(u)*b_3(u)] du = \int [-u^6 + 3u^5 -3u^4 + u3]/36 du
    ans += 1.0 / 240 - 1.0 / 252;
    return ans;
  }

  return 0;
}

static
double
//g_prime(double u, int a1, int a2, int m )
g_prime( int a1, int a2, int m )
{
  int min_a = (a1 < a2) ? a1 : a2;
  int max_a = (a1 < a2) ? a2 : a1;
  assert( min_a >= -1 );
  assert( max_a <= m+1 );
  int diff_a = vnl_math_abs( a1 - a2 );

  double ans = 0;
  if( diff_a == 0 ) {
    if( max_a <= m-2 ) {
      // \int [b'_0(x)*b'_0(x)] dx = \int [(1-u)^4]/4 dx = (1-u^2+u^3/3)/4
      ans += 0.05;
    }
    if( min_a >= 0 && max_a <= m-1 ) {
      // \int [b'_1(x)*b'_1(x)] dx = \int [9u^4 -24u^3+16u^2]/4 dx
      ans += 0.2 + 1.0 / 12;
    }
    if( min_a >= 1 && max_a <= m ) {
      // \int [b'_2(x)*b'_2(x)] dx = \int [b'_1(x)*b'_1(x)] dx
      ans += 0.2 + 1.0 / 12;
    }
    if( min_a >= 2 ) {
      // \int [b_3(x)*b_3(x)] dx = \int [b'_0(x)*b'_0(x)] dx
      ans += 0.05;
    }
    return ans;
  }

  if( diff_a == 1 ) {
    if( max_a <= m-1 ) {
      // \int [b'_0(u)*b'_1(u)] du = \int [-3u^4 + 10u^3 - 11u^2 + 4u]/4 du
      ans += -0.025 + 1.0 / 12;
    }
    if( min_a >= 0 && max_a <= m) {
      // \int [b'_1(u)*b'_2(u)] du = \int [- 9u^4 + 18u^3 - 5u^2 - 4u]/4 du
      ans += 0.175 - 5.0 / 12;
    }
    if( min_a >= 1 ) {
      // \int [b'_2(u)*b'_3(u)] du = \int [b'_0(u)*b'_1(u)] du
      ans += -0.025 + 1.0 / 12;
    }
    return ans;
  }

  if( diff_a == 2 ) {
    if( max_a <= m ) {
      // \int [b'_0(u)*b'_2(u)] du = \int [3u^4 - 8u^3 + 6u^2 - 1]/4 du
      ans += -0.1;
    }
    if( min_a >= 0 ) {
      // \int [b'_1(u)*b'_3(u)] du = \int [b'_0(u)*b'_2(u)] du
      ans += -0.1;
    }
    return ans;
  }
  
  if( diff_a == 3 ) {
    // \int [b'_0(u)*b'_3(u)] du = \int [-u^4 + 2u^3 - u^2]/4 du
    ans += 0.075 - 1.0 / 12;
    return ans;
  }

  return 0;
}

static
double
//g_double_prime( double u, int a1, int a2, int m )
g_double_prime( int a1, int a2, int m )
{
  int min_a = (a1 < a2) ? a1 : a2;
  int max_a = (a1 < a2) ? a2 : a1;
  assert( min_a >= -1 );
  assert( max_a <= m+1 );
  int diff_a = vnl_math_abs( a1 - a2 );

  double ans = 0;
  if( diff_a == 0 ) {
    if( max_a <= m-2 ) {
      // \int [b"_0(x)*b"_0(x)] dx = \int [(1-u)^2] dx = (u-u^2+u^3/3)
      ans += 1.0 / 3;
    }
    if( min_a >= 0 && max_a <= m-1 ) {
      // \int [b"_1(x)*b"_1(x)] dx = \int [9u^2 - 12u + 4] dx = 3u^3 -6u + 4
      ans += 1.0;
    }
    if( min_a >= 1 && max_a <= m ) {
      // \int [b"_2(x)*b"_2(x)] dx = \int [b"_1(x)*b"_1(x)] dx
      ans += 1.0;
    }
    if( min_a >= 2 ) {
      // \int [b"_3(x)*b"_3(x)] dx = \int [b"_0(x)*b"_0(x)] dx
      ans += 1.0 / 3;
    }
    return ans;
  }

  if( diff_a == 1 ) {
    if( max_a <= m-1 ) {
      // \int [b"_0(u)*b"_1(u)] du = \int [-3u^2 + 5u - 2] du
      ans += -0.5;
    }
    if( min_a >= 0 && max_a <= m) {
      // \int [b"_1(u)*b"_2(u)] du = \int [- 9u^2 + 9u - 2] du
      ans += -0.5;
    }
    if( min_a >= 1 ) {
      // \int [b"_2(u)*b"_3(u)] du = \int [b"_0(u)*b"_1(u)] du
      ans += -0.5;
    }
    return ans;
  }

  if( diff_a == 2 ) {
    // if( max_a <= m )
      // \int [b"_0(u)*b"_2(u)] du = 0
    // if( min_a >= 0 )
      // \int [b"_1(u)*b"_3(u)] du = 0
    return ans;
  }
  
  if( diff_a == 3 ) {
    // \int [b"_0(u)*b"_3(u)] du = \int [-u^4 + 2u3 - u^2]/4 du
    ans += 0.5 - 1.0 / 3;
    return ans;
  }

  return 0;
}

static
inline
double
refine_helper_f1( double v1, double v2)
{
  return ( v1 + v2 ) / 2;
}

static
inline
double
refine_helper_f2( double v1, double v2, double v3 )
{
  return ( v1 / 8 + v2 * 3 / 4 + v3 / 8 );
}

rgrl_spline_sptr
rgrl_spline::
refinement( vnl_vector< unsigned > const& m ) const
{
  unsigned dim = m_.size();
//  vnl_vector< unsigned > m = m_ * 2;
  rgrl_spline_sptr refined_spline = new rgrl_spline( m );
  vnl_vector< double > w( refined_spline->num_of_control_points() );

  // 0--1--2--3 ==> -0-1-2-3-4-
  if( dim == 1 ) {
    unsigned i = 0;
    for( ; i<m_[0]+2; ++i ) {
      // 2*i
      if( 2*i < m[0]+3 )
      w[2*i] = refine_helper_f1( c_[i], c_[i+1] );
      // 2*i+1
      if( i!=m_[0]+1 && 2*i+1 < m[0]+3 )
        w[2*i+1] = refine_helper_f2( c_[i], c_[i+1], c_[i+2] );
    }
  }
  else if ( dim == 2 ) {
    vcl_vector< double > tmp( m_[1] + 3, 0.0 );
    vcl_vector< vcl_vector< double > > v2( m_[0] + 1, tmp );
    vcl_vector< vcl_vector< double > > v1( m_[0] + 2, tmp );
    unsigned a = m_[0]+3;
    unsigned i=0, j=0, n=0;

    for( i=0; i<m_[0]+2; ++i ) {
      for( j=0, n=i; j<m_[1]+3; ++j, n+=a ) {
        v1[i][j] = refine_helper_f1( c_[n], c_[n+1] );
        if( i!=m_[0]+1 )
          v2[i][j] = refine_helper_f2( c_[n], c_[n+1], c_[n+2] );
      }
    }
    for( i=0; i<m_[0]+2; ++i ) {
      for( j=0; j<m_[1]+2; ++j ) {
        // 2*j
        if( 2*i < m[0]+3 && 2*j < m[1]+3 ) {
          w[2*i + 2*j*(m[0]+3)] = refine_helper_f1( v1[i][j], v1[i][j+1] );
        }
        if( i!=m_[0]+1 && 2*i+1 < m[0]+3 && 2*j < m[1]+3 ) {
          w[2*i+1 + 2*j*(m[0]+3)] = refine_helper_f1( v2[i][j], v2[i][j+1] );
        }
        // 2*j+1
        if( j!=m_[1]+1 && 2*i < m[0]+3 && 2*j+1 < m[1]+3 ) {
          w[2*i + (2*j+1)*(m[0]+3)] = refine_helper_f2( v1[i][j], v1[i][j+1], v1[i][j+2] );
        }
        if( i!=m_[0]+1 && j!=m_[1]+1 && 2*i+1 < m[0]+3 && 2*j+1 < m[1]+3 ) {
          w[2*i+1 + (2*j+1)*(m[0]+3)] = refine_helper_f2( v2[i][j], v2[i][j+1], v2[i][j+2] );
        }
      }
    }
  }
  else if ( dim == 3 ) {
    vcl_vector< double > tmp( m_[2] + 3, 0.0 );
    vcl_vector< vcl_vector< double > > tmp1( m_[1] + 3, tmp );
    vcl_vector< vcl_vector< double > > tmp2( m_[1] + 1, tmp );
    vcl_vector< vcl_vector< double > > tmp3( m_[1] + 2, tmp );
    vcl_vector< vcl_vector< vcl_vector< double > > > v2( m_[0] + 1, tmp1 );
    vcl_vector< vcl_vector< vcl_vector< double > > > v1( m_[0] + 2, tmp1 );
    vcl_vector< vcl_vector< vcl_vector< double > > > v12( m_[0]+2, tmp2 );
    vcl_vector< vcl_vector< vcl_vector< double > > > v22( m_[0]+1, tmp2 );
    vcl_vector< vcl_vector< vcl_vector< double > > > v11( m_[0]+2, tmp3 );
    vcl_vector< vcl_vector< vcl_vector< double > > > v21( m_[0]+1, tmp3 );

    unsigned b = m_[1]+3;
    unsigned a = m_[0]+3;
    unsigned ab = a * b;
    unsigned i=0, j=0, k=0, n=0;
    for( i=0; i<m_[0]+2; ++i ) {
      for( j=0; j<m_[1]+3; ++j ) {
        for( k=0, n=j*a+i; k<m_[2]+3; ++k, n+=ab ) {
          v1[i][j][k] = refine_helper_f1( c_[n], c_[n+1] );
          if( i!=m_[0]+1 )
            v2[i][j][k] = refine_helper_f2( c_[n], c_[n+1], c_[n+2] );
        }
      }
    }

    for( i=0; i<m_[0]+2; ++i ) {
      for( j=0; j<m_[1]+2; ++j ) {
        for( k=0; k<m_[2]+3; ++k ) {
          v11[i][j][k] = refine_helper_f1( v1[i][j][k], v1[i][j+1][k] );
          if( i!=m_[0]+1 )
            v21[i][j][k] = refine_helper_f1( v2[i][j][k], v2[i][j+1][k] );
          if( j!=m_[1]+1 )
            v12[i][j][k] = refine_helper_f2( v1[i][j][k], v1[i][j+1][k], v1[i][j+2][k] );
          if( i!=m_[0]+1 && j!=m_[1]+1 )
            v22[i][j][k] = refine_helper_f2( v2[i][j][k], v2[i][j+1][k], v2[i][j+2][k] );
        }
      }
    }

    for( i=0; i<m_[0]+2; ++i ) {          
      for( j=0; j<m_[1]+2; ++j ) {
        for( k=0; k<m_[2]+2; ++k ) {
          // 2*k
          if( 2*i < m[0]+3 && 2*j < m[1]+3 && 2*k < m[2]+3 )
            w[2*i + 2*j*(m[0]+3) + 2*k*(m[0]+3)*(m[1]+3)] = 
              refine_helper_f1( v11[i][j][k], v11[i][j][k+1] ) ;
          if( i!=m_[0]+1 && 2*i+1 < m[0]+3 && 2*j < m[1]+3 && 2*k < m[2]+3 )
            w[2*i+1 + 2*j*(m[0]+3) + 2*k*(m[0]+3)*(m[1]+3) ] = 
              refine_helper_f1( v21[i][j][k], v21[i][j][k+1] );
          if( j!=m_[1]+1 && 2*i < m[0]+3 && 2*j+1 < m[1]+3 && 2*k < m[2]+3 )
            w[2*i + (2*j+1)*(m[0]+3) + 2*k*(m[0]+3)*(m[1]+3)] = 
              refine_helper_f1( v12[i][j][k], v12[i][j][k+1] );
          if( i!=m_[0]+1 && j!=m_[1]+1 && 2*i+1 < m[0]+3 && 2*j+1 < m[1]+3 && 2*k < m[2]+3 )
          w[2*i+1 + (2*j+1)*(m[0]+3) + 2*k*(m[0]+3)*(m[1]+3) ] = 
            refine_helper_f1( v22[i][j][k], v22[i][j][k+1] );
          
          // 2*k+1
          if( k!=m_[2]+1 && 2*i < m[0]+3 && 2*j < m[1]+3 && 2*k+1 < m[2]+3 )
            w[2*i + 2*j*(m[0]+3) + (2*k+1)*(m[0]+3)*(m[1]+3)] = 
              refine_helper_f2( v11[i][j][k], v11[i][j][k+1], v11[i][j][k+2] );
          if( i!=m_[0]+1 && k!=m_[2]+1 && 2*i+1 < m[0]+3 && 2*j < m[1]+3 && 2*k+1 < m[2]+3 )
            w[2*i+1 + 2*j*(m[0]+3) + (2*k+1)*(m[0]+3)*(m[1]+3) ] = 
              refine_helper_f2( v21[i][j][k], v21[i][j][k+1], v21[i][j][k+2] );
          if( j!=m_[1]+1 && k!=m_[2]+1 && 2*i < m[0]+3 && 2*j+1 < m[1]+3 && 2*k+1 < m[2]+3 )
            w[2*i + (2*j+1)*(m[0]+3) + (2*k+1)*(m[0]+3)*(m[1]+3)] = 
              refine_helper_f2( v12[i][j][k], v12[i][j][k+1], v12[i][j][k+2] );
          if( i!=m_[0]+1 && j!=m_[1]+1 && k!=m_[2]+1 && 2*i+1 < m[0]+3 && 2*j+1 < m[1]+3 && 2*k+1 < m[2]+3 ) 
            w[2*i+1 +(2*j+1)*(m[0]+3) + (2*k+1)*(m[0]+3)*(m[1]+3) ] = 
              refine_helper_f2( v22[i][j][k], v22[i][j][k+1], v22[i][j][k+2] );
        }
      }
    }

//      for( i=0; i<m_[0]+2; ++i ) {          
//        for( j=0; j<m_[1]+2; ++j ) {
//          for( k=0; k<m_[2]+2; ++k ) {
//            // 2*k
//            w[2*i + 2*j*(2*m_[0]+3) + 2*k*(2*m_[0]+3)*(2*m_[1]+3)] = 
//              refine_helper_f1( v11[i][j][k], v11[i][j][k+1] ) ;
//            if( i!=m_[0]+1 )
//              w[2*i+1 + 2*j*(2*m_[0]+3) + 2*k*(2*m_[0]+3)*(2*m_[1]+3) ] = 
//                refine_helper_f1( v21[i][j][k], v21[i][j][k+1] );
//            if( j!=m_[1]+1 )
//              w[2*i + (2*j+1)*(2*m_[0]+3) + 2*k*(2*m_[0]+3)*(2*m_[1]+3)] = 
//                refine_helper_f1( v12[i][j][k], v12[i][j][k+1] );
//            if( i!=m_[0]+1 && j!=m_[1]+1 )
//            w[2*i+1 + (2*j+1)*(2*m_[0]+3) + 2*k*(2*m_[0]+3)*(2*m_[1]+3) ] = 
//              refine_helper_f1( v22[i][j][k], v22[i][j][k+1] );
          
//            // 2*k+1
//            if( k!=m_[2]+1 )
//              w[2*i + 2*j*(2*m_[0]+3) + (2*k+1)*(2*m_[0]+3)*(2*m_[1]+3)] = 
//                refine_helper_f2( v11[i][j][k], v11[i][j][k+1], v11[i][j][k+2] );
//            if( i!=m_[0]+1 && k!=m_[2]+1 )
//              w[2*i+1 + 2*j*(2*m_[0]+3) + (2*k+1)*(2*m_[0]+3)*(2*m_[1]+3) ] = 
//                refine_helper_f2( v21[i][j][k], v21[i][j][k+1], v21[i][j][k+2] );
//            if( j!=m_[1]+1 && k!=m_[2]+1 )
//              w[2*i + (2*j+1)*(2*m_[0]+3) + (2*k+1)*(2*m_[0]+3)*(2*m_[1]+3)] = 
//                refine_helper_f2( v12[i][j][k], v12[i][j][k+1], v12[i][j][k+2] );
//            if( i!=m_[0]+1 && j!=m_[1]+1 && k!=m_[2]+1 ) 
//              w[2*i+1 +(2*j+1)*(2*m_[0]+3) + (2*k+1)*(2*m_[0]+3)*(2*m_[1]+3) ] = 
//                refine_helper_f2( v22[i][j][k], v22[i][j][k+1], v22[i][j][k+2] );
//          }
//        }
//      }
  }

  vcl_cout << "rgrl_spline.cxx: refinement set_control_points: " << w << vcl_endl;
  refined_spline->set_control_points( w );
  return refined_spline;
}


vcl_ostream& 
operator<< (vcl_ostream& os, rgrl_spline const& spline )
{
  // output m(# of intervals) vector
  os << spline.m_.size() << ' ';
  os << spline.m_ << vcl_endl;  
  
  // control points
  os << spline.c_.size() << vcl_endl;
  os << spline.c_;
  
  // 
  return os;
}

vcl_istream& 
operator>> (vcl_istream& is, rgrl_spline& spline )
{
  int s;
  
  s=-1;
  is >> s;
  spline.m_.set_size( s );
  is >> spline.m_;
  
  // control points
  s=-1;
  is >> s;
  spline.c_.set_size( s );
  is >> spline.c_;
  
  return is;
}

//  bool
//  rgrl_spline::
//  is_support( vnl_vector< double > const& pt, unsigned index )
//  {
//    assert( pt.size() == m_.size() );
//    unsigned dim = pt.size();

//    for( unsigned i=0; i<m_.size(); ++i ) {
//      if( pt[i] < -3 || pt[i] >= m_[i]+3 )
//        return false;
//    }

//    vnl_vector< int > floor(dim);
//    vnl_vector< int > ceil(dim);

//    for( unsigned n=0; n<dim; ++n ) {
//      floor[n] = (int)vcl_floor( pt[n] ) ;
//      ceil[n] = (int)vcl_ceil( pt[n] );
//    }

//    // 3D case
//    if (dim == 3) {
//      int a = (m_[0]+3) * (m_[1]+3);
//      int b = m_[0]+3;

//      unsigned blk[ 3 ];
//      blk[ 0 ] = index % b;
//      blk[ 1 ] = index % a / b;
//      blk[ 2 ] = index / a;

//      for( unsigned i = 0; i < dim; ++i )
//        if( blk[i] < floor[i] || blk[i] > ceil[i]+2 ) 
//          return false;

//    }
//    // 2D case
//    else if (dim == 2) {
//      int a =(m_[0]+3);
//      unsigned blk[ 2 ];
//      blk[ 0 ] = index % a;
//      blk[ 1 ] = index / a;

//      for( unsigned i = 0; i < dim; ++i )
//        if( blk[i] < floor[i] || blk[i] > ceil[i]+2 ) 
//          return false;

//    }
//    // 1D case
//    else if (dim == 1) {
//      if( index < floor[0] || index > ceil[0]+2 ) 
//        return false;

//    }
//    return true;
//  }
