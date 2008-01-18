#ifndef rsdl_bins_txx_
#define rsdl_bins_txx_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rsdl_bins.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>


//:
// This class is used in n_nearest_impl. It stores a pointer to an
// entry (= (location,value) pair), and a distance to that entry from
// the query point. It supports ordering by ascending distance.
//
template<unsigned N, typename CoordType, typename ValueType>
struct rsdl_bins_point_dist_entry
{
  typedef rsdl_bins<N,CoordType,ValueType>                 bin_class;
  typedef rsdl_bins_bin_entry_type<N,CoordType,ValueType>  bin_entry_type;
  typedef typename bin_class::coord_type                   coord_type;
  typedef typename bin_class::point_type                   point_type;

  rsdl_bins_point_dist_entry( point_type const& query_pt,
                              bin_entry_type const* entry );

  inline bool operator<( rsdl_bins_point_dist_entry const& other ) const;

  bin_entry_type const* entry_;
  coord_type            sqr_dist_;
};


// ---------------------------------------------------------------------------
//                                                                   rsdl bins

template<unsigned N, typename C, typename V>
rsdl_bins<N,C,V>::
rsdl_bins( point_type const& min_coord,
           point_type const& max_coord,
           point_type const& bin_sizes )
{
  dist_tol_ = 0.0;

  int total_size = 1;
  for ( unsigned i=0; i < N; ++i ) {
    bin_size_[i] = bin_sizes[i];
    min_pt_[i] = min_coord[i];
    size_[i] = coord_to_bin( max_coord[i], i ) + 1;
    assert( size_[i] > 0 );
    total_size *= size_[i];
  }

  bins_.resize( total_size );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
set_distance_tolerance( coord_type const& tol )
{
  dist_tol_ = tol;
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
clear()
{
  typedef typename vcl_vector< bin_type >::iterator vec_iter;

  for ( vec_iter i = bins_.begin(); i != bins_.end(); ++i ) {
    i->clear();
  }
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
add_point( point_type const& pt, value_type const& val )
{
  bins_[ bin_index( pt ) ].push_back( bin_entry_type( pt, val ) );
}


template<unsigned N, typename C, typename V>
bool
rsdl_bins<N,C,V>::
get_value( point_type const& pt, value_type& val ) const
{
  typedef typename bin_index_vector::iterator ind_iter;
  typedef typename bin_type::const_iterator entry_iter;

  coord_type dist_tol_sqr = dist_tol_*dist_tol_;

  // Look through each entry in each possible bin and return the first
  // entry found.
  //
  bin_index_vector indices = bin_indices( pt );
  for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
    bin_type const& bin = bins_[*bi];
    for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
      if ( ei->equal( pt, dist_tol_sqr ) ) {
        val = ei->value_;
        return true;
      }
    }
  }

  // Point not found.
  //
  return false;
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest( point_type const& pt,
           unsigned n,
           vcl_vector< value_type >& values ) const
{
  n_nearest_impl( pt, n, values, 0 );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest( point_type const& pt,
           unsigned n,
           vcl_vector< point_type >& points,
           vcl_vector< value_type >& values  ) const
{
  n_nearest_impl( pt, n, values, &points );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest_exhaustive( point_type const& pt,
                      unsigned n,
                      vcl_vector< value_type >& values ) const
{
  n_nearest_exhaustive_impl( pt, n, values, 0 );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest_exhaustive( point_type const& pt,
                      unsigned n,
                      vcl_vector< point_type >& points,
                      vcl_vector< value_type >& values  ) const
{
  n_nearest_exhaustive_impl( pt, n, values, &points );
}


template<unsigned N, typename C, typename V>
bool
rsdl_bins<N,C,V>::
is_any_point_within_radius( point_type const& pt,
                            coord_type const& radius ) const
{
  // FIXME: re-implement points_within_radius_impl here, without the
  // push_backs...

  vcl_vector< value_type > values;
  points_within_radius( pt, radius, values );

  return ! values.empty();
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_within_radius( point_type const& pt,
                      coord_type const& radius,
                      vcl_vector< value_type >& values ) const
{
  points_within_radius_impl( pt, radius, values, 0 );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_within_radius( point_type const& pt,
                      coord_type const& radius,
                      vcl_vector< point_type >& points,
                      vcl_vector< value_type >& values  ) const
{
  points_within_radius_impl( pt, radius, values, &points );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_in_bounding_box( point_type const& min_pt,
                        point_type const& max_pt,
                        vcl_vector< value_type >& values ) const
{
  points_in_bounding_box_impl( min_pt, max_pt, values, 0 );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_in_bounding_box( point_type const& min_pt,
                        point_type const& max_pt,
                        vcl_vector< point_type >& points,
                        vcl_vector< value_type >& values  ) const
{
  points_in_bounding_box_impl( min_pt, max_pt, values, &points );
}


// ---------------------------------------------------------------------------
//                                                rsdl bins internal functions

template<unsigned N, typename C, typename V>
int
rsdl_bins<N,C,V>::
coord_to_bin( coord_type x, unsigned d ) const
{
  return int( vcl_floor( ( x - min_pt_[d] ) / bin_size_[d] ) );
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
point_to_bin( point_type const& pt, int ind[N] ) const
{
  for ( unsigned d=0; d < N; ++d ) {
    ind[d] = coord_to_bin( pt[d], d );
    if ( ind[d] < 0 ) {
      ind[d] = 0;
    } else if ( ind[d] >= size_[d] ) {
      ind[d] = size_[d] - 1;
    }
  }
}


template<unsigned N, typename C, typename V>
typename rsdl_bins<N,C,V>::bin_index_type
rsdl_bins<N,C,V>::
bin_index( point_type const& pt ) const
{
  bin_index_type index = 0;
  bin_index_type skip = 1;

  for ( unsigned d=0; d < N; ++d ) {
    int i = coord_to_bin( pt[d], d );
    if ( i < 0 ) {
      i = 0;
    } else if ( i >= size_[d] ) {
      i = size_[d] - 1;
    }
    index += i*skip;
    skip *= size_[d];
  }

  return index;
}


template<unsigned N, typename C, typename V>
typename rsdl_bins<N,C,V>::bin_index_type
rsdl_bins<N,C,V>::
bin_index( int bin[N] ) const
{
  bin_index_type index = 0;
  bin_index_type skip = 1;

  for ( unsigned d=0; d < N; ++d ) {
    assert( 0 <= bin[d] && bin[d] < size_[d] );
    index += bin[d]*skip;
    skip *= size_[d];
  }

  return index;
}


template<unsigned N, typename C, typename V>
typename rsdl_bins<N,C,V>::bin_index_vector
rsdl_bins<N,C,V>::
bin_indices( point_type const& pt ) const
{
  bin_index_vector indices;

  int bin_lo[N], bin_hi[N];
  point_to_bin( pt-dist_tol_, bin_lo );
  point_to_bin( pt+dist_tol_, bin_hi );

  if ( bin_lo == bin_hi ) {
    indices.push_back( bin_index( bin_lo ) );
  } else {
    int cur[N];
    scan_region( bin_lo, bin_hi, cur, 0, indices );
  }

  return indices;
}

template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
closest_face ( point_type const& pt,
               int bin_lo[N],
               int bin_hi[N],
               unsigned * face_dim,
               unsigned * face_dir,
               coord_type * face_dist,
               bool & face_inf_dist) const
{
  face_inf_dist = true;         // init dist to closest face as infinity
  coord_type best_dist;         // need local copy of best distance since face_dist might be NULL

  // for each dimension
  for( unsigned dim=0; dim<N; ++dim ) {
    // for each direction
    for( unsigned dir=0; dir<2; ++dir ) {

      // faces at the extremes are treated as faces with infinite distance
      // since they cannot be closer than infinity, we just skip them
      if( dir == 0 )
        if( bin_lo[dim] == 0 )
          continue;
      if( dir == 1 )
        if( bin_hi[dim] == ( size_[dim] - 1 ) )
          continue;

      coord_type face_x;  // coordinate of the this face in this dimension

      if( dir == 0 )
        face_x = bin_lo[dim] * bin_size_[dim] + min_pt_[dim];
      else
        face_x = ( bin_hi[dim] + 1 ) * bin_size_[dim] + min_pt_[dim];

      coord_type dist = vcl_fabs ( pt[dim] - face_x );
      if ( face_inf_dist || dist < best_dist ) {
        if(face_dim) *face_dim = dim;
        if(face_dir) *face_dir = dir;
        if(face_dist) *face_dist = dist;
        face_inf_dist = false;
        best_dist = dist;
      }
    }
  }
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest_impl( point_type const& pt,
                unsigned n,
                vcl_vector< value_type >& values,
                vcl_vector< point_type >* points ) const
{
  bool done = false;
  coord_type nth_dist_sqr = 0;  // squared distance to nth closest point found

  // is the set of bins searched empty?
  bool bin_rng_empty = true;
  // range of bins searched
  int bin_rng_lo[N], bin_rng_hi[N];
  // new portion of the search range
  int bin_new_lo[N], bin_new_hi[N];

  // the n closest points, with their distances to the test point
  vcl_vector< point_dist_entry > distances;

  while( ! done ) {

    // STEP 1: Determine whether we need to search more bins, either
    // because we have not yet found enough points or there are still
    // bins that may contain points closer than the nth closest point
    // found so far.

    bool add_new_bins = false;

    if ( distances.size() < n ) {

      add_new_bins = true;

    }
    else {

      // sort the points we have found so far
      // we only need the closest n points
      vcl_nth_element( distances.begin(),
                       distances.begin() + n,
                       distances.end() );

      // remove all points that are further away than the nth closest point
      if( distances.size() > n )
        distances.erase( distances.begin() + n, distances.end() );
      assert ( distances.size() == n );

      // squared distance to the nth closest point
      nth_dist_sqr = distances[n-1].sqr_dist_;

      coord_type face_dist;     // not-squared distance to closest side of rectangular region of bins searched
      bool face_inf_dist;       // is this distance infinite?
      closest_face (pt, bin_rng_lo, bin_rng_hi,
                    0, 0, &face_dist, face_inf_dist);

      if( face_inf_dist || nth_dist_sqr <= face_dist*face_dist )
        done = true;
      else
        add_new_bins = true;

    }

    // STEP 2: Add new bins.  If the search range is empty add a
    // single bin.  Otherwise, expand one side of the rectangular
    // search range, choosing whichever side is closest to the point.

    bool found_new_bins = false; // were we able to add new bins to the search region?

    if( add_new_bins ) {

      if( bin_rng_empty ) {

        // the bin search range is empty, so initialize to single bin
        point_to_bin( pt, bin_rng_lo );
        for (unsigned i=0; i<N; ++i) bin_rng_hi[i] = bin_rng_lo[i];
        bin_rng_empty = false;

        // the entire bin search range is newly added
        for (unsigned i=0; i<N; ++i) bin_new_lo[i] = bin_rng_lo[i];
        for (unsigned i=0; i<N; ++i) bin_new_hi[i] = bin_rng_hi[i];
        found_new_bins = true;

      }
      else {

        // add bins to search region by extending the closest face

        unsigned face_dim;
        unsigned face_dir;
        bool face_inf_dist;

        closest_face (pt, bin_rng_lo, bin_rng_hi,
                      &face_dim, &face_dir, 0, face_inf_dist);

        if( ! face_inf_dist ) {

          // We have found the closest face.  Expand the bin search
          // range and keep tabs on what is the newly added part of
          // the search range.

          for (unsigned i=0; i<N; ++i) bin_new_lo[i] = bin_rng_lo[i];
          for (unsigned i=0; i<N; ++i) bin_new_hi[i] = bin_rng_hi[i];

          assert( face_dim < N );

          if( face_dir == 0 ) {
            bin_rng_lo[face_dim] -= 1;
            bin_new_lo[face_dim] = bin_rng_lo[face_dim];
            bin_new_hi[face_dim] = bin_rng_lo[face_dim];
          }
          else {
            bin_rng_hi[face_dim] += 1;
            bin_new_lo[face_dim] = bin_rng_hi[face_dim];
            bin_new_hi[face_dim] = bin_rng_hi[face_dim];
          }

          found_new_bins = true;

        }

      }
    }

    // STEP 3: Add new points if we added new bins.

    if( found_new_bins ) {

      // Get the list of points in the new candidate bins and
      // compute their distance from the query point.

      // stores the indices of the new candidate bins.
      bin_index_vector indices;

      int cur[N];   // bin index, used by recursive iterator, needs no initializtion
      scan_region( bin_new_lo, bin_new_hi, cur, 0, indices );

      typedef typename bin_index_vector::iterator ind_iter;
      typedef typename bin_type::const_iterator entry_iter;

      for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
        bin_type const& bin = bins_[*bi];
        for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
          distances.push_back( point_dist_entry( pt, &(*ei) ) );
        }
      }

    }

    // STEP 4: If we wanted to add new bins to the search range, but
    // we did not find any bins to add, then we are done.  This will
    // always happen if there are fewer than n points in the data
    // structure.  (It could also happen when there are n or more
    // points, depending on the point distribution.)

    if( add_new_bins && ! found_new_bins ) {
      done = true;
    }

  }

  // sort the points by distance
  // if ( distances.size() < n )
  //   vcl_sort( distances.begin(), distances.end() );

  // Copy into the output structures.

  typedef typename vcl_vector< point_dist_entry >::iterator point_dist_iter;

  assert( distances.size() <= n );

  for ( point_dist_iter i = distances.begin(); i != distances.end(); ++i ) {
    values.push_back( i->entry_->value_ );
    if ( points ) {
      points->push_back( i->entry_->point_ );
    }
  }

}

#if 0

// previous implementation of n_nearest_impl - has some subtle bugs
// and will be removed

template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest_impl( point_type const& pt,
                unsigned n,
                vcl_vector< value_type >& values,
                vcl_vector< point_type >* points ) const
{
  // !!!!!!!!! BUG Notice !!!!!!!!!!
  //
  // The following implementation of N nearest neighbors did
  // not consider the situation where after found >=n candidates,
  // there can still exist a bin further away, but contains points
  // close than some of these candidates.
  //
  // !!!!!!! FIX Notice !!!!!!!
  //

  // Let N be the dimension of the space, and s be the bin size
  // (length of one side).  Then the length of the diagonal of a bin
  // is
  //   d = sqrt(N*s*s) = s*sqrt(N)
  // Now, if we grow an N-dimensional region in all directions by k
  // bins, the farthest possible distance between the given point and
  // its nearest neighbor in this region will be d' = (k+1) * d.  To
  // make sure we include all bins that may contain a point closer
  // than this, we will additionally increase our region by f
  // number of bins in all dimensions, where
  //  f = ceil ((d' - (k * s)) / s)
  // There might be more optimal solution to this fix.

  static bool rsdl_bins_bug_warning_s = false;
  if( rsdl_bins_bug_warning_s ) {
    vcl_cerr << "Warning: results from current rsdl_bins<N,C,V>::n_nearest_impl"
      << "may be inaccurate.  Please contact developers for details. " << vcl_endl;
    rsdl_bins_bug_warning_s = false;
  }

  // 1. Find the list of bins with candidate points. The candidate
  // bins will collectively hold at least n points to test. Points in
  // non-candidate bins will be further away that all points in the
  // candidate bins.
  //
  // The idea is to repeatedly bins from an ever growning
  // (square) "circle" until we have enough bins.

  // stores the indices of the candidate bins.
  bin_index_vector indices;

  // dimensions of the box currently being examined.
  int bin_lo[N], bin_hi[N], cur[N];

  point_to_bin( pt, bin_lo );
  for (unsigned int i=0; i<N; ++i) bin_hi[i] = bin_lo[i];

  bool still_looking = true;
  unsigned found = 0;
  do {
    found += scan_bdy( bin_lo, bin_hi, cur, 0, indices );
    if ( found >= n ) {
      // We found the requested number of points, now make sure to
      // include all bins that might contain points that are closer
      // than the ones we found so far as described in the FIX notes
      // above.
      unsigned k = (bin_hi[0] - bin_lo[0]) / 2;
      // find number of bins needed to expand the region
      unsigned f = unsigned( vcl_ceil( double(k+1) * vcl_sqrt(double(N)) - double(k) ) );
      for (unsigned j=0; j<f; ++j) {
        for (unsigned i=0; i<N; ++i) {
          // increase the region one bin at a time
          --bin_lo[i];
          ++bin_hi[i];
        }
        found += scan_bdy( bin_lo, bin_hi, cur, 0, indices );
      }
      still_looking = false;
    } else {
      bool some_dimension_in_bounds = false;
      for ( unsigned i=0; i < N; ++i ) {
        --bin_lo[i];
        ++bin_hi[i];
        if ( bin_lo[i] >= 0 || bin_hi[i] < size_[i] )
          some_dimension_in_bounds = true;
      }
      // we've searched the whole space, and didn't find anything.
      if ( !some_dimension_in_bounds ) {
        still_looking = false;
      }
    }
  } while ( still_looking );

  // 2. Get the list of points in the candidate bins and compute their
  // distance from the query point.
  //
  typedef typename bin_index_vector::iterator ind_iter;
  typedef typename bin_type::const_iterator entry_iter;

  vcl_vector< point_dist_entry > distances;

  for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
    bin_type const& bin = bins_[*bi];
    for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
      distances.push_back( point_dist_entry( pt, &(*ei) ) );
    }
  }

  // 3. Extract the n-closest points and copy into the output
  // structures.
  //
  typedef typename vcl_vector< point_dist_entry >::iterator point_dist_iter;

  point_dist_iter mid;
  if ( distances.size() > n ) {
    mid = distances.begin() + n;
    vcl_partial_sort( distances.begin(), mid, distances.end() );
  } else {
    mid = distances.end();
  }

#define support_points_outside_boundaries 0
#if support_points_outside_boundaries

  // Check if any of the distances are greater than the dimensions of
  // the region that was searched.  This would cover points that are
  // located ouside min-max boundaries.  If a point is located outside
  // bin region, it would be placed into a bin on the boundary of the
  // region. As a result of this projection the distance information
  // would be lost and we have to search area that includes the
  // point's real location.

  // get the longest distance between a given point and points that were found
  double longest_distance = vcl_sqrt(double((mid-1)->sqr_dist_));

  // find dimension of the region that has been searched
  double r = ((bin_hi[0] - bin_lo[0]) / 2) * bin_size_[0];

  // check if we have a point outside this searched region
  if (longest_distance > r) {
    unsigned reg_incr = unsigned(vcl_ceil(longest_distance / bin_size_[0] - (bin_hi[0] - bin_lo[0])/2));
    for (unsigned j=0; j<reg_incr; ++j) {
      for (unsigned i=0; i<N; ++i) {
        // increase the region one bin at a time
        --bin_lo[i];
        ++bin_hi[i];
      }
      found += scan_bdy( bin_lo, bin_hi, cur, 0, indices );
    }
    // repeat above steps 2 and 3 here to compute distances again
    distances.clear();
    for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
      bin_type const& bin = bins_[*bi];
      for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
        distances.push_back( point_dist_entry( pt, &(*ei) ) );
      }
    }
    // sort points by their distance
    if ( distances.size() > n ) {
      mid = distances.begin() + n;
      vcl_partial_sort( distances.begin(), mid, distances.end() );
    }
    else {
      mid = distances.end();
    }
  }

#endif  // end of support points outside region boundaries logic

  for ( point_dist_iter i = distances.begin(); i != mid; ++i ) {
    values.push_back( i->entry_->value_ );
    if ( points ) {
      points->push_back( i->entry_->point_ );
    }
  }
}

#endif

template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
n_nearest_exhaustive_impl( point_type const& pt,
                           unsigned n,
                           vcl_vector< value_type >& values,
                           vcl_vector< point_type >* points ) const
{
  typedef typename vcl_vector< bin_type >::const_iterator vec_iter;
  typedef typename bin_type::const_iterator entry_iter;
  typedef typename vcl_vector< point_dist_entry >::iterator point_dist_iter;

  vcl_vector< point_dist_entry > distances;
  coord_type nth_dist_sqr = 0;

  // for every bin
  for ( vec_iter bi = bins_.begin(); bi != bins_.end(); ++bi ) {
    bin_type const & bin = *bi;
    // for every entry in the bin
    for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
      bin_entry_type const & entry = *ei;

      // if we don't yet have the requested n points, or this point is
      // closer than the nth closest point found so far
      coord_type dist_sqr = vnl_vector_ssd( pt, ei->point_ );
      if ( distances.size() < n
           || vnl_vector_ssd( pt, ei->point_ ) < nth_dist_sqr ) {

        distances.push_back( point_dist_entry( pt, &(*ei) ) );

        if ( distances.size() >= n )
          vcl_nth_element ( distances.begin(),
                            distances.begin() + n,
                            distances.end() );

        // if we now have more than n points, get rid of the extra
        // there could only be 1 extra point here
        if ( distances.size() > n ) {
          distances.pop_back();
          // is size was >n then it must have been n+1
          assert ( distances.size() == n );
        }

        // if we have found n points, find the distance to the nth
        // point because that will be the new criteria for adding new
        // points
        if ( distances.size() == n )
          nth_dist_sqr = distances[n-1].sqr_dist_;
      }
    }
  }

  // sort the points by distance
  // if ( distances.size() < n )
  //   vcl_sort( distances.begin(), distances.end() );

  // copy points to the output structures
  for ( point_dist_iter i = distances.begin(); i != distances.end(); ++i ) {
    values.push_back( i->entry_->value_ );
    if ( points ) {
      points->push_back( i->entry_->point_ );
    }
  }

}

template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_within_radius_impl( point_type const& pt,
                           coord_type const& radius,
                           vcl_vector< value_type >& values,
                           vcl_vector< point_type >* points ) const
{
  // 1. Find the list of bins that may contain the points we
  // want. These are the bins overlapping
  // [pt-radius*vec(1), pt+radius*vec(1)].
  //

  // stores the indices of the candidate bins.
  bin_index_vector indices;

  // dimensions of the box currently being examined.
  int bin_lo[N], bin_hi[N], cur[N];

  point_to_bin( pt-radius, bin_lo );
  point_to_bin( pt+radius, bin_hi );

  scan_region( bin_lo, bin_hi, cur, 0, indices );

  // 2. Iterate over the list of points in the candidate bins and
  // output those in range.
  //
  typedef typename bin_index_vector::iterator ind_iter;
  typedef typename bin_type::const_iterator entry_iter;

  coord_type rad_sqr = radius * radius;
  for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
    bin_type const& bin = bins_[*bi];
    for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
      coord_type dist_sqr = vnl_vector_ssd( pt, ei->point_ );
      if ( dist_sqr < rad_sqr ) {
        values.push_back( ei->value_ );
        if ( points ) {
          points->push_back( ei->point_ );
        }
      }
    }
  }
}


template<unsigned N, typename C, typename V>
void
rsdl_bins<N,C,V>::
points_in_bounding_box_impl( point_type const& min_pt,
                             point_type const& max_pt,
                             vcl_vector< value_type >& values,
                             vcl_vector< point_type >* points ) const
{
  // 1. Find the list of bins that may contain the points we
  // want.
  //

  // stores the indices of the candidate bins.
  bin_index_vector indices;

  // dimensions of the box currently being examined.
  int bin_lo[N], bin_hi[N], cur[N];

  point_to_bin( min_pt, bin_lo );
  point_to_bin( max_pt, bin_hi );

  scan_region( bin_lo, bin_hi, cur, 0, indices );

  // 2. Iterate over the list of points in the candidate bins and
  // output those in range.
  //
  typedef typename bin_index_vector::iterator ind_iter;
  typedef typename bin_type::const_iterator entry_iter;

  for ( ind_iter bi = indices.begin(); bi != indices.end(); ++bi ) {
    bin_type const& bin = bins_[*bi];
    for ( entry_iter ei = bin.begin(); ei != bin.end(); ++ei ) {
      bool in_range = true;
      for ( unsigned d=0; d < N; ++d ) {
        if ( ei->point_[d] < min_pt[d] || ei->point_[d] > max_pt[d] ) {
          in_range = false;
          break;
        }
      }
      if ( in_range ) {
        values.push_back( ei->value_ );
        if ( points ) {
          points->push_back( ei->point_ );
        }
      }
    }
  }
}


//:
//
// This will scan the \a dim dimensional region bounded by
// lo[dim..N-1] and hi[dim..N-1], boundary points inclusive. It
// will return the indices of the bins that fall within the scanned
// region. The coordinates for the first dim-1 dimensions are given in
// \a cur.
//
// It will add the append the bins to \a indices. The return value is
// the total number of points in the bins that were appended.
//
// The routine will scan the \a dim dimensional equivalent of
// \verbatim
//    +-----------+
//    |           |
//    |           |
//    |           |
//    |           |
//    |           |
//    |           |
//    +-----------+
// \endverbatim
//
template<unsigned N, typename C, typename V>
unsigned
rsdl_bins<N,C,V>::
scan_region( int lo[N], int hi[N], int cur[N], unsigned dim,
             bin_index_vector& indices ) const
{
  unsigned found = 0;

  if ( dim==N ) {
    // 0d region is a point, so just check this point.
    bin_index_type ind = bin_index( cur );
    indices.push_back( ind );
    found = bins_[ind].size();
  } else {
    int bx = vcl_max( lo[dim], 0 );
    int ex = vcl_min( hi[dim]+1, size_[dim] );
    for ( int x=bx; x < ex; ++x ) {
      cur[dim] = x;
      found += scan_region( lo, hi, cur, dim+1, indices );
    }
  }

  return found;
}

#if 0

//:
//
// Similar to scan_region, but will only scan the boundary of the
// region, not the interior. That is, it will scan the dim dimensional
// equivalent of
// \verbatim
//    +-----------+
//    |           |
//    |  +-----+  |
//    |  |     |  |
//    |  |     |  |
//    |  +-----+  |
//    |           |
//    +-----------+
// \endverbatim
//
template<unsigned N, typename C, typename V>
unsigned
rsdl_bins<N,C,V>::
scan_bdy( int lo[N], int hi[N], int cur[N], unsigned dim,
          bin_index_vector& indices ) const
{
  unsigned found = 0;

  // There is no boundary in 0d, so we only need to do work for
  // dim < N
  //
  if ( dim < N ) {
    int bx = vcl_max( lo[dim], 0 );
    int ex = vcl_min( hi[dim]+1, size_[dim] );
    int x = bx;
    if ( x==lo[dim] ) {
      cur[dim] = x;
      found += scan_region( lo, hi, cur, dim+1, indices );
      ++x;
    }
    for ( ; x < ex-1; ++x ) {
      cur[dim] = x;
      found += scan_bdy( lo, hi, cur, dim+1, indices );
    }
    if ( x==hi[dim] ) {
      cur[dim] = x;
      found += scan_region( lo, hi, cur, dim+1, indices );
    }
  }

  return found;
}

#endif

// ---------------------------------------------------------------------------
//                                                              bin entry type
//


template<unsigned N, typename C, typename V>
rsdl_bins_bin_entry_type<N,C,V>::
rsdl_bins_bin_entry_type( point_type const& pt, const value_type val )
  : point_( pt ),
    value_( val )
{
}

template<unsigned N, typename C, typename V>
bool
rsdl_bins_bin_entry_type<N,C,V>::
equal( point_type const& pt, double tol_sqr ) const
{
  return vnl_vector_ssd( pt, point_ ) < tol_sqr;
}


// ---------------------------------------------------------------------------
//                                                            point dist entry
//

template<unsigned N, typename C, typename V>
rsdl_bins_point_dist_entry<N,C,V>::
rsdl_bins_point_dist_entry( point_type const& query_pt,
                            bin_entry_type const* entry )
 : entry_( entry ),
   sqr_dist_( vnl_vector_ssd( query_pt, entry->point_ ) )
{
}


template<unsigned N, typename C, typename V>
bool
rsdl_bins_point_dist_entry<N,C,V>::
operator<( rsdl_bins_point_dist_entry const& other ) const
{
  return this->sqr_dist_ < other.sqr_dist_;
}

#define INSTANTIATE_RSDL_BINS( n, V, C ) \
  template class rsdl_bins< n, V, C >; \
  template struct rsdl_bins_bin_entry_type< n, V, C >; \
  template struct rsdl_bins_point_dist_entry< n, V, C >

#endif // rsdl_bins_txx_
