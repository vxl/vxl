#ifndef rsdl_bins_h_
#define rsdl_bins_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// Provide an n-dimensional bin structure for fast point retrieval.

#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>

//: N-dimensional bin structure for fast point retrieval.
//
// This data structure stores point locations in larger bins so that
// neighbourhood queries can be performed efficiently by not having to
// consider the whole data set.
//
// The points are of type vnl_vector_fixed<\a CoordType, \a N>. With
// every point is associated a value, of type \a ValueType.
//
// The structure will store any point location, including those
// outside the range of the underlying bin storage. Such points are
// stored in the nearest available bin.
//
// Location equality is performed to within a tolerance (see
// set_distance_tolerance). This affects get_value, for
// example.
//
template<unsigned N, typename CoordType, typename ValueType>
class rsdl_bins
{
 public:
  //:
  typedef CoordType                       coord_type;

  //:
  typedef ValueType                       value_type;

  //:
  typedef vnl_vector_fixed<CoordType,N>   point_type;

 public:

  //:
  // Construct bins of size \a bin_sizes with enough bins to span \a
  // min_coord to \a max_coord.
  //
  // The equality comparision tolerance is set to 0.
  //
  rsdl_bins( point_type const& min_coord,
             point_type const& max_coord,
             point_type const& bin_sizes );

  //: Remove all points from the bins.
  void clear();

  //: Update the tolerance used in equality checking.
  //
  // Two points with Eucledian distance <= \a tol will comare equal.
  //
  void set_distance_tolerance( coord_type const& tol );

  //: Adds the point \a pt with value \a val.
  void add_point( point_type const& pt, value_type const& val );

  //: Retreives the value at \a pt, if possible.
  //
  // If the a point equal to \a pt, to within the current tolerance,
  // is found, \a val will be set to the associated value, and the
  // function will return true. Otherwise, the function will return
  // false.
  //
  // If multiple points compare equal, one will be arbitrarily
  // selected.
  //
  bool get_value( point_type const& pt, value_type& val ) const;

  //: Return the values of the \a n nearest neighbours.
  //
  // If the structure has fewer than \a n points, it will return all
  // the values stored.
  //
  void n_nearest( point_type const& pt,
                  unsigned n,
                  vcl_vector< value_type >& values ) const;

  //: Return the values and locations of the \a n nearest neighbours.
  //
  // If the structure has fewer than \a n points, it will return all
  // the points stored.
  //
  void n_nearest( point_type const& pt,
                  unsigned n,
                  vcl_vector< point_type >& points,
                  vcl_vector< value_type >& values  ) const;

  //: Check if there is at least one point within \a radius of \a pt.
  //
  bool is_any_point_within_radius( point_type const& pt,
                                   coord_type const& radius ) const;

  //: Return values of all the points within \a radius of \a pt.
  //
  void points_within_radius( point_type const& pt,
                             coord_type const& radius,
                             vcl_vector< value_type >& values ) const;

  //: Return values and locations of all the points within \a radius of \a pt.
  //
  void points_within_radius( point_type const& pt,
                             coord_type const& radius,
                             vcl_vector< point_type >& points,
                             vcl_vector< value_type >& values  ) const;

  //: Return values of all the points within the bounding box.
  //
  // The boundaries of the bounding box are included in the region
  // searched.
  //
  void points_in_bounding_box( point_type const& min_pt,
                               point_type const& max_pt,
                               vcl_vector< value_type >& values ) const;

  //: Return values and locations of all the points within the bounding box.
  //
  // The boundaries of the bounding box are included in the region
  // searched.
  //
  void points_in_bounding_box( point_type const& min_pt,
                               point_type const& max_pt,
                               vcl_vector< point_type >& points,
                               vcl_vector< value_type >& values  ) const;

 private:
  //:
  // Essentially a (location,value) pair with equality checking on the
  // location.
  //
  struct bin_entry_type;

  // See comment in .txx
  struct point_dist_entry;

  //:
  // Data stored at each bin
  //
  typedef vcl_vector< bin_entry_type >  bin_type;

  //:
  // The type of an index into the bins_ storage structure.
  //
  typedef typename bin_type::size_type bin_index_type;

  //:
  // A vector of indices into the bins_ storage structure.
  //
  typedef vcl_vector< bin_index_type > bin_index_vector;

  //:
  // Converts the coordinate \a x to a bin coordinate for dimension \a
  // d. Requires that min_pt_[d] and bin_size_[d] are initialized. The
  // resulting coordinate is not necessarily within the allocated
  // range.
  //
  int coord_to_bin( coord_type x, unsigned d ) const;

  //:
  // Converts the point to a set of bin indices.
  //
  void point_to_bin( point_type const& pt, int ind[N] ) const;

  //:
  // Convert the point into an index into the bin data structure bins_.
  //
  bin_index_type bin_index( point_type const& pt ) const;

  //:
  // Convert the bin coordinates into an index into the bin data
  // structure bins_.
  //
  bin_index_type bin_index( int bin[N] ) const;

  //:
  // The list of possible bin indices that could hold pt given the
  // current distance tolerance.
  bin_index_vector bin_indices( point_type const& pt ) const;


  //:
  // Implementation of n_nearest. See the documentation for that.
  //
  // This version will add the results to values, and, if points is
  // not null, to points.
  //
  void  n_nearest_impl( point_type const& pt,
                        unsigned n,
                        vcl_vector< value_type >& values,
                        vcl_vector< point_type >* points ) const;

  //:
  // Implementation of points_within_radius. See the documentation for
  // that.
  //
  // This version will add the results to values, and, if points is
  // not null, to points.
  //
  void points_within_radius_impl( point_type const& pt,
                                  coord_type const& radius,
                                  vcl_vector< value_type >& values,
                                  vcl_vector< point_type >* points ) const;

  //:
  // Implementation of points_in_bounding_box. See the documentation
  // for that.
  //
  // This version will add the results to values, and, if points is
  // not null, to points.
  //
  void points_in_bounding_box_impl( point_type const& min_pt,
                                    point_type const& max_pt,
                                    vcl_vector< value_type >& values,
                                    vcl_vector< point_type >* points ) const;

  // documentation in .txx
  unsigned scan_region( int lo[N], int hi[N], int cur[N], unsigned dim,
                        bin_index_vector& indices ) const;

  // documentation in .txx
  unsigned scan_bdy( int lo[N], int hi[N], int cur[N], unsigned dim,
                     bin_index_vector& indices ) const;


 private:
  //:
  // The number of bins along each dimension. The numbers are ints and
  // not unsigned ints because the logic for off-the-bottom is easier
  // if we allow -ve indices.
  //
  int size_[N];

  //:
  // The size of the bins along each dimension.
  //
  coord_type bin_size_[N];

  //:
  // The offsets for bin(0,0,0)
  coord_type min_pt_[N];

  //:
  // Distance tolerance used to compare if two points are the same.
  //
  coord_type dist_tol_;

  //:
  // Storage for all the bins.
  vcl_vector< bin_type > bins_;
};


//:
// This is just a (location,value) pair that has a
// equal-within-threshold function. Used in rsdl_bins to store the
// points in the bins.
//
template<unsigned N, typename CoordType, typename ValueType>
struct rsdl_bins<N,CoordType,ValueType>::bin_entry_type
{
  typedef rsdl_bins<N,CoordType,ValueType> bin_class;
  typedef typename bin_class::coord_type coord_type;
  typedef typename bin_class::value_type value_type;
  typedef typename bin_class::point_type point_type;

  bin_entry_type( point_type const& pt, const value_type val );

  inline bool equal( point_type const& pt, double tol_sqr ) const;

  point_type point_;
  value_type value_;
};


#endif // rsdl_bins_h_
