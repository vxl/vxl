// This is rpl/rsdl/rsdl_bins_2d.txx
#ifndef rsdl_bins_2d__txx_
#define rsdl_bins_2d__txx_

#include "rsdl_bins_2d.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>


template < class COORD_T, class VALUE_T >
rsdl_bins_2d< COORD_T, VALUE_T > ::
rsdl_bins_2d()
  : bins_(1,1), min_pt_(0,0), max_pt_(1,1), bin_sizes_(1,1), num_bins_x_(1), num_bins_y_(1),
    dist_tolerance_sqr_( 0 )
{
}


template < class COORD_T, class VALUE_T >
rsdl_bins_2d< COORD_T, VALUE_T > ::
rsdl_bins_2d( const vnl_vector_fixed< COORD_T, 2 > & min_pt,
              const vnl_vector_fixed< COORD_T, 2 > & max_pt,
              const vnl_vector_fixed< COORD_T, 2 > & bin_sizes )
  : min_pt_( min_pt ), max_pt_( max_pt ), bin_sizes_( bin_sizes ), dist_tolerance_sqr_( 0 )
{
  assert( min_pt_[0] <= max_pt_[0] );
  assert( min_pt_[1] <= max_pt_[1] );

  num_bins_x_ = int( vcl_ceil( double( max_pt_[0] - min_pt_[0] ) / bin_sizes_[0] ) );
  num_bins_y_ = int( vcl_ceil( double( max_pt_[1] - min_pt_[1] ) / bin_sizes_[1] ) );

  bins_.resize( num_bins_x_, num_bins_y_ );
}


template < class COORD_T, class VALUE_T >
rsdl_bins_2d< COORD_T, VALUE_T > ::
~rsdl_bins_2d()
{
}

template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
reset( const vnl_vector_fixed< COORD_T, 2 > & min_pt,
       const vnl_vector_fixed< COORD_T, 2 > & max_pt,
       const vnl_vector_fixed< COORD_T, 2 > & bin_sizes )
{
  min_pt_ = min_pt;
  max_pt_ = max_pt;
  bin_sizes_ = bin_sizes;

  assert( min_pt_[0] <= max_pt_[0] );
  assert( min_pt_[1] <= max_pt_[1] );

  num_bins_x_ = int( vcl_ceil( double( max_pt_[0] - min_pt_[0] ) / bin_sizes_[0] ) );
  num_bins_y_ = int( vcl_ceil( double( max_pt_[1] - min_pt_[1] ) / bin_sizes_[1] ) );

  bins_.resize( 2, 2);    //needed because the next line does not clear out the previous bins
                          //if the previous bins are the same size....KHF 8/6/01
  bins_.resize( num_bins_x_, num_bins_y_ );  // rely on this clearing out the previous bins
}

template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
remove_all_points( )
{
  for ( int bin_x = 0; bin_x < num_bins_x_; ++ bin_x ) {
    for ( int bin_y = 0; bin_y < num_bins_y_; ++ bin_y ) {
      bins_ (bin_x, bin_y).clear ();
    }
  }
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
add_point( const vnl_vector_fixed< COORD_T, 2 > & pt,
           const VALUE_T & value )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );
  bins_( bin_x, bin_y ).push_back( bin_entry_type_ ( pt, value ) );
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
get_value( const vnl_vector_fixed< COORD_T,  2 > & pt, VALUE_T& value )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );

  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( pt[0]-distance, pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( pt[0]+distance, pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( pt, p->point_ ) <= dist_tolerance_sqr_ ) {
          value = p->value_;
          return true;
        }

  return false;
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
change_point( const vnl_vector_fixed< COORD_T, 2 > & pt,
              const VALUE_T & value )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );

  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( pt[0]-distance, pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( pt[0]+distance, pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( pt, p->point_ ) <= dist_tolerance_sqr_ ) {
          p->value_ = value;
          return true;
        }

  return false;
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
change_point( const vnl_vector_fixed< COORD_T, 2 > & pt,
              const VALUE_T& old_val,
              const VALUE_T& new_val )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );

  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( pt[0]-distance, pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( pt[0]+distance, pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( pt, p->point_ ) <= dist_tolerance_sqr_  &&
             p->value_ == old_val) {
          p->value_ = new_val;
          return true;
        }

  return false;
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
remove_point( const vnl_vector_fixed< COORD_T, 2 > & pt )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );

  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( pt[0]-distance, pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( pt[0]+distance, pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( pt, p->point_ ) <= dist_tolerance_sqr_ ) {
          bins_( bin_x, bin_y ).erase( p );
          return true;
        }

  return false;
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
remove_point( const vnl_vector_fixed< COORD_T, 2 > & pt,
              const VALUE_T & value )
{
  int bin_x, bin_y;
  this->point_to_bin( pt[0], pt[1], bin_x, bin_y );

  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( pt[0]-distance, pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( pt[0]+distance, pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( pt, p->point_ ) <= dist_tolerance_sqr_
             && p->value_ == value ) {
          bins_( bin_x, bin_y ).erase( p );
          return true;
        }

  return false;
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
n_nearest( const vnl_vector_fixed< COORD_T, 2 >& query_pt,
           int n,
           vcl_vector< VALUE_T >& values ) const
{
  vcl_vector< vnl_vector_fixed< COORD_T, 2 > > points;
  n_nearest( query_pt, n, points, values );
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
n_nearest( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
           int n,
           vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
           vcl_vector< VALUE_T > & values ) const
{
  assert( n >= 1 );
  points.clear();
  values.clear();
  vcl_vector< double > sq_distances( n );

  int num_found = 0;

  int c_bin_x, c_bin_y;
  this->point_to_bin( query_pt[0], query_pt[1], c_bin_x, c_bin_y );

  int infinity_norm_dist = 0;

  vcl_vector< int > bin_xs, bin_ys;
  bin_xs.push_back( c_bin_x );
  bin_ys.push_back( c_bin_y );

  bool still_testing;

  do {
    still_testing = false;
    for ( unsigned int i=0; i<bin_xs.size(); ++ i ) {
      if ( num_found < n ||
           this->min_sq_distance_to_bin( query_pt[0], query_pt[1], bin_xs[i], bin_ys[i] )
             < sq_distances[ num_found - 1 ] ) {
        still_testing = true;
        for (typename bin_vector_type_::const_iterator p = bins_( bin_xs[i], bin_ys[i] ).begin();
               p != bins_( bin_xs[i], bin_ys[i] ).end(); ++p )
        {
          COORD_T sq_d = vnl_vector_ssd( query_pt, p->point_ );

          if ( num_found < n || sq_d < sq_distances[ num_found- 1] ) {
            if ( num_found < n ) {
              points.push_back( p -> point_ );  // do this to ensure enough space
              values.push_back( p -> value_ );  // do this to ensure enough space
              ++ num_found ;
            }

            int j = num_found-2;
            while ( j >= 0 && sq_d < sq_distances[ j ] ) {
              points[ j+1 ] = points[ j ];
              values[ j+1 ] = values[ j ];
              sq_distances[ j+1 ] = sq_distances[ j ];
              -- j;
            }
            points[ j+1 ] = p -> point_;
            values[ j+1 ] = p -> value_;
            sq_distances[ j+1 ] = sq_d;
          }
        }
      }
    }

    if ( still_testing )
    {
      infinity_norm_dist ++ ;
      bin_xs.clear(); bin_ys.clear();

      // make list of bins to try
      int lower_x_for_y = vnl_math_max( c_bin_x - infinity_norm_dist, 0 );
      int upper_x_for_y = vnl_math_min( c_bin_x + infinity_norm_dist, num_bins_x_-1 );

      // across top (when origin is at upper left)
      if ( c_bin_y - infinity_norm_dist >= 0 ) {
        for ( int bin_x = lower_x_for_y; bin_x <= upper_x_for_y; ++ bin_x ) {
          bin_xs.push_back( bin_x );
          bin_ys.push_back( c_bin_y - infinity_norm_dist );
        }
      }

      // across bottom (when origin is at upper left)
      if ( c_bin_y + infinity_norm_dist < num_bins_y_ ) {   // haven't fallen off bottom
        for ( int bin_x = lower_x_for_y; bin_x <= upper_x_for_y; ++ bin_x ) {
          bin_xs.push_back( bin_x );
          bin_ys.push_back( c_bin_y + infinity_norm_dist );
        }
      }

      int lower_y_for_x = vnl_math_max( c_bin_y - infinity_norm_dist+1, 0 );
      int upper_y_for_x = vnl_math_min( c_bin_y + infinity_norm_dist-1, num_bins_y_-1 );

      // across left
      if ( c_bin_x - infinity_norm_dist >= 0 ) {   // haven't fallen off left edge
        for ( int bin_y = lower_y_for_x;  bin_y <= upper_y_for_x; ++ bin_y ) {
          bin_xs.push_back( c_bin_x - infinity_norm_dist );
          bin_ys.push_back( bin_y );
        }
      }

      // across right
      if ( c_bin_x + infinity_norm_dist < num_bins_x_ ) {   // haven't fallen off right edge
        for ( int bin_y = lower_y_for_x;  bin_y <= upper_y_for_x; ++ bin_y ) {
          bin_xs.push_back( c_bin_x + infinity_norm_dist );
          bin_ys.push_back( bin_y );
        }
      }
    }
  } while ( still_testing );
}

template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
is_any_point_within_radius( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                            COORD_T radius ) const
{
  radius += vcl_sqrt(dist_tolerance_sqr_);
  COORD_T sq_radius = radius * radius;

  int min_bin_x, min_bin_y;
  this->point_to_bin( query_pt[0] - radius, query_pt[1] - radius, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( query_pt[0] + radius, query_pt[1] + radius, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for ( typename bin_vector_type_::const_iterator p = bins_(bin_x,bin_y).begin();
            p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( query_pt, p->point_ ) < sq_radius )
          return true;

  return false;
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
points_within_radius( const vnl_vector_fixed< COORD_T,  2> & query_pt,
                      COORD_T radius,
                      vcl_vector< VALUE_T > & values ) const
{
  vcl_vector< vnl_vector_fixed< COORD_T, 2 > > points;
  points_within_radius( query_pt, radius, points, values );
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
points_within_radius( const vnl_vector_fixed< COORD_T, 2 > & query_pt,
                      COORD_T radius,
                      vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
                      vcl_vector< VALUE_T > & values ) const
{
  radius += vcl_sqrt(dist_tolerance_sqr_);
  COORD_T sq_radius = radius * radius;

  int min_bin_x, min_bin_y;
  this->point_to_bin( query_pt[0] - radius, query_pt[1] - radius, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( query_pt[0] + radius, query_pt[1] + radius, max_bin_x, max_bin_y );

  points.clear();
  values.clear();

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for ( typename bin_vector_type_::const_iterator p = bins_(bin_x,bin_y).begin();
            p != bins_( bin_x, bin_y ).end(); ++p )
        if ( vnl_vector_ssd( query_pt, p->point_ ) < sq_radius ) {
          points.push_back( p->point_ );
          values.push_back( p->value_ );
        }
}


template < class COORD_T, class VALUE_T >
bool
rsdl_bins_2d< COORD_T, VALUE_T > ::
is_any_point_in_bounding_box( const vnl_vector_fixed<COORD_T,2>& min_query_pt,
                              const vnl_vector_fixed<COORD_T,2>& max_query_pt) const
{
  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( min_query_pt[0]-distance, min_query_pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( max_query_pt[0]+distance, max_query_pt[1]+distance, max_bin_x, max_bin_y );

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::const_iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( min_query_pt[0] <= p->point_[0] && p->point_[0] <= max_query_pt[0] &&
             min_query_pt[1] <= p->point_[1] && p->point_[1] <= max_query_pt[1] )
          return true;

  return false;
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
points_in_bounding_box( const vnl_vector_fixed<COORD_T,2>& min_query_pt,
                        const vnl_vector_fixed<COORD_T,2>& max_query_pt,
                        vcl_vector< VALUE_T >& values ) const
{
  vcl_vector< vnl_vector_fixed< COORD_T, 2 > > points;
  points_in_bounding_box( min_query_pt, max_query_pt, points, values );
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
points_in_bounding_box( const vnl_vector_fixed< COORD_T, 2 > & min_query_pt,
                        const vnl_vector_fixed< COORD_T, 2 > & max_query_pt,
                        vcl_vector< vnl_vector_fixed< COORD_T, 2 > > & points,
                        vcl_vector< VALUE_T > & values ) const
{
  COORD_T distance = vcl_sqrt(dist_tolerance_sqr_);
  int min_bin_x, min_bin_y;
  this->point_to_bin( min_query_pt[0]-distance, min_query_pt[1]-distance, min_bin_x, min_bin_y );

  int max_bin_x, max_bin_y;
  this->point_to_bin( max_query_pt[0]+distance, max_query_pt[1]+distance, max_bin_x, max_bin_y );

  points.clear();
  values.clear();

  for ( int bin_x = min_bin_x; bin_x <= max_bin_x; ++ bin_x )
    for ( int bin_y = min_bin_y; bin_y <= max_bin_y; ++ bin_y )
      for (typename bin_vector_type_::const_iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( min_query_pt[0] <= p->point_[0] && p->point_[0] <= max_query_pt[0] &&
             min_query_pt[1] <= p->point_[1] && p->point_[1] <= max_query_pt[1] ) {
          points.push_back( p->point_ );
          values.push_back( p->value_ );
        }
}


template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
point_to_bin( COORD_T x, COORD_T y, int& bin_x, int& bin_y ) const
{
  bin_x = int( ( x - min_pt_[0] ) / bin_sizes_[0] );

  if ( bin_x < 0 )
    bin_x = 0;
  else if ( bin_x >= num_bins_x_ )
    bin_x = num_bins_x_ - 1;

  bin_y = int( ( y - min_pt_[1] ) / bin_sizes_[1] );

  if ( bin_y < 0 )
    bin_y = 0;
  else if ( bin_y >= num_bins_y_ )
    bin_y = num_bins_y_ - 1;
}


template < class COORD_T, class VALUE_T >
COORD_T
rsdl_bins_2d< COORD_T, VALUE_T > ::
min_sq_distance_to_bin( COORD_T x, COORD_T y, int bin_x, int bin_y ) const
{
  COORD_T sq_dist = 0;

  COORD_T min_x = min_pt_[0] + bin_x * bin_sizes_[0];
  COORD_T max_x = min_pt_[0] + (bin_x + 1) * bin_sizes_[0];  // not right for COORD_T == int

  if ( x < min_x )
    sq_dist += vnl_math_sqr( min_x - x );
  else if ( x > max_x )
    sq_dist += vnl_math_sqr( x - max_x );

  COORD_T min_y = min_pt_[1] + bin_y * bin_sizes_[1];
  COORD_T max_y = min_pt_[1] + (bin_y + 1) * bin_sizes_[1];  // not right for COORD_T == int

  if ( y < min_y )
    sq_dist += vnl_math_sqr( min_y - y );
  else if ( y > max_y )
    sq_dist += vnl_math_sqr( y - max_y );

  return sq_dist;
}

template < class COORD_T, class VALUE_T >
void
rsdl_bins_2d< COORD_T, VALUE_T > ::
change_value( const VALUE_T& old_val, const VALUE_T& new_val )
{
  for ( int bin_x = 0; bin_x < num_bins_x_; ++ bin_x )
    for ( int bin_y = 0; bin_y < num_bins_y_; ++ bin_y )
      for (typename bin_vector_type_::iterator p = bins_(bin_x,bin_y).begin();
           p != bins_( bin_x, bin_y ).end(); ++p )
        if ( p->value_ == old_val )
          p->value_ = new_val;
}


template < class COORD_T, class VALUE_T >
vcl_ostream& operator<< ( vcl_ostream& ostr,
                          const vcl_vector< rsdl_bins_2d_entry< COORD_T, VALUE_T > > & entries )
{
  for ( unsigned int i=0; i<entries.size(); ++ i )
    ostr << i << ":  point = " << entries[i].point_ << ", value = " << entries[i].value_ << '\n';
  return ostr;
}


#define RSDL_BINS_2D_INSTANTIATE( COORD_T, VALUE_T ) \
template class rsdl_bins_2d_entry< COORD_T, VALUE_T >; \
template class rsdl_bins_2d< COORD_T, VALUE_T >; \
template vcl_ostream& \
operator<< ( vcl_ostream& ostr, \
             const vcl_vector< rsdl_bins_2d_entry< COORD_T, VALUE_T > > & entry )

#endif // rsdl_bins_2d__txx_
