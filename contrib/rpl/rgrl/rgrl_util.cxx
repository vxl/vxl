//:
// \file
// \author Charlene Tsai

#include "rgrl_util.h"
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>

#include "rgrl_converge_status.h"
#include "rgrl_weighter.h"
#include "rgrl_estimator.h"
#include "rgrl_match_set.h"


// CHECK: only tested with affine transform
rgrl_mask_box 
rgrl_util_estimate_global_region( rgrl_mask_box const&         from_image_roi,
                                  rgrl_mask_box const&         to_image_roi,
                                  rgrl_mask_box const&         current_region, 
                                  rgrl_transformation const&   curr_xform, 
                                  bool                         union_with_curr)
{
  // Forward map 9 points - 4 corners, 4 side centers and 1 image center (27
  // points in 3D) of the from_image_roi. For each corner point q of the to_image_roi, 
  // find the closest forward_xformed point for initialized inverse_map for q.
  // Take the intersection of the from_image_roi and the inverse_xformed to_image_roi 
  // as the maximum region. If any q failed to converge or the region is null,
  // this procedure fails.
  //

  typedef vcl_vector<vnl_vector<double> > pt_vector;
  typedef pt_vector::iterator pt_iter;
  const double epsilon = 1;
  const double eps_squared = epsilon*epsilon;

  //1. compute the set of points from from_image_roi for forward mapping
  vnl_vector<double> from_x0 =  from_image_roi.x0();
  vnl_vector<double> from_x1 =  from_image_roi.x1();
  unsigned m = from_x0.size();
  rgrl_mask_box region = current_region;

  pt_vector from_pts;
  for (double i = from_x0[0]; i<= from_x1[0]; i+= (from_x1[0]-from_x0[0])/2) {
    for (double j = from_x0[1]; j<= from_x1[1]; j+= (from_x1[1]-from_x0[1])/2) {
      if (m == 3) {
        for (double k = from_x0[2]; k<= from_x1[2]; k+= (from_x1[2]-from_x0[2])/2) {
          vnl_vector<double> pt(3);
          pt[0] = i;
          pt[1] = j;
          pt[2] = k;
          from_pts.push_back(pt);
        }
      }
      else {
        vnl_vector<double> pt(2);
        pt[0] = i;
        pt[1] = j; 
        from_pts.push_back(pt);
      }
    }
  }

  //2. Forward map all the points in from_pts
  pt_vector to_pts;
  for ( pt_iter pitr = from_pts.begin();  pitr != from_pts.end(); ++pitr) {
    vnl_vector<double> to_pt = curr_xform.map_location(*pitr);
    to_pts.push_back(to_pt);
  }

  //3. Place the corner points of to_image_roi into a list
  vnl_vector<double> to_x0 =  to_image_roi.x0();
  vnl_vector<double> to_x1 =  to_image_roi.x1();
  pt_vector to_corner_pts;
  for (double i = to_x0[0]; i<= to_x1[0]; i+= to_x1[0]-to_x0[0]) {
    for (double j = to_x0[1]; j<= to_x1[1]; j+= to_x1[1]-to_x0[1]) {
      if (m == 3) {
        for (double k = to_x0[2]; k<= to_x1[2]; k+= to_x1[2]-to_x0[2]) {
          vnl_vector<double> pt(3);
          pt[0] = i;
          pt[1] = j;
          pt[2] = k;
          to_corner_pts.push_back(pt);
        }
      }
      else {
        vnl_vector<double> pt(2);
        pt[0] = i;
        pt[1] = j; 
        to_corner_pts.push_back(pt);
      }
    }
  }

  //4. For each corner point q of the to_image_roi, find the closest 
  //   forward_xformed point for initialized inverse_map for q.
  vnl_vector<double> inv_mapped_x0 = from_x1;
  vnl_vector<double> inv_mapped_x1 = from_x0;
  for ( pt_iter pitr = to_corner_pts.begin();  pitr != to_corner_pts.end(); ++pitr ) {
    double min_sqr_dist =  vnl_vector_ssd(to_pts[0], (*pitr));
    unsigned int min_index = 0;
    for (unsigned int i = 1; i<to_pts.size(); ++i) {
      double sqr_dist = vnl_vector_ssd(to_pts[i], (*pitr));
      if (sqr_dist < min_sqr_dist) {
        min_sqr_dist = sqr_dist;
        min_index = i;
      }
    }
    // use from_pts[min_index] as the initial guess for the inverse_mapped q
    vnl_vector<double> to_delta, from_next_est; //not used;
    vnl_vector<double> inv_mapped_pt = from_pts[min_index];
    curr_xform.inv_map(*pitr, false, to_delta, inv_mapped_pt, from_next_est);
    vnl_vector<double> fwd_mapp_pt = curr_xform.map_location(inv_mapped_pt);
    if (vnl_vector_ssd(fwd_mapp_pt, *pitr) > eps_squared) //didn't converge
      return region;

    //update the inv_mapped bounding box
    for( unsigned d=0; d < m; ++d ) {
      if (inv_mapped_pt[d] < inv_mapped_x0[d]) inv_mapped_x0[d] = inv_mapped_pt[d];
      if (inv_mapped_pt[d] > inv_mapped_x1[d]) inv_mapped_x1[d] = inv_mapped_pt[d];
    }
  }
 
  //5. Take the intersection of the from_image_roi and the inverse_xformed to_image_roi
  //   as the maximum region.
  vnl_vector<double> region_x0(m);
  vnl_vector<double> region_x1(m);

  for( unsigned d=0; d < m; ++d ) {
    region_x0[d] = (inv_mapped_x0[d] > from_x0[d])? inv_mapped_x0[d]:from_x0[d];
    region_x1[d] = (inv_mapped_x1[d] < from_x1[d])? inv_mapped_x1[d]:from_x1[d];
    if ( region_x0[d] > from_x1[d] )  region_x0[d] = from_x1[d];
    if ( region_x1[d] < from_x0[d] )  region_x0[d] = from_x0[d];
  }
  if (region_x0 == region_x1) //no overlap
    return region;

  //6. If union_with_curr set, union region and current_region to prevent oscillation
  if (union_with_curr) {
    for( unsigned d=0; d < m; ++d ) {
      if (region_x0[d] > current_region.x0()[d]) region_x0[d] = current_region.x0()[d];
      if (region_x1[d] < current_region.x1()[d]) region_x1[d] = current_region.x1()[d];
    }
  }

  //7. If the changes from current_region is insignificant, or the change is too 
  //   drastic, set region to be same ascurrent_region
  bool changed = 
    ( (region_x0 - current_region.x0()).inf_norm() > 1 ||
      (region_x1 - current_region.x1()).inf_norm() > 1);
  bool drastic_changed = false;
  double prev_space = 1, new_space = 1;
  vnl_vector<double>  prev_space_dim = current_region.x1() - current_region.x0();
  vnl_vector<double>  new_space_dim = region_x1 - region_x0;
  for( unsigned d=0; d < m; ++d ) {
    prev_space *= prev_space_dim[d];
    new_space  *= new_space_dim[d];
  }
  if ( union_with_curr && new_space/prev_space > 1.75 ) {
    drastic_changed = true;
  }

  if ( !changed || drastic_changed) {
    region_x0 = current_region.x0();
    region_x1 = current_region.x1();
  }

  region.set_x0(region_x0);
  region.set_x1(region_x1);

  return region;
}

double
rgrl_util_geometric_error_scaling( rgrl_match_set const& match_set )
{
  typedef rgrl_match_set::const_from_iterator FIter;

  if ( match_set.from_size() == 0 ) return 0.0; //geometric scaling not significant

  // The dimensionality of the space we are working in. Find it by
  // looking at the dimension of one of the data points.
  //
  int m;
  m = match_set.from_begin().from_feature()->location().size();

  // Compute the centers of the from_feature_set and the mapped_feature_set
  //
  vnl_vector<double> from_centre( m, 0.0 );
  vnl_vector<double> mapped_centre( m, 0.0 );
  for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
    from_centre += fi.from_feature()->location();
    mapped_centre += fi.mapped_from_feature()->location();
  }
  from_centre /= match_set.from_size();
  mapped_centre /=  match_set.from_size();
  
  // Compute the covariance matrices
  //
  vnl_matrix<double> cov_matrix_from(m,m,0.0);
  vnl_matrix<double> cov_matrix_mapped(m,m,0.0);
  for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
    cov_matrix_from += 
      outer_product(fi.from_feature()->location() - from_centre,
                    fi.from_feature()->location() - from_centre);
    cov_matrix_mapped += 
      outer_product(fi.mapped_from_feature()->location() - mapped_centre,
                    fi.mapped_from_feature()->location() - mapped_centre);
  }
  cov_matrix_from /= match_set.from_size();
  cov_matrix_mapped /=  match_set.from_size();

  // Perform SVD to get the 1st and 2nd eigenvalues.
  //
  vnl_svd<double> svd_from (cov_matrix_from );
  vnl_svd<double> svd_mapped (cov_matrix_mapped );
  double ev_fst_from = svd_from.W(0);
  double ev_snd_from = svd_from.W(1);
  double ev_fst_mapped = vnl_math_max( svd_mapped.W(0), 1e-16 );
  double ev_snd_mapped = vnl_math_max( svd_mapped.W(1), 1e-16 );

  // Estimate the change in the spread of the feature set
  //
  double change_in_fst = vnl_math_max(ev_fst_from/ev_fst_mapped, 
                                      ev_fst_mapped/ev_fst_from);
  double change_in_snd = vnl_math_max(ev_snd_from/ev_snd_mapped, 
                                      ev_snd_mapped/ev_snd_from);

  double scaling_sqr = vnl_math_max( change_in_fst, change_in_snd );
  double scaling = vcl_sqrt(scaling_sqr);

  return scaling;

  /*
  // The new formulation, measures the changes of the ratio of te 2nd
  // moments
  double ratio_from = ev_fst_from/ev_snd_from;
  double ratio_mapped = ev_fst_mapped/ev_snd_mapped;
  double distortion = vnl_math_max( ratio_from/ratio_mapped,
                                    ratio_mapped/ratio_from );

  return distortion;
  */
}

double
rgrl_util_geometric_error_scaling( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets )
{
  // Take the max geometric scaling of the set
  //
  double geometric_scaling = 0;
  for( unsigned ds=0; ds < current_match_sets.size(); ++ds ) {
    geometric_scaling = vnl_math_max(geometric_scaling, 
                                     rgrl_util_geometric_error_scaling(*current_match_sets[ds]) );
  }

  return geometric_scaling;
}

void
rgrl_util_extract_region_locations( vnl_vector< double >             const& center,
				    vcl_vector< vnl_vector<double> > const& basis_dirs,
				    vnl_vector< double >             const& basis_radii,
				    vcl_vector< vnl_vector<int> >         & pixel_locations )
{
  //  0. Get the image dimension, clear the pixels, and make sure
  //  everyone agrees on the sizes.
  unsigned int dimension = center.size();
  pixel_locations.clear();
  assert( center.size() == basis_dirs.size() );
  assert( center.size() == basis_dirs[0].size() );
  assert( center.size() == basis_radii.size() );

  //  1. We need to get the bounding box in axis-aligned coordinates.  We
  //  therefore need to get the corners of the rotated box in these
  //  coordinates...  

  //  1a. We start by forming a vector of corner points.  There will be
  //  2^dimension corners.   

  vcl_vector< vnl_vector<double> > corner_points;
  int num_corners = vnl_math_rnd( vcl_exp( dimension * vcl_log(2.0) ));
  corner_points.reserve( num_corners );

  //  1b. Since the dimension is computed dynamically, we can't do the
  //  obvious thing of forming for loops, with one loop per dimension.
  //  Instead we build up the corner vectors one dimension at a time.
  //  Each dimension is built by taking the partially-built vectors
  //  from the previous dimensions and adding +/- r * dir to each
  //  vector.  This doubles the number of vectors stored in each
  //  iteration of the outer loop.  This is an iterative version of
  //  what would normally be at least designed as a recursive
  //  procedure. 

//   vnl_vector<double> zero_vector( dimension, 0.0 );
//   corner_points.push_back( zero_vector );
  corner_points.push_back( center );

  for ( unsigned int i=0; i<dimension; ++i )
    {
      // 1b(i). For each current / partially built corner vector, create a
      // new corner by adding r * dir, then add -r * dir to the
      // current vector.

      vnl_vector<double> offset = basis_dirs[i] * basis_radii[i];
      unsigned curr_size = corner_points.size();
      for ( unsigned int j=0; j<curr_size; ++j )
	{
	  corner_points.push_back( corner_points[j] + offset );
	  corner_points[j] -= offset;
	}
    }

  //  1c. Form a bounding box by taking the min and max over the
  //  corners.  At this point, the bounding box is in centered 
  //  coordinates, not in absolute coordinates.

  vnl_vector<double> lower = center; 
  vnl_vector<double> upper = center;

  for ( unsigned int i=0; i<corner_points.size(); ++i )
    {
      for ( unsigned int j=0; j<dimension; ++j )
	{
	  if ( lower[j] > corner_points[i][j] )
	    lower[j] = corner_points[i][j];
	  if ( upper[j] < corner_points[i][j] )
	    upper[j] = corner_points[i][j];
	}
    }

  //  2. Ok, folks, time to build the intervals of possible points.
  //  How to do this generalized over dimension?  An interval is an
  //  axis-aligned line segment.  Each line segments has dimension-1
  //  coordinates fixed.  Arbitrarily, we'll decide that these
  //  coordinates are the first dimension-1 coordinates.  Therefore,
  //  our first goal in building the intervals is to enumerate all
  //  possible combinations of dimension-1 coordinates from the
  //  bounding boxes.  As examples, in 2d this  will contain just the
  //  possible x values (lower[0] .. lower[1]), in 3d this will
  //  contain all possible x,y values.   

  //  2a. Find an approximate upper bound on the number of
  //  intervals.  This is used for nothing more than to reserve space
  //  in a vector.

  int max_num_intervals = 1;
  for ( unsigned int i=0; i < dimension-1; ++i ) 
    max_num_intervals *= (int)vcl_floor( upper[i] - lower[i] + 1 );

  //  2b. Allocate a vector of interval indices.  Note that in each
  //  of these, the last component will be 0.0.  It is important that
  //  it stay this way for the computation below (step 3).  Also the
  //  interval_indices will be stored as doubles even though they are
  //  really integers.  This just simplifies some computation. 

  vcl_vector< vnl_vector<double> > interval_indices;
  interval_indices.reserve( max_num_intervals );

  //  2c. The procedure is similar to 1b above.  We are after all
  //  combinations of the possible indices from the first dimension-1
  //  axis-aligned coordinates.  Once we have these, in step 3 we will
  //  build the intervals by finding the min and max bounds on the
  //  line segments as they intersect the rotated rectangular
  //  structure.  (Some will not.)  In the actual procedure, the set
  //  of coordinate combinations is built up one dimension at a time.
  //  In each step, the integer values in the bounding box (remember,
  //  it is axis-aligned) are combined with each in the previous
  //  dimension.  

  //   vnl_vector< double > zero_vect( dimension, 0.0 );  // basis
  //   interval_indices.push_back( zero );
  interval_indices.push_back( center );

  for ( unsigned int i=0; i < dimension-1; ++i )
  {
    // round or floor/ceil?
    //       int lower_index = vnl_math_rnd( lower[i] );
    //       int upper_index = vnl_math_rnd( upper[i] );
    int lower_index = (int)vcl_ceil( lower[i] );
    int upper_index = (int)vcl_floor( upper[i] );
    int prev_size = interval_indices.size();

    //  2c(i).  For the current dimension, expand the set of
    //  coordinates of the axis-aligned line segments.  Go through
    //  each 
    for ( int prev=0; prev < prev_size; ++prev )
    {
      vnl_vector< double > partial_interval = interval_indices[prev];
      interval_indices[ prev ][ i ] = double(lower_index);
      for ( int index = lower_index + 1; index <= upper_index; ++index )
      {
        partial_interval[ i ] = double(index);  // double for convenience
        interval_indices.push_back( partial_interval );
      }

    }
  }

  //  3. Now, we are ready to get the region and the pixel locations
  //  within the region.  For each interval, compute the min and max
  //  value of the last coordinate.  If it includes any pixels, record
  //  these pixel locations and their associated intensities.

  for ( unsigned int i=0; i < interval_indices . size(); ++i )
  {
    vnl_vector<double> & interval = interval_indices[i];

    //  3a. Initialize the bounds to the outer bounds of the
    //  rectangle.  The computation will tighten these bounds.
    double min_z = lower[ dimension-1 ], max_z = upper[ dimension-1 ];

    //  3b. Go through each of the basis vectors for the region
    //  (oriented).  For each of these, compute the bounds and use
    //  them to narrow the bounds on the line segment.  In effect,
    //  we are intersecting the line with the two infinite planes in
    //  direction + and - the basis vector, at distance r.  Overall,
    //  remember that we are computing the intersection between the
    //  infinite line aligned with the first dimension-1 axes at
    //  location "interval" with the oriented rectangular solid.

    for ( unsigned int basis_index=0; basis_index<dimension; ++basis_index )
    {
      // for calculating max_z and min_z correctly, let bdir[ dimension - 1 ] >= 0.
      vnl_vector< double > bdir = 
        ( basis_dirs[ basis_index ][ dimension - 1 ] < 0 ) ? 
        basis_dirs[ basis_index ] / -1 : basis_dirs[ basis_index ];
      double bradius = basis_radii[ basis_index ];

      //  3b(i). If the last dimension of this basis vector is
      //  nearly 0, then this basis vector will have little effect
      //  on the interval.  Ignore it to avoid problems caused by
      //  numerical issues.

      if ( vnl_math_abs( bdir[ dimension-1 ] ) <= 1.0E-6 ) continue;

      //  3b(ii).  Project the interval, which has a 0 in the last
      //  dimension, onto the basis vector.

      double projection = dot_product( interval - center, bdir );

      //  3c(iii). Solve for the min_end based on this basis vector.  If
      //  this is larger than the current min_z, then update the
      //  min.  Do the same for the max_end / max_z.

      double basis_min_end = ( -bradius - projection ) / bdir[ dimension-1 ] + center[ dimension - 1 ] ;
      if ( min_z < basis_min_end ) min_z = basis_min_end;
      double basis_max_end = ( bradius - projection ) / bdir[ dimension-1 ] + center[ dimension - 1 ] ;
      if ( max_z > basis_max_end ) max_z = basis_max_end;
    }

    //  3d. If the interval can not contain any points (even after
    //  rounding), skip the rest of the computation and go on to the
    //  next interval. 
    //       if ( max_z < min_z-1 ) continue;

    //  3e. Form the first dimension-1 components of the pixel index by
    //  adding the center location.

    vnl_vector< int > pixel_index( dimension, 0 );
    // change to integer
    for ( unsigned int d=0; d<dimension-1; ++d ) {
      pixel_index[d] = (int)interval[d] ;
      assert( pixel_index[ d ] == interval[ d ] );
    }

    //  3f. Go through the interval specified by the range of last
    //  components to form the final, complete set of indices for
    //  this line segment.

    // round or floor/ceil?
    //       int last_lower_bound = vnl_math_rnd( min_z + center[ dimension-1 ] );
    //       int last_upper_bound = vnl_math_rnd( max_z + center[ dimension-1 ] );
    int last_lower_bound = (int)vcl_ceil( min_z );
    int last_upper_bound = (int)vcl_floor( max_z );
    for ( int last_component = last_lower_bound; last_component <= last_upper_bound;
          ++ last_component )
      {
        pixel_index[ dimension - 1 ] = last_component;
        pixel_locations.push_back( pixel_index );
      }
  }
} 

bool
rgrl_util_irls( rgrl_match_set_sptr              match_set,
                rgrl_scale_sptr                  scale,
                rgrl_weighter_sptr               weighter,
                rgrl_convergence_tester   const& conv_tester,
                rgrl_estimator_sptr              estimator,
                rgrl_transformation_sptr       & estimate,
                double                         & error,
                unsigned int                     debug_flag )
{
  rgrl_set_of<rgrl_match_set_sptr> match_sets;
  match_sets.push_back( match_set );
  rgrl_set_of<rgrl_scale_sptr> scales;
  scales.push_back( scale );
  vcl_vector<rgrl_weighter_sptr> weighters;
  weighters.push_back(weighter);
  
  return rgrl_util_irls(match_sets, scales, weighters,
                        conv_tester, estimator, estimate, error, debug_flag);
}

bool 
rgrl_util_irls( rgrl_set_of<rgrl_match_set_sptr> const& match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& scales,
                vcl_vector<rgrl_weighter_sptr>   const& weighters,
                rgrl_convergence_tester          const& conv_tester,
                rgrl_estimator_sptr              estimator,
                rgrl_transformation_sptr&        estimate,
                double&                          total_error,
                unsigned int                     debug_flag )
{
  DebugFuncMacro( debug_flag, 1, " In irls for model "<<estimator->transformation_type().name()<<"\n" );

  typedef rgrl_match_set::from_iterator  from_iter;
  typedef from_iter::to_iterator         to_iter;

  unsigned int iteration = 0;
  unsigned int max_iterations = 25;
  bool failed = false;

  //  Basic loop:
  //  1. Calculate new estimate
  //  2. Map matches and calculate weights 
  //  3. Test for convergence.
  //

  // initialize the weights for the first estimation
  rgrl_converge_status_sptr current_status = 0;
  for( unsigned ms=0; ms < match_sets.size(); ++ms ) {
    rgrl_match_set_sptr match_set = match_sets[ms];
    if (match_set->from_size() > 0) {
      match_set->remap_from_features( *estimate );
      weighters[ms]->compute_weights( *scales[ms], *match_set );
    }
  }
  do {
    // Step 1.  Calculate new estimate.
    //
    rgrl_transformation_sptr
      new_estimate = estimator->estimate(match_sets, *estimate);
    if ( !new_estimate ) {
      estimate = 0;
      DebugFuncMacro( debug_flag, 1, "*** irls failed!\n" );
      return failed;
    }

    //  Step 2.  Map matches and calculate weights 
    //
    for( unsigned ms=0; ms < match_sets.size(); ++ms ) {
      rgrl_match_set_sptr match_set = match_sets[ms];
      if (match_set->from_size() > 0) {
        match_set->remap_from_features( *new_estimate );
        weighters[ms]->compute_weights( *scales[ms], *match_set );
      }
    }

    //  Step 3.  Test for convergence. The alignment error is not
    //  scaled by the distortion.
    //
    current_status = conv_tester.compute_status( current_status,
                                                 new_estimate, estimator,
                                                 match_sets, scales, false);

    DebugFuncMacro_abv(debug_flag, 2, "irls: (iteration = " << iteration << ") oscillation count = " << current_status->oscillation_count() << "\n" );
    DebugFuncMacro_abv(debug_flag, 2, "irls: error = " << current_status->error() << vcl_endl );
    DebugFuncMacro_abv(debug_flag, 2, "irls: error_diff = " << current_status->error_diff() << vcl_endl );
    DebugFuncMacro_abv(debug_flag, 2, "irls: converged = " << current_status->has_converged() << vcl_endl );

    estimate = new_estimate;
    ++ iteration;
  } while ( !current_status->has_converged() &&
            !current_status->has_stagnated() && 
            iteration < max_iterations );

  DebugFuncMacro_abv(debug_flag, 1, "irls status: " << 
                     ( ( current_status->has_converged() )? 
                       "converged" : ( ( current_status->has_stagnated() )? 
                                       "stagnated" : "reaches max iteration\n" ) ) );
  
  // Compute the total alignment error as the sum of the weighted
  // residual squares
  //
  total_error = 0;
  for( unsigned ms=0; ms < match_sets.size(); ++ms ) {
    rgrl_match_set_sptr match_set = match_sets[ms];
    //  for each from image feature being matched
    for( from_iter fitr = match_set->from_begin(); fitr != match_set->from_end(); ++fitr ){
      if( fitr.size() == 0 )  continue;

      rgrl_feature_sptr mapped_from = fitr.from_feature()->transform( *estimate );
      for( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr ) {
        //  for each match with a "to" image feature
        rgrl_feature_sptr to_feature = titr.to_feature();
        double geometric_err = to_feature->geometric_error( *mapped_from );

        total_error += vnl_math_sqr(geometric_err) * titr.geometric_weight();
      }
    }
  }

  DebugFuncMacro_abv( debug_flag, 1, " Total alignment error = " << total_error << vcl_endl );

  return !failed;
}

//: skip empty lines in input stream
void 
rgrl_util_skip_empty_lines( vcl_istream& is )
{
  vcl_streampos pos;
  vcl_string str;
  const static vcl_string white_chars = " \t";
  int non_empty_pos; 
  
  // skip any empty lines  
  do{
    // store current reading position
    pos = is.tellg();
    str = "";
    vcl_getline( is, str );
    
    non_empty_pos = str.find_first_not_of( white_chars );
  }while( str.empty() || non_empty_pos < 0 );
  
  // back to the beginning of non-empty line
  is.seekg( pos );
}  
