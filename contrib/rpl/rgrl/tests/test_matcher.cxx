#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_2.h>

#include <rgrl/rgrl_matcher_k_nearest_boundary.h>
#include <rgrl/rgrl_feature_trace_pt.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_est_affine.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_matcher_sptr.h>

#include "test_util.h"

namespace {

  void test_matcher_k_nearest_boundary() {

    // create the transformation
    vnl_matrix<double> A( 2, 2 );
    vnl_vector<double> t( 2 );
    vnl_double_2 diff1(12.0, 0.0), diff2(0.0, 12.0);
    vnl_matrix<double> covar( 6, 6 );

    A(0,0) = 1.0;  A(0,1) = 0.0;
    A(1,0) = 0.0;  A(1,1) = 1.0;

    t[0] =  3.0;
    t[1] = -4.0;

    covar.set_identity();

    rgrl_transformation_sptr trans = new rgrl_trans_affine(A, t, covar);

    rgrl_mask_box roi(vnl_double_2(0, -50), vnl_double_2(50, 50));
    rgrl_estimator_sptr est_p = new rgrl_est_affine;
    rgrl_view_sptr view = new rgrl_view( roi, roi, roi, roi, est_p, trans, 0 );
    rgrl_scale_sptr scale = new rgrl_scale();

    // Create a vector of rgrl_features of the appropriate type.
    //
    vcl_vector<rgrl_feature_sptr> from_pts, to_pts;
    
    from_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(10.0, 20.0), 
                                                  vnl_double_2(0.0, 1.0),
                                                  5, 5));
    from_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(35.0, -20.0), 
                                                  vnl_double_2(1.0, 0.0), 
                                                  5, 5)); 
    to_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2( 25.0, 16.0), 
                                                vnl_double_2(0.0, 1.0),
                                                5, 5));
    to_pts.push_back(new rgrl_feature_trace_pt( vnl_double_2(38.0, -12.0), 
                                                vnl_double_2(-1.0, 0.0), 
                                                5, 5)); 

    // Create feature sets from the vector of features.
    //
    rgrl_feature_set_sptr from_set, to_set;
    from_set = new rgrl_feature_set_location<2>( from_pts );
    to_set = new rgrl_feature_set_location<2>( to_pts );

    rgrl_matcher_sptr matcher = new rgrl_matcher_k_nearest_boundary(1);
    rgrl_match_set_sptr match_set =  matcher->compute_matches( *from_set, *to_set,
                                                               *view, *trans, *scale);
    testlib_test_begin( "Correct number of matches" );
    testlib_test_perform( match_set->from_size() == 4 );
    
    vcl_vector<vnl_vector<double> > from_bd_locs;
    vcl_vector<vnl_vector<double> > to_bd_locs;
    vcl_vector<vnl_vector<double> > xformed_bd_locs;

    typedef rgrl_match_set::from_iterator FIter;
    typedef FIter::to_iterator TIter;

    for( FIter fi = match_set->from_begin(); fi != match_set->from_end(); ++fi ) {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        from_bd_locs.push_back( fi.from_feature()->location() );
        to_bd_locs.push_back( ti.to_feature()->location() );
        xformed_bd_locs.push_back(A*fi.from_feature()->location() +t);
      }
    }

    testlib_test_begin( "Correct boundary points" );
    testlib_test_perform( close( from_bd_locs[0], vnl_double_2(5, 20)) &&
                          close( from_bd_locs[1], vnl_double_2(15, 20)) );

    testlib_test_begin( "Correct boundary matches, same side" );
    testlib_test_perform( close( to_bd_locs[0], xformed_bd_locs[0]+diff1 ) && 
                          close( to_bd_locs[1], xformed_bd_locs[1]+diff1 ));
   
    testlib_test_begin( "Correct boundary matches, opposite side" );
    testlib_test_perform( close( to_bd_locs[2], xformed_bd_locs[2]+diff2 ) && 
                          close( to_bd_locs[3], xformed_bd_locs[3]+diff2 ));
   
  }
  
  
}

MAIN( test_matcher )
{
  START( "various matcher" );

  test_matcher_k_nearest_boundary();

  SUMMARY();
}
